#pragma once

#include "InitializeRequest.h"
#include "DidOpenTextDocumentNotification.h"
#include "DidChangeTextDocumentNotification.h"
#include "TextDocument.h"

InitializeResponse newInitializeResponse(int id);
