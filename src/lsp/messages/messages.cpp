#include "messages.h"
#include "PublishDiagnosticsNotification.h"
#include "TextDocument.h"
#include <optional>

InitializeResponse newInitializeResponse(int id) {
  return InitializeResponse{
      "2.0", id,
      InitializeResult{ServerCapabilities{
                           1,     // textDocumentSync
                           true,  // hoverProvider
                           false, // definitionProvide
                           false, // codeActionProvider
                           true,  // documentFormattingProvider
                       },
                       ServerInfo{"tiplsp", "0.0.1"}}};
};

HoverResponse newHoverResponse(int id, std::string contents) {
  return HoverResponse{"2.0", id, HoverResult{contents}};
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
      "2.0", "textDocument/publishDiagnostics",
      PublishDiagnosticsParams{
          textDocument.uri,
          textDocument.version,
          {Diagnostic{Range{Position{line, column}, Position{line, column}}, 1,
                      errorMsg}}}

  };
}

PublishDiagnosticsNotification newPublishDiagnosticsNotificationEmpty(
    VersionedTextDocumentIdentfier textDocument) {

  return PublishDiagnosticsNotification{
      "2.0", "textDocument/publishDiagnostics",
      PublishDiagnosticsParams{textDocument.uri, textDocument.version, {}}

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
  return DocumentFormattingResponse{"2.0", id, formattedDocument};
}

DocumentFormattingResponse newNullDocumentFormattingResponse(int id) {
  return DocumentFormattingResponse{"2.0", id, std::nullopt};
}
