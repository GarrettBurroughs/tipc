#pragma once

#include "InitializeRequest.h"
#include "DidOpenTextDocumentNotification.h"
#include "HoverRequest.h"
#include "DidChangeTextDocumentNotification.h"
#include "TextDocument.h"

InitializeResponse newInitializeResponse(int id);
HoverResponse newHoverResponse(int id, std::string contents);
