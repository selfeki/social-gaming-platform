#include <iostream>  
#include <string>    
#include <sstream>
#include <iterator>
#include <optional>

class Player {
  public:
  std::string name; 
  int wins = 0 ;
  DataPacket command ;
  Player(std::string name):
    name{name}
    { }
  Player(const Player& pl):
   name{pl.name}
   {}
  void setAttribute(DataPacket dp, std::string data){
  	if(dp.getData() == "wins"){
  		wins= stoi(data) ;
  	}
  	if(dp.getData() == "command"){
  		command.data = data ;
  	}
  }
};

class State{
  public:
  DataPacket DP ;
  PlayersListPacket<Player> playersInvolved ;
  std::string attribute ; 
  State(DataPacket Dp, PlayersListPacket<Player> players):
    DP{Dp},
    playersInvolved{players}
    { }
  State(DataPacket Dp, PlayersListPacket<Player> players, std::string attribute):
    DP{Dp},
    playersInvolved{players},
    attribute{attribute}
    { }
};

using Rule = boost::variant<GlobalMessage, Message<Player>, Scores, InputText<Player>>;
class Interpreter : public boost::static_visitor<State> {
  public:
	  Interpreter(std::vector<Player> players):
		  players{players}
		  { }
    State operator()(GlobalMessage const& gM) const{}
    State operator()(Message<Player> const& m) const{}
    State operator()(Scores const& sc) const{} 
    State operator()( InputText<Player> const& iT) const{}
    State takeInput(int timeout, PlayersListPacket<Player> player, std::string attribute){}

  private:
    std::vector<Player> players ;  
    std::stringstream outStream ;
};

