#include "config.hpp"

#include <arepa/serializer/jsonSerializer.h>

#include <fstream>

using arepa::Error;
using arepa::Ok;
using arepa::Result;

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Utilities -
// ---------------------------------------------------------------------------------------------------------------------

Result<serverConfig::Configuration, std::string> load_config_from_json(const std::string& filepath) {
    serverConfig::Configuration config;
    std::ifstream s(filepath);

    // If the file failed to open, return an Error object.
    if (s.fail()) {
        return Error(std::string(strerror(errno)));
    }

    // Parse the JSON.
    nlohmann::json jsonServerConfig;
    try {
        jsonServerConfig = nlohmann::json::parse(s);
    } catch (nlohmann::json::parse_error& e) {
        return Error("Invalid JSON: " + std::string(e.what()));
    }

    // Parse the config.
    config = jsonSerializer::parseServerConfig(jsonServerConfig);
    config.err = false;
    return Ok(config);
}
