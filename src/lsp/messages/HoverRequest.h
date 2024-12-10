#pragma once 

#include "TextDocument.h"
#include <optional>

struct HoverParams {
    TextDocumentIdentfier textDocument;
    Position position;
    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(HoverParams, textDocument, position)

};

struct HoverResult {
    std::string contents;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(HoverResult, contents)
};

struct HoverRequest {
    int id;
    std::string jsonrpc;

    HoverParams params;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(HoverRequest, id, jsonrpc, params)
};

struct HoverResponse {
    std::string jsonrpc;
    std::optional<int> id;
    HoverResult result;

    friend void to_json(nlohmann::json& j, const HoverResponse& r) {
        j["jsonrpc"] = r.jsonrpc;
        j["result"] = r.result;
        if (r.id) {
            j["id"] = r.id.value();
        }

    }

    friend void from_json(const nlohmann::json& j, HoverResponse & r) {
        if (j.contains("id")) {
            r.id = j.at("id").get<int>();
        } else {
            r.id = std::nullopt;
        }
        r.jsonrpc = j.at("jsonrpc").get<std::string>();
        r.result = j.at("result").get<HoverResult>();
    }
};

