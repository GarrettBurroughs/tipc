#include "messages.h"
#include "PublishDiagnosticsNotification.h"
#include "TextDocument.h"
#include <optional>

#define TIPLSP_VERSION "0.0.1"
enum DiagnosticSeverity {
  Error = 1,
  Warning = 2,
  Information = 3,
  Hint = 4,
};

InitializeResponse newInitializeResponse(int id) {
  return InitializeResponse{
      .jsonrpc = "2.0",
      .id = id,
      .result = InitializeResult{
          .capabilities = ServerCapabilities{.textDocumentSync = 1,
                                             .hoverProvider = true,
                                             .definitionProvider = false,
                                             .codeActionProvider = false,
                                             .documentFormattingProvider = true,
                                             .completionProvider = {}},
          .serverInfo =
              ServerInfo{.name = "tiplsp", .version = TIPLSP_VERSION}}};
};

HoverResponse newHoverResponse(int id, std::string contents) {
  return HoverResponse{
      .jsonrpc = "2.0", .id = id, .result = HoverResult{.contents = contents}};
};

PublishDiagnosticsNotification newPublishDiagnosticsNotificationError(
    VersionedTextDocumentIdentfier textDocument, std::string error) {

  size_t atPos = error.find('@');
  if (atPos == std::string::npos) {
    return PublishDiagnosticsNotification{
        "2.0", "textDocument/publishDiagnostics",
        PublishDiagnosticsParams{
            textDocument.uri,
            textDocument.version,
            {Diagnostic{Range{Position{0, 0}, Position{0, 0}}, 1, error}}}};
  }
  std::string numbersPart = error.substr(atPos + 1);
  size_t commaPos = numbersPart.find(',');
  int line = std::stoi(numbersPart.substr(0, commaPos)) - 1;
  int column = std::stoi(numbersPart.substr(commaPos + 1));
  std::string errorMsg = error.substr(0, atPos);

  return PublishDiagnosticsNotification{
      .jsonrpc = "2.0",
      .method = "textDocument/publishDiagnostics",
      .params =
          PublishDiagnosticsParams{
              .uri = textDocument.uri,
              .version = textDocument.version,
              .diagnostics = {Diagnostic{
                  .range = Range{.start = Position{line, column},
                                 .end = Position{line, column}},
                  .severity = Error,
                  .message = errorMsg}}}

  };
}

PublishDiagnosticsNotification newPublishDiagnosticsNotificationEmpty(
    VersionedTextDocumentIdentfier textDocument) {

  return PublishDiagnosticsNotification{
      .jsonrpc = "2.0",
      .method = "textDocument/publishDiagnostics",
      .params = PublishDiagnosticsParams{.uri = textDocument.uri,
                                         .version = textDocument.version,
                                         .diagnostics = {}}

  };
}

std::pair<int, int> findEndPosition(const std::string &contents) {
  std::istringstream stream(contents);
  std::string line;
  int row = 0;
  int charPosition = 0;

  // Process each line in the string
  while (std::getline(stream, line)) {
    row++;
    charPosition = line.size(); // Update to the length of the last line
  }

  return {row, charPosition};
}

DocumentFormattingResponse newDocumentFormattingResponse(int id,
                                                         std::string original,
                                                         std::string contents) {

  auto [endRow, endChar] = findEndPosition(original);
  auto formattedDocument = {TextEdit{Range{
                                         Position{0, 0},
                                         Position{endRow, endChar},
                                     },
                                     contents}};
  return DocumentFormattingResponse{
      .jsonrpc = "2.0", .id = id, .result = formattedDocument};
}

DocumentFormattingResponse newNullDocumentFormattingResponse(int id) {
  return DocumentFormattingResponse{
      .jsonrpc = "2.0", .id = id, .result = std::nullopt};
}
