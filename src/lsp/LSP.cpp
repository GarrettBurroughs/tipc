#include "LSP.h"
#include "RPC.h"
#include "messages/InitializeRequest.h"
#include <exception>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

#include "loguru.hpp"

void handleMessage(void* state, std::string method, std::vector<uint8_t> contents) {
    LOG_S(INFO) << "Recieved method: " << method << std::endl;

    nlohmann::json jsonContent;
    try {
        jsonContent = nlohmann::json::parse(contents.begin(), contents.end());
    } catch (const std::exception& e) {
        throw std::invalid_argument(std::string("Failed to parse JSON: ") + e.what());
    }

    if (method == "initialize") {
        InitializeRequest request = jsonContent.get<InitializeRequest>();
        LOG_S(INFO) << "Connected to: " << request.params.clientInfo->name << " " << request.params.clientInfo->version << std::endl;
    }
}

class StreamParser {
public:
    explicit StreamParser(std::istream& input) : inputStream(input) {}

    std::string getNextToken() {
        while (true) {
            read();

            auto token = parseBuffer();
            if (!token.empty()) {
                return token;
            }

            if (inputStream.eof()) {
                throw std::runtime_error("Incomplete message at end of input");
            }
        }
    }

private:
    std::istream& inputStream;
    std::string buffer;
    size_t expectedSize = 0;

    // TODO: Figure out if we can read more data at a time? 
    void read() {
        char data[1];
        inputStream.read(data, sizeof(data));
        buffer.append(data, inputStream.gcount());
    }

    std::string parseBuffer() {
        static const std::string separator = "\r\n\r\n";

        size_t separatorPos = buffer.find(separator);
        if (separatorPos == std::string::npos) {
            return ""; // Not enough data to determine a token
        }

        std::string header = buffer.substr(0, separatorPos);
        std::string content = buffer.substr(separatorPos + separator.size());

        static const std::string contentLengthPrefix = "Content-Length: ";
        size_t contentLengthPos = header.find(contentLengthPrefix);
        if (contentLengthPos == std::string::npos) {
            throw std::runtime_error("Missing Content-Length header");
        }
        contentLengthPos += contentLengthPrefix.size();

        std::string contentLengthStr = header.substr(contentLengthPos);
        size_t contentLength = std::stoi(contentLengthStr);

        expectedSize = separatorPos + separator.size() + contentLength;
        if (buffer.size() < expectedSize) {
            return ""; 
        }

        std::string token = buffer.substr(0, expectedSize);
        buffer = buffer.substr(expectedSize); 
        expectedSize = 0; 
        return token;
    }
};

void startLsp(std::istream& in, std::ostream& out) { 
    LOG_S(INFO) << "Starting tip lsp" << std::endl; 
    std::string input; 


    StreamParser parser(in);

    while (true) {
        try {
            LOG_S(INFO) << "attempting to get token" << std::endl;
            std::string line = parser.getNextToken();
            std::vector<uint8_t> msg(line.begin(), line.end());

            LOG_S(INFO) << "Token: " << line << std::endl;
            auto [method, contents] = DecodeMessage(msg);

            handleMessage(NULL, method, contents);
        } catch (const std::exception& e) {
            LOG_S(ERROR) << "LSP Error " << e.what() << std::endl;
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    loguru::g_stderr_verbosity = 1;
    loguru::g_preamble = true;
    loguru::g_preamble_date = false;
    loguru::g_preamble_time = false;
    loguru::g_preamble_uptime = false;
    loguru::g_preamble_thread = false;

    // TODO: Remove this log file when finished (or add option for enabling loggin)
    loguru::add_file("/home/gburroughs/dev/tipc_test/log.txt", loguru::Truncate, loguru::Verbosity_INFO);
    startLsp(std::cin, std::cout);
    return 0;
}

