#include <optional>
#include <string>
#include <nlohmann/json.hpp>


struct ClientInfo {
    std::string name;
    std::string version;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ClientInfo, name, version)
};

struct InitializeRequestParams {
    std::optional<ClientInfo> clientInfo;

    friend void to_json(nlohmann::json& j, const InitializeRequestParams& p) {
        if (p.clientInfo) {
            j["clientInfo"] = p.clientInfo.value();
        }
    }

    friend void from_json(const nlohmann::json& j, InitializeRequestParams& p) {
        if (j.contains("clientInfo")) {
            p.clientInfo = j.at("clientInfo").get<ClientInfo>();
        } else {
            p.clientInfo = std::nullopt;
        }
    }
};

struct InitializeRequest {
    int id;
    std::string jsonrpc;
    std::string method;

    InitializeRequestParams params;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(InitializeRequest, id, jsonrpc, method, params)
};

struct ServerInfo {
    std::string name;
    std::string version;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ServerInfo, name, version)
};

struct ServerCapabilities {
    int textDocumentSync;
    bool hoverProvider;
    bool definitionProvider;
    bool codeActionProvider;
    std::map<std::string, nlohmann::json> completionProvider;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ServerCapabilities, textDocumentSync, hoverProvider, definitionProvider, codeActionProvider, completionProvider)
};

struct InitializeResult {
    ServerCapabilities capabilities;
    ServerInfo serverInfo;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(InitializeResult, capabilities, serverInfo)
};

// TODO: Make id optional
struct InitializeResponse {

    std::string jsonrpc;
    std::optional<int> id;
    InitializeResult result;

    friend void to_json(nlohmann::json& j, const InitializeResponse& r) {
        j["jsonrpc"] = r.jsonrpc;
        j["result"] = r.result;
        if (r.id) {
            j["id"] = r.id.value();
        }

    }

    friend void from_json(const nlohmann::json& j, InitializeResponse & r) {
        if (j.contains("id")) {
            r.id = j.at("id").get<int>();
        } else {
            r.id = std::nullopt;
        }
        r.jsonrpc = j.at("jsonrpc").get<std::string>();
        r.result = j.at("result").get<InitializeResult>();
    }
};


