#include <string>
#include <nlohmann/json.hpp>

std::string EncodeMessage(const nlohmann::json& msg);
std::pair<std::string, std::vector<uint8_t>> DecodeMessage(const std::vector<uint8_t>& msg);

