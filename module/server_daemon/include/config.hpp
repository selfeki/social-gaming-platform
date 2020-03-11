#pragma once

#include <arepa/Result.hpp>
#include <arepa/server_config/Config.h>

#include <nlohmann/json.hpp>
#include <string>

const std::string DEFAULT_CONFIG_FILE = "templates/server/default.json";    // NOLINT

/**
 * Loads server config from a JSON file.
 *
 * @param filepath The JSON file.
 * @return The loaded configuration.
 */
arepa::Result<serverConfig::Configuration, std::string> load_config_from_json(const std::string& filepath);
