#pragma once

#include "InitializeRequest.h"
#include "DidOpenTextDocumentNotification.h"
#include "HoverRequest.h"
#include "PublishDiagnosticsNotification.h"
#include "DidChangeTextDocumentNotification.h"
#include "TextDocument.h"

InitializeResponse newInitializeResponse(int id);
HoverResponse newHoverResponse(int id, std::string contents);
PublishDiagnosticsNotification newPublishDiagnosticsNotificationError(VersionedTextDocumentIdentfier textDocument, std::string error);
PublishDiagnosticsNotification newPublishDiagnosticsNotificationEmpty(VersionedTextDocumentIdentfier textDocument);
