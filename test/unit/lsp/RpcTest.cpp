#include <catch2/catch_test_macros.hpp>
#include <string>
#include <nlohmann/json.hpp>

#include "Rpc.h"

using json = nlohmann::json;

TEST_CASE("Lsp: EncodeMessage", "[Lsp]") {
    json input {
        {"Testing", true}
    };
    std::string expected = "Content-Length: 16\r\n\r\n{\"Testing\":true}";
    std::string actual = EncodeMessage(input);
    REQUIRE(expected == actual);
}

TEST_CASE("Lsp: DecodeMessage", "[Lsp]") {
    std::string input = "Content-Length: 15\r\n\r\n{\"Method\":\"hi\"}";
    std::vector<uint8_t> vec(input.begin(), input.end());
    auto output = DecodeMessage(vec);
    std::string method = output.first;
    std::vector<uint8_t> content = output.second;
    REQUIRE(content.size() == 15);
    REQUIRE(method == "hi");
}
