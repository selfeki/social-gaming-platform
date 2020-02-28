#include <iostream>  
#include <string>    
#include <sstream>
#include <iterator>
#include <optional>
#include "RuleBook.h"
#include "../../game_spec/include/GameState.h"
#include <boost/variant.hpp>


// using Rule = boost::variant<GlobalMessage, MessageRule, Scores, InputText, Add, InputVote>;
using Rule = boost::variant<GlobalMessage, MessageRule, Scores>;
class Interpreter : public boost::static_visitor<GameState> {
  public:
  	Interpreter(GameState state):
      state{state}
  	  { }
    GameState operator()(GlobalMessage const& gM) const;
    GameState operator()(MessageRule const& m) const ;
    GameState operator()(Scores const& sc) const ;
    // GameState operator()(InputText const& iT) const ;
    GameState operator()(Add const& add) const;
    // GameState takeInput(int timeout, PlayersListPacket player, std::string attribute);
    // GameState operator()(InputVote const& InputVote) const;
    DataPacket getVariablesInvolved(const DataPacket& dp) const; 
    DataPacket getCleanMessage(const DataPacket& dp) const;
  private:
    // PlayersListPacket players ;  
    GameState state ;
};

struct MiddelWare{
  MapWrapper<std::string, Expression> mapp;
  std::vector<Expression> listt;
  std::string str;
  int integerr;
  bool booll ;
};
struct Visit_Expression : public boost::static_visitor<MiddelWare> {
    MiddelWare operator()(MapWrapper<std::string, Expression> const& mapp)const{MiddelWare md; md.mapp=mapp; return md;};
    MiddelWare operator()(std::vector<Expression> const& listt)const {
      MiddelWare md; 
      md.listt=listt; 
      return md;
    };
    MiddelWare operator()(std::string const& str)const{
      MiddelWare md; 
      md.str=str; 
      return md;
    };
    MiddelWare operator()(int const& integerr)const {MiddelWare md; md.integerr=integerr; return md;};
    MiddelWare operator()(bool const& booll)const {MiddelWare md; md.booll=booll; return md;};
};







