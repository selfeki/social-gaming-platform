#include <boost/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>
#include <boost/variant/variant.hpp>
#include <unordered_map>

// todo: implement State struct

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
// todo
};