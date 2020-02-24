#pragma once

#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>
#include <list>
#include "arepa/game_spec/GameSpecification.h"

using json = nlohmann::json;

//MASTER PLAN: Change all formats into json and throw it into here

namespace json_serializer {





// struct spec_player_count{
// };
// struct spec_setup{
// };
// struct per_player{
//     int wins;
// };
// struct constants_weapon{
//     std::string name;
//     std::string beats;
// };
// struct spec_constants{
//     //TODO: discuss if vector of structs is a reasonable way  to store list of weapons
//     std::vector<constants_weapon> weapons;
// };

// struct specification {
//     std::string name;
//     //std::unordered_map<std::string, json> player_count;
//     bool audience;
//     //std::unordered_map<std::string, json> setup;
//     //std::unordered_map<std::string, json> constants;
//     std::unordered_map<std::string, json> variables;
//     //std::unordered_map<std::string, json> per_player;
//     std::unordered_map<std::string, json> per_audience;
//     std::vector<json> rules;
//     std::string jsonpath;
//     bool err = true;

//     spec_player_count player_count_s;
//     spec_per_player per_player_s;
//     spec_setup setup_s;
//     spec_constants constants_s;
//     //default true, set only to false when all fields are assigned to values
// };

// void from_json(const json&, specification&);

gameSpecification::Specification
parseGameSpecification(const json&);






    // //Returns an empty vector on failure to find rule.
    // //Returns a vector of all json objects that matches rule name
    // std::vector<json> find_rule(const std::vector<json>&, const std::string&);

    // //Returns itself if json object has no rules
    // //Returns the first rule in rules. Assuming rules is a vector/array
    // //Optional parameter for getting rule in desired index
    // json first_rule(const json&, const int=0);

    // //Returns true if json object has rules
    // //Returns false if jason object does not have rules
    // //Use in conjuction with first_rule.
    // bool has_rules(const json&);

// move the unrelated routine to specification class
    // void print_spec(const specification&); // exists as an example for accessing data

// move to 
    // specification load_file(const std::string&, bool=false);



} // json serializer


// todo: move server config to separate module
namespace server_config{

    enum FIELDS {
        PORT,
        HTML
    };

    const std::unordered_map<FIELDS, std::string> enum_to_str({
        {PORT,"/port"},
        {HTML, "/htmlpath"}
    });

    struct configuration {
        int port;
        std::string htmlpath;
        std::string jsonpath;
        bool err = true; //default true, set only to false when fields are assigned to values
    };


    void from_json(const json&, configuration&);

    void print_config(const configuration&);

    configuration load_file(const std::string&, bool=false);

    bool valid(const json&);
}
