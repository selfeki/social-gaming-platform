#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include "jsonconfig.h"

using json = nlohmann::json;
//Game config
void game_config::from_json(const json& j, game_config::configuration& config) {

    j["configuration"]["name"].get_to(config.name);
    //j["configuration"]["player count"].get_to(config.player_count);
    j["configuration"]["audience"].get_to(config.audience);
    //j["configuration"]["setup"].get_to(config.setup);
    //j["constants"].get_to(config.constants);
    j["variables"].get_to(config.variables);
    j["per-audience"].get_to(config.per_audience);
    for (const auto& element: j["rules"]) {
        config.rules.push_back(element);
    }

    std::unordered_map<std::string, json> player_count_json;
    j["configuration"]["player count"].get_to(player_count_json);
    config.player_count_s = config_player_count{player_count_json.find("min")->second, player_count_json.find("max")->second};

    std::unordered_map<std::string, json> per_player_json;
    j["per-player"].get_to(per_player_json);
    config.per_player_s = config_per_player{per_player_json.find("wins")->second};

    std::unordered_map<std::string, json> setup_json;
    j["configuration"]["setup"].get_to(setup_json);
    config.setup_s = config_setup{setup_json.find("Rounds")->second};

    std::unordered_map<std::string, json> constants_json;
    j["constants"].get_to(constants_json);

    for(const auto& elem: j["constants"]["weapons"]){
        constants_weapon weapon = constants_weapon{elem["name"], elem["beats"]};
        config.constants_s.weapons.push_back(weapon);
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

json game_config::first_rule(const json& j, const int i) {
    if (j.find("rules") == j.end()) {
        return j;
    } else {
        return j["rules"][i];
    }
}

bool game_config::has_rules(const json& j) {
    return j.find("rules") != j.end();
}

void game_config::print_config(const game_config::configuration& config) {

    std::cout << "configuration:\n"
    << "name: " << config.name
    //<< "\nplayer count: " << config.player_count
    << "\nPlayer count: Min: "<< config.player_count_s.min <<"Max: "<< config.player_count_s.max
    << "\naudience: " << config.audience
    //<< "\nsetup: " << config.setup
    << "\nsetup: " << config.setup_s.rounds
    //<< "\nconstants: " << config.constants
    //<< "\nvariables: " << config.variables
    //<< "\nper_player: " << config.per_player
    << "\nwins per player : "<< config.per_player_s.wins
    << "\nper_audience: " << config.per_audience
    << "\nweapons: \n";
    for (const auto& elem: config.constants_s.weapons){
        std::cout << "\tname: " << elem.name << " beats: " << elem.beats << "\n";
    }
    std::cout << "\nrules: ";
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

bool game_config::valid(const json& j) {
    json temp = j.flatten();
    for (auto element: game_config::enum_to_str) {
        if (temp[element.second] == NULL) {
            return false;
        }
    }
    return true;
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

bool server_config::valid(const json& j) {
    json temp = j.flatten();
    for (auto element: server_config::enum_to_str) {
        if (temp[element.second] == NULL) {
            return false;
        }
    }
    return true;
}
