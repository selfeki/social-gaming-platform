#include <iostream>
#include <string>
#include <sstream>
#include <iterator>
#include <optional>
#include "RuleBook.h"
#include <boost/variant.hpp>


using Rule = boost::variant<GlobalMessage, MessageRule, Scores, InputText>;
class Interpreter : public boost::static_visitor<State> {
public:
    Interpreter(std::vector<Player> players):
    players{players}
    { }
    State operator()(GlobalMessage const& gM) const;
    State operator()(MessageRule const& m) const ;
    State operator()(Scores const& sc) const ;
    State operator()( InputText const& iT) const ;
    State takeInput(int timeout, PlayersListPacket player, std::string attribute);
    DataPacket getVariablesInvolved(DataPacket dp) const;
    
private:
    PlayersListPacket players ;
    std::stringstream outStream ;
};

