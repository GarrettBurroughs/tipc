#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "ASTDeclNode.h"
#include "ParseError.h"
#include "loguru.hpp"


#include "ASTProgram.h"
#include "FrontEnd.h"
#include "SemanticAnalysis.h"

#include "RPC.h"
#include "LSPState.h"
#include "messages/HoverRequest.h"
#include "messages/messages.h"

using json = nlohmann::json;

void handleMessage(LSPState& state, std::string method, std::vector<uint8_t> contents) {
    LOG_S(INFO) << "Recieved method: " << method;

    nlohmann::json jsonContent;
    try {
        jsonContent = nlohmann::json::parse(contents.begin(), contents.end());
    } catch (const std::exception& e) {
        throw std::invalid_argument(std::string("Failed to parse JSON: ") + e.what());
    }

    if (method == "initialize") {
        InitializeRequest request = jsonContent.get<InitializeRequest>();

        InitializeResponse result = newInitializeResponse(request.id);
        std::string message = EncodeMessage(result);
        std::cout << message;
        LOG_S(INFO) << "Connected to: " << request.params.clientInfo->name << " " << request.params.clientInfo->version;

    } else if (method == "textDocument/didOpen") {
        DidOpenTextDocumentNotification request = jsonContent.get<DidOpenTextDocumentNotification>();

        state.openDocument(request.params.textDocument.uri, request.params.textDocument.text);
        LOG_S(INFO) << "Opened: " << request.params.textDocument.uri;

    } else if (method == "textDocument/didChange") {
        DidChangeTextDocumentNotification request = jsonContent.get<DidChangeTextDocumentNotification>();

        for (const TextDocumentContentChangeEvent changeEvent : request.params.contentChanges) {
            state.updateDocument(request.params.textDocument.uri, changeEvent.text);
        }
        try {
            std::stringstream program(state.getDocument(request.params.textDocument.uri));
            FrontEnd::parse(program);
            auto diagnosticNotification = newPublishDiagnosticsNotificationEmpty(request.params.textDocument);
            std::string message = EncodeMessage(diagnosticNotification);
            std::cout << message;
        } catch (ParseError e) {
            auto diagnosticNotification = newPublishDiagnosticsNotificationError(request.params.textDocument, e.what());
            std::string message = EncodeMessage(diagnosticNotification);
            std::cout << message;
        }
        LOG_S(INFO) << "Changed: " << request.params.textDocument.uri;

        // try {
        //     std::stringstream inputStream(state.getDocument(request.params.textDocument.uri));
        //     std::shared_ptr<ASTProgram> ast = FrontEnd::parse(inputStream);
        //     auto analysisResults = SemanticAnalysis::analyze(ast.get(), false);
        //     auto symbolTable = analysisResults->getSymbolTable();
        //     auto mainFn = symbolTable->getFunction("main");
        //     auto xVar = symbolTable->getLocal("x", mainFn);
        //     auto typeResults = analysisResults->getTypeResults();
        //     auto mainType = typeResults->getInferredType(mainFn);
        //     auto xType = typeResults->getInferredType(xVar);
        //     LOG_S(INFO) << *mainType;
        //     LOG_S(INFO) << *xType;
        // } catch (std::exception e) {
        //     LOG_S(INFO) << e.what();
        // }
    } else if (method == "textDocument/hover") {
        HoverRequest request = jsonContent.get<HoverRequest>();
        try {
            HoverResponse result = state.hover(request);
            std::string message = EncodeMessage(result);
            std::cout << message;
        } catch (std::exception e) {
            HoverResponse result = newHoverResponse(request.id, "");
            std::string message = EncodeMessage(result);
            std::cout << message;
        }
    }
}

class StreamParser {
public:
    explicit StreamParser(std::istream& input) : inputStream(input) {}

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
    std::istream& inputStream;
    std::string buffer;
    size_t expectedSize = 0;

    // TODO: Figure out if we can read more data at a time? 
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

void startLsp(std::istream& in, std::ostream& out) { 
    LOG_S(INFO) << "Starting tip lsp";
    std::string input; 


    StreamParser parser(in);
    LSPState state;

    while (true) {
        try {
            LOG_S(INFO) << "attempting to get token";
            std::string line = parser.getNextToken();
            std::vector<uint8_t> msg(line.begin(), line.end());

            // LOG_S(INFO) << "Token: " << line;
            auto [method, contents] = DecodeMessage(msg);

            handleMessage(state, method, contents);
        } catch (const std::exception& e) {
            LOG_S(ERROR) << "LSP Error " << e.what();
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    loguru::g_stderr_verbosity = 0;
    loguru::g_preamble = false;
    loguru::g_preamble_date = false;
    loguru::g_preamble_time = false;
    loguru::g_preamble_uptime = false;
    loguru::g_preamble_thread = false;
    loguru::g_colorlogtostderr = false;

    // TODO: Remove this log file when finished (or add option for enabling loggin)
    loguru::add_file("/home/gburroughs/dev/tipc_test/log.txt", loguru::Append, loguru::Verbosity_INFO);
    startLsp(std::cin, std::cout);
    return 0;
}

