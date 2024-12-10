#include "messages.h"
#include "PublishDiagnosticsNotification.h"
#include "TextDocument.h"


InitializeResponse newInitializeResponse(int id){ 
    return InitializeResponse {
        "2.0",
        id,
        InitializeResult {
            ServerCapabilities {
                1, // Get full document
                true,
                false,
                false
            },
            ServerInfo {
                "tiplsp",
                "0.0.1"
            }
        }
    };
};

HoverResponse newHoverResponse(int id, std::string contents){ 
    return HoverResponse {
        "2.0",
        id,
        HoverResult {
            contents
        }
    };
};

PublishDiagnosticsNotification newPublishDiagnosticsNotificationError(VersionedTextDocumentIdentfier textDocument, std::string error) {
    size_t atPos = error.find('@');
    std::string numbersPart = error.substr(atPos + 1);
    size_t commaPos = numbersPart.find(',');
    int line = std::stoi(numbersPart.substr(0, commaPos)) - 1;
    int column = std::stoi(numbersPart.substr(commaPos + 1));
    std::string errorMsg = error.substr(0, atPos);

    return PublishDiagnosticsNotification {
        "2.0",
        "textDocument/publishDiagnostics",
        PublishDiagnosticsParams {
            textDocument.uri,
            textDocument.version,
            {
                Diagnostic{
                    Range {
                        Position{ line, column },
                        Position{ line, column }
                    }, 
                    1,
                    errorMsg
                }
            }
        }


    };
}

PublishDiagnosticsNotification newPublishDiagnosticsNotificationEmpty(VersionedTextDocumentIdentfier textDocument) {

    return PublishDiagnosticsNotification {
        "2.0",
        "textDocument/publishDiagnostics",
        PublishDiagnosticsParams {
            textDocument.uri,
            textDocument.version,
            {}
        }


    };
}
