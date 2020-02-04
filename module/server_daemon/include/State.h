#include <boost/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>
#include <boost/variant/variant.hpp>
#include <unordered_map>
// #include <nlohmnann_json>

// todo: right now state representation is simple json object
// -> replace with better DS
// using json = nlohmann::json;

using Value = boost::make_recursive_variant<
    int, 
    bool, 
    std::unordered_map<std::string, boost::recursive_variant_>, 
    std::vector<boost::recursive_variant_>
  >::type;


struct Environment {
public:
  void
  set(std::string key, Value value) {
    assignments[key] = value;
  }

  std::optional<Value>
  get(const std::string& key) const {
    auto found = assignments.find(key);
    if (found != assignments.end()) {
      return {found->second};
    } else {
      return {};
    }
  }

private:
  std::unordered_map<std::string, Value> assignments;
};


struct Environments {
  Environment constants;
  Environment variables;
  Environment perPlayer;
  Environment perAudience;
};

class State {
public:
  // json sampleState = R"(
  //   {
  //     "served games": [
  //       {
  //         "configuration": {
  //           "name": "Zen Game",
  //           "player count": {"min": 0, "max": 0},
  //           "audience": false,
  //           "setup": {
  //             "required": [
  //               {
  //                 "kind": "string",
  //                 "prompt": "This is a prompt displayed during game creation"
  //               } 
  //             ],
  //             "optional": {
  //               "display message": "this is a configurable message"
  //             }
              
  //           }
  //         },
  //         "constants": {},
  //         "variables": {},
  //         "per-player": {},
  //         "per-audience": {},
  //         "rules": {}
  //       }
  //     ],
  //     "created games": [
  //       {
  //         "configuration": {
  //           "name": "Zen Game",
  //           "player count": 3,
  //           "audience": false,
  //           "setup": { 
  //             "display message" : "this configurable message has been altered"
  //           }
  //         },
  //         "invite code": "abc123",
  //         "variables": {},
  //         "players": [
  //           {
  //             "name": "player selects this name upon joining game. It must be unique"
  //           }
  //         ],
  //         "audience": []
          
  //       }
  //     ],
  //     "users": [
  //       {
  //         "user name": "fake user name",
  //         "don't know": "what else goes in here?"
  //       }
  //     ]
  //   }
  // )"_json;
};