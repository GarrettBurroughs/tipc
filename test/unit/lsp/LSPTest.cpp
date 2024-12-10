#include <catch2/catch_test_macros.hpp>
#include <cstdio>
#include <string>
#include <nlohmann/json.hpp>

#include "RPC.h"

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
    std::string input = "Content-Length: 15\r\n\r\n{\"method\":\"hi\"}";
    std::vector<uint8_t> vec(input.begin(), input.end());
    auto output = DecodeMessage(vec);
    std::string method = output.first;
    std::vector<uint8_t> content = output.second;
    REQUIRE(content.size() == 15);
    REQUIRE(method == "hi");
}

// TODO: Figure out a nice way to test this without needing to rely on stdout 
//
// TEST_CASE("Lsp: Parse Tokens", "[Lsp]") {
//     std::stringstream input;
//     input << "Content-Length: 18\r\n\r\n{\"Method\":\"test1\"}Content-Length: 18\r\n\r\n{\"Method\":\"test2\"}";
//     input << EOF;
//     std::ostringstream output; 
// #include "LSP.h"
//     startLsp(input, output);
//     std::ostringstream expected; 
//     expected << "Token: Content-Length: 18\r\n\r\n{\"Method\":\"test1\"}" << std::endl;
//     expected << "Token: Content-Length: 18\r\n\r\n{\"Method\":\"test2\"}" << std::endl;
//     REQUIRE(output.str() == expected.str());
// }
