#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>
#include "jsonconfig.h"

using json = nlohmann::json;
//Game config
void game_config::from_json(const json& j, game_config::configuration& config) {
        j["configuration"]["name"].get_to(config.name);
        j["configuration"]["player count"].get_to(config.player_count);
        j["configuration"]["audience"].get_to(config.audience);
        j["configuration"]["setup"].get_to(config.setup);
        j["constants"].get_to(config.constants);
        j["variables"].get_to(config.variables);
        j["per-player"].get_to(config.per_player);
        j["per-audience"].get_to(config.per_audience);

        for (const auto& element: j["rules"]) {
            config.rules.push_back(element);
        }

        config.err = false;

    }

std::vector<json> game_config::find_rule(const std::vector<json>& j, const std::string& rule_name) {
    std::vector<json> ret;
    for (auto& element: j) {
        auto found = element.find("rule");
        if (found != element.end() && element.at("rule") == rule_name) {
            ret.push_back(element);
        }
    }
    return ret;
}

void game_config::print_config(const game_config::configuration& config) { 
    std::cout << "configuration:\n"
    << "name: " << config.name
    << "\nplayer count: " << config.player_count 
    << "\naudience: " << config.audience
    << "\nsetup: " << config.setup 
    << "\nconstants: " << config.constants
    << "\nvariables: " << config.variables
    << "\nper_player: " << config.per_player
    << "\nper_audience: " << config.per_audience
    << "\nrules: ";

    for (const auto& element: config.rules) {
        std::cout << element << "\n\n";
    }

    std::cout << "\n";
}

game_config::configuration game_config::load_file(const std::string& filepath, bool debug) {
    json temp;
    game_config::configuration ret;
    std::ifstream s(filepath);
    if (s.fail()) {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        return ret;
    }
    
    try {
        temp = json::parse(s);
        ret = temp.get<game_config::configuration>();
    }
    catch (json::parse_error& e) {
        std::cerr << "Failed to parse JSON.\n in jsonconfig.cpp\n"
        << "message: " << e.what();
        return ret;
    }

    
    ret.jsonpath = filepath;
    ret.err = false;
    if (debug) {
        game_config::print_config(ret);
    }

    return ret;
}



//Server config
void server_config::from_json(const json& j, server_config::configuration& config) {
    j["port"].get_to(config.port);
    j["html"].get_to(config.htmlpath);
    config.err = false;
}

void server_config::print_config(const server_config::configuration& config) {
    std::cout << "configuration: {\n"
    << "html: " << config.htmlpath
    << "\nport: " << config.port
    << "\n}\n";
}

server_config::configuration server_config::load_file(const std::string& filepath, bool debug) {
    json temp;
    server_config::configuration ret;
    std::ifstream s(filepath);
    if (s.fail()) {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        return ret;
    }
    
    try {
        temp = json::parse(s);
        ret = temp.get<server_config::configuration>();
    }
    catch (json::parse_error& e) {
        std::cerr << "Failed to parse JSON.\n in jsonconfig.cpp\n"
        << "message: " << e.what();
        return ret;
    }

    
    ret.jsonpath = filepath;
    ret.err = false;
    if (debug) {
        server_config::print_config(ret);
    }

    return ret;
}

