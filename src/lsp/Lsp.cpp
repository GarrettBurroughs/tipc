#include "Lsp.h"
#include "Rpc.h"
#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include "loguru.hpp"

class StreamParser {
public:
    explicit StreamParser(std::istream& input) : inputStream(input) {}

    // Parses the next complete message from the stream
    std::string getNextToken() {
        while (true) {
            // Read data into buffer if needed
            if (buffer.empty() || buffer.size() < expectedSize) {
                loadMoreData();
            }

            // Try parsing the next token
            auto token = parseBuffer();
            if (!token.empty()) {
                return token;
            }

            // If we can't parse, load more data
            if (inputStream.eof()) {
                throw std::runtime_error("Incomplete message at end of input");
            }
        }
    }

private:
    std::istream& inputStream;
    std::string buffer;
    size_t expectedSize = 0;

    // Load more data into the buffer
    void loadMoreData() {
        std::string chunk;
        char data[1024]; // Read in chunks of 1 KB
        inputStream.read(data, sizeof(data));
        buffer.append(data, inputStream.gcount());
    }

    // Parse the buffer to extract a complete token
    std::string parseBuffer() {
        static const std::string separator = "\r\n\r\n";

        // Find the header-content separator
        size_t separatorPos = buffer.find(separator);
        if (separatorPos == std::string::npos) {
            return ""; // Not enough data to determine a token
        }

        // Extract the header and content
        std::string header = buffer.substr(0, separatorPos);
        std::string content = buffer.substr(separatorPos + separator.size());

        // Parse the Content-Length
        static const std::string contentLengthPrefix = "Content-Length: ";
        size_t contentLengthPos = header.find(contentLengthPrefix);
        if (contentLengthPos == std::string::npos) {
            throw std::runtime_error("Missing Content-Length header");
        }
        contentLengthPos += contentLengthPrefix.size();

        size_t endOfNumber = header.find_first_of("\r\n", contentLengthPos);
        if (endOfNumber == std::string::npos) {
            throw std::runtime_error("Invalid Content-Length header");
        }

        std::string contentLengthStr = header.substr(contentLengthPos, endOfNumber - contentLengthPos);
        size_t contentLength = std::stoi(contentLengthStr);

        // Check if we have enough content
        expectedSize = separatorPos + separator.size() + contentLength;
        if (buffer.size() < expectedSize) {
            return ""; // Not enough data yet
        }

        // Extract the full message
        std::string token = buffer.substr(0, expectedSize);
        buffer = buffer.substr(expectedSize); // Remove the processed part
        expectedSize = 0; // Reset expected size
        return token;
    }
};


void handleMessage(void* state, std::string method, std::vector<uint8_t> contents) {
    LOG_S(1) << "Recieved method: " << method << std::endl;
}

void startLsp(std::istream& in, std::ostream& out) { 
    std::cout << "Starting tip lsp" << std::endl; 
    std::string input; 


    StreamParser parser(in);

    while (true) {
        try {
            std::string line = parser.getNextToken();
            std::vector<uint8_t> msg(line.begin(), line.end());

            out << "Token: " << line << std::endl;
            auto [method, contents] = DecodeMessage(msg);

            handleMessage(NULL, method, contents);
        } catch (const std::exception& e) {
            LOG_S(ERROR) << "LSP Error " << e.what() << std::endl;
        }
    }
}

