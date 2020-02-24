#pragma once

#include <vector>
#include <string>
#include <optional>
#include <boost/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>
#include <boost/variant/variant.hpp>

namespace gameSpecification {
	
#include "MapWrapper.h"
#include "Expression.h"
#include "Rules.h"
#include "GameState.h"

////// Game Setup related

enum DataKind {
	INTEGER,
	STRING,
	BOOLEAN,
	QUESTION_ANSWER,
	MULTIPLE_CHOICE
};

struct CustomSetup {
	DataKind kind;
	std::string prompt;
};

using SetupValue = boost::variant<
	CustomSetup,
	std::string,
	int,
	bool>;

using Setup = MapWrapper<std::string, SetupValue>;

//////////


struct PlayerCount {
	int min;
	int max;
};

struct Configuration {
	std::string name;
	PlayerCount	count;
	bool	allowAudience;
	Setup	setup;
};

struct Specification {
	Configuration 	configuration;
	GameState		gameState;
	rules::RuleList	rules;
};


} // namespace gameSpecification