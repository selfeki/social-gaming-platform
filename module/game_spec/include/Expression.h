#include "MapWrapper.h"

// An Expression is a R-value in game specification

using ExpMap  = MapWrapper<std::string, boost::recursive_variant_>;
using ExpList = std::vector<boost::recursive_variant_>;

using Expression = boost::make_recursive_variant<
  ExpMap,
  ExpList,
  std::string,
  int,
  bool>::type;