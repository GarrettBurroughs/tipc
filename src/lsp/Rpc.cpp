#include "Rpc.h"

#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream> 
#include <stdexcept> 


using json = nlohmann::json;

std::string EncodeMessage(const json& msg) {
    try {
        std::string content = msg.dump();

        std::ostringstream oss;
        oss << "Content-Length: " << content.size() << "\r\n\r\n" << content;

        return oss.str();
    } catch (const std::exception& e) {
        // TODO: Check error consistency througohut project
        throw std::runtime_error(std::string("Failed to encode message: ") + e.what());
    }
}


struct BaseMessage {
    std::string Method;

    // Deserialize JSON into BaseMessage
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(BaseMessage, Method)
};

std::pair<std::string, std::vector<uint8_t>> DecodeMessage(const std::vector<uint8_t>& msg) {
    const std::string separator = "\r\n\r\n";
    auto separatorPos = std::search(msg.begin(), msg.end(), separator.begin(), separator.end());

    if (separatorPos == msg.end()) {
        throw std::invalid_argument("Did not find separator");
    }

    std::string header(msg.begin(), separatorPos);
    std::vector<uint8_t> content(separatorPos + separator.size(), msg.end());

    const std::string contentLengthPrefix = "Content-Length: ";
    auto contentLengthPos = header.find(contentLengthPrefix);
    if (contentLengthPos == std::string::npos) {
        throw std::invalid_argument("Missing Content-Length header");
    }

    int contentLength = 0;
    try {
        contentLength = std::stoi(header.substr(contentLengthPos + contentLengthPrefix.size()));
    } catch (const std::exception&) {
        throw std::invalid_argument("Invalid Content-Length value");
    }

    if (content.size() < static_cast<size_t>(contentLength)) {
        throw std::out_of_range("Content length exceeds available data");
    }

    nlohmann::json jsonContent;
    try {
        jsonContent = nlohmann::json::parse(content.begin(), content.begin() + contentLength);
    } catch (const std::exception& e) {
        throw std::invalid_argument(std::string("Failed to parse JSON: ") + e.what());
    }

    BaseMessage baseMessage = jsonContent.get<BaseMessage>();
    return {baseMessage.Method, std::vector<uint8_t>(content.begin(), content.begin() + contentLength)};
}
