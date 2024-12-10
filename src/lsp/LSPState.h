#pragma once

#include "messages/HoverRequest.h"
#include <map>
#include <string>

class LSPState {
private:
    std::map<std::string, std::string> documents;
    std::vector<std::string> splitLines(const std::string& content);
    std::pair<int, std::string> getWordAtCursor(const std::string& content, int line, int character);
public:
    LSPState() : documents() {}


    void openDocument(std::string uri, std::string text);
    void updateDocument(std::string uri, std::string text);
    std::string getDocument(std::string uri);
    HoverResponse hover(HoverRequest request);
};

