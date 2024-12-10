#include "LSPState.h"

void LSPState::openDocument(std::string uri, std::string text) {
    documents[uri] = text;
}


void LSPState::updateDocument(std::string uri, std::string text) {
    documents[uri] = text;
}

std::string LSPState::getDocument(std::string uri) {
    return documents[uri];
}

