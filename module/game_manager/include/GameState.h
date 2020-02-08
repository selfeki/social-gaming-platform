#include <boost/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>
#include <boost/variant/variant.hpp>
#include <unordered_map>
#include <iostream>

// todo: implement State struct

template <class keyTy, class valTy>
struct MapWrapper {
public:
  void
  set(keyTy key, valTy value) {
    assignments[key] = value;
  }

  valTy
  get(const keyTy& key) const {
    auto found = assignments.find(key);
    if (found != assignments.end()) {
      return {found->second};
    } else {
      std::cerr << "Invalid key: " << key << '\n';
      throw;
    }
  }

private:
  std::unordered_map<keyTy, valTy> assignments;
};


////// In-Game State

using MapValue  = std::unordered_map<std::string, boost::recursive_variant_>;
using ValueList = std::vector<boost::recursive_variant_>;

using EnvValue = boost::make_recursive_variant<
    MapValue,
    ValueList, 
    std::string,
    int, 
    bool
  >::type;



////// Game Settings

enum DataKind {
  INTEGER,
  STRING,
  BOOLEAN,
  QUESTION_ANSWER,
  MULTIPLE_CHOICE
};

struct MapSetupValue {
  DataKind kind;
  std::string prompt;
};

using SetupValue = boost::make_recursive_variant<
    MapSetupValue,
    std::string,
    int, 
    bool
  >::type;


////// Player and Audience States

struct Player {
  std::string name;
};


struct Audience {
  std::string name;
};


///////////////////////////////////


using Environment = MapWrapper<std::string, EnvValue>;
using Setup       = MapWrapper<std::string, SetupValue>;


struct GameState {
  Environment constants;
  Environment variables;
  Environment perPlayer;
  Environment perAudience;
  Setup       setup;

  // player exclusive data contained here or in userStates?
  // A user might have game-agnostic data
  std::vector<Player> players;
  std::vector<Audience> audience;
};


struct UserState; // todo: Implement

class RoomState {
  GameState gameState;
  std::vector<UserState> userStates;
};