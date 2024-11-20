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
