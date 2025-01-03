#include "ASTDeclNode.h"
#include "ParseError.h"
#include "loguru.hpp"
#include <exception>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <vector>

#include "ASTProgram.h"
#include "FrontEnd.h"
#include "SemanticAnalysis.h"

#include "LSPState.h"
#include "RPC.h"
#include "messages/DocumentFormattingRequest.h"
#include "messages/HoverRequest.h"
#include "messages/PublishDiagnosticsNotification.h"
#include "messages/messages.h"

using json = nlohmann::json;

void handleMessage(LSPState &state, std::string method,
                   std::vector<uint8_t> contents) {
  LOG_S(INFO) << "Recieved method: " << method;

  nlohmann::json jsonContent;
  try {
    jsonContent = nlohmann::json::parse(contents.begin(), contents.end());
  } catch (const std::exception &e) {
    throw std::invalid_argument(std::string("Failed to parse JSON: ") +
                                e.what());
  }

  if (method == "initialize") {
    InitializeRequest request = jsonContent.get<InitializeRequest>();

    InitializeResponse response = newInitializeResponse(request.id);
    std::string message = EncodeMessage(response);
    std::cout << message;
    LOG_S(INFO) << "Connected to: " << request.params.clientInfo->name << " "
                << request.params.clientInfo->version;

  } else if (method == "textDocument/didOpen") {
    DidOpenTextDocumentNotification request =
        jsonContent.get<DidOpenTextDocumentNotification>();

    state.openDocument(request.params.textDocument.uri,
                       request.params.textDocument.text);
    LOG_S(INFO) << "Opened: " << request.params.textDocument.uri;

  } else if (method == "textDocument/didChange") {
    DidChangeTextDocumentNotification request =
        jsonContent.get<DidChangeTextDocumentNotification>();

    for (const TextDocumentContentChangeEvent changeEvent :
         request.params.contentChanges) {
      auto error = state.updateDocument(request.params.textDocument.uri,
                                        changeEvent.text);
      PublishDiagnosticsNotification diagnosticsNotification;
      if (error) {
        diagnosticsNotification = newPublishDiagnosticsNotificationError(
            request.params.textDocument, error.value().what());
      } else {
        diagnosticsNotification =
            newPublishDiagnosticsNotificationEmpty(request.params.textDocument);
      }
      std::string message = EncodeMessage(diagnosticsNotification);
      std::cout << message;
    }
    LOG_S(INFO) << "Changed: " << request.params.textDocument.uri;

  } else if (method == "textDocument/hover") {
    HoverRequest request = jsonContent.get<HoverRequest>();
    HoverResponse response = state.hover(request);
    std::string message = EncodeMessage(response);
    std::cout << message;

  } else if (method == "textDocument/formatting") {
    DocumentFormattingRequest request =
        jsonContent.get<DocumentFormattingRequest>();
    std::string uri = request.params.textDocument.uri;
    auto ast = state.getUpdatedAst(request.params.textDocument.uri);
    DocumentFormattingResponse response;
    if (ast) {
      std::stringstream output;
      FrontEnd::prettyprint(ast.value().get(), output);
      response = newDocumentFormattingResponse(
          request.id, state.getDocument(uri), output.str());
    } else {
      response = newNullDocumentFormattingResponse(request.id);
    }
    std::string message = EncodeMessage(response);
    std::cout << message;
  }
}

class StreamParser {
public:
  explicit StreamParser(std::istream &input) : inputStream(input) {}

  std::string getNextToken() {
    while (true) {
      read();

      auto token = parseBuffer();
      if (!token.empty()) {
        return token;
      }

      if (inputStream.eof()) {
        throw std::runtime_error("Incomplete message at end of input");
      }
    }
  }

private:
  std::istream &inputStream;
  std::string buffer;
  size_t expectedSize = 0;

  void read() {
    char data[1];
    inputStream.read(data, sizeof(data));
    buffer.append(data, inputStream.gcount());
  }

  std::string parseBuffer() {
    static const std::string separator = "\r\n\r\n";

    size_t separatorPos = buffer.find(separator);
    if (separatorPos == std::string::npos) {
      return ""; // Not enough data to determine a token
    }

    std::string header = buffer.substr(0, separatorPos);
    std::string content = buffer.substr(separatorPos + separator.size());

    static const std::string contentLengthPrefix = "Content-Length: ";
    size_t contentLengthPos = header.find(contentLengthPrefix);
    if (contentLengthPos == std::string::npos) {
      throw std::runtime_error("Missing Content-Length header");
    }
    contentLengthPos += contentLengthPrefix.size();

    std::string contentLengthStr = header.substr(contentLengthPos);
    size_t contentLength = std::stoi(contentLengthStr);

    expectedSize = separatorPos + separator.size() + contentLength;
    if (buffer.size() < expectedSize) {
      return "";
    }

    std::string token = buffer.substr(0, expectedSize);
    buffer = buffer.substr(expectedSize);
    expectedSize = 0;
    return token;
  }
};

void startLsp(std::istream &in, std::ostream &out) {
  LOG_S(INFO) << "Starting tip lsp";
  std::string input;

  StreamParser parser(in);
  LSPState state;

  while (true) {
    try {
      LOG_S(INFO) << "attempting to get token";
      std::string line = parser.getNextToken();
      std::vector<uint8_t> msg(line.begin(), line.end());

      auto [method, contents] = DecodeMessage(msg);

      handleMessage(state, method, contents);
    } catch (const std::exception &e) {
      LOG_S(ERROR) << "LSP Error " << e.what();
      break;
    }
  }
}

// TODO: Implement proper arguments/help
int main(int argc, char *argv[]) {
  loguru::g_stderr_verbosity = 0;
  loguru::g_preamble = false;
  loguru::g_preamble_date = false;
  loguru::g_preamble_time = false;
  loguru::g_preamble_uptime = false;
  loguru::g_preamble_thread = false;
  loguru::g_colorlogtostderr = false;

  if (argc > 1) {
    loguru::add_file(argv[1], loguru::Append, loguru::Verbosity_INFO);
  }

  startLsp(std::cin, std::cout);
  return 0;
}
