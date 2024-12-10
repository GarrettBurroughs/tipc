#pragma once 

#include <string>
#include <nlohmann/json.hpp>

struct TextDocumentItem {
    std::string uri; 
    std::string languageId; 
    int version; 
    std::string text;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(TextDocumentItem, uri, languageId, version, text)
};


struct VersionedTextDocumentIdentfier {
    std::string uri; 
    int version;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(VersionedTextDocumentIdentfier, uri, version)
};


