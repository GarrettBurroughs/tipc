#pragma once

#include "DidChangeTextDocumentNotification.h"
#include "DidOpenTextDocumentNotification.h"
#include "DocumentFormattingRequest.h"
#include "HoverRequest.h"
#include "InitializeRequest.h"
#include "PublishDiagnosticsNotification.h"
#include "TextDocument.h"

InitializeResponse newInitializeResponse(int id);
HoverResponse newHoverResponse(int id, std::string contents);
PublishDiagnosticsNotification newPublishDiagnosticsNotificationError(
    VersionedTextDocumentIdentfier textDocument, std::string error);
PublishDiagnosticsNotification newPublishDiagnosticsNotificationEmpty(
    VersionedTextDocumentIdentfier textDocument);
DocumentFormattingResponse newDocumentFormattingResponse(int id,
                                                         std::string original,
                                                         std::string contents);
DocumentFormattingResponse newNullDocumentFormattingResponse(int id);
