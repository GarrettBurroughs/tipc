#include <nlohmann/json.hpp>
#include <string>

std::string EncodeMessage(const nlohmann::json &msg);
std::pair<std::string, std::vector<uint8_t>>
DecodeMessage(const std::vector<uint8_t> &msg);
