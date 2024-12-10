#pragma once

#include <map>
#include <string>

class LSPState {
private:
    std::map<std::string, std::string> documents;
public:
    LSPState() : documents() {}


    void openDocument(std::string uri, std::string text);
    void updateDocument(std::string uri, std::string text);
    std::string getDocument(std::string uri);
};

