#include "MapWrapper.h"

// An Expression is a R-value in game specification

using Map  = MapWrapper<std::string, boost::recursive_variant_>;
using List = std::vector<boost::recursive_variant_>;

using Expression = boost::make_recursive_variant<
  Map,
  List,
  std::string,
  int,
  bool>::type;