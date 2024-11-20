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
