#include "DSLInterpreter.h"

#include <numeric>
#include <string>

namespace gameSpecification::rule {


void 
InterpretVisitor::visitImpl(const ForEach& forEach) {
  // auto temp = boost::hash<ForEach>()(forEach);
  // Rule r = forEach;
  // context.push(&forEach);
  // context.push(forEach);
  // return state;
}

// GameState 
// Interpreter::operator()(GlobalMessage const& gM) const{
//     GameState newState(state);
//     DataPacket variable = getVariablesInvolved(gM.message);
//     DataPacket cleanMessage = getCleanMessage(gM.message);
//     MiddelWare mop = boost::apply_visitor(Visit_Expression(), newState.variables.get(variable.getData()));
//     if(mop.integerr){
//         newState.variables.set("GlobalMessage", Expression{cleanMessage.getData() + " " + std::to_string(mop.integerr)});
//     }
//     if(mop.str.length()>0){
//         newState.variables.set("GlobalMessage", Expression{cleanMessage.getData() + " " + mop.str});
//     }
//     return newState;
// }

// GameState Interpreter:: operator()(MessageRule const& m) const{
//     GameState newState(state);
//     DataPacket variable = getVariablesInvolved(m.message.getData());
//     DataPacket cleanMessage = getCleanMessage(m.message.getData());
//     MiddelWare mop = boost::apply_visitor(Visit_Expression(), newState.variables.get(variable.getData()));
//     for(Player player: m.recipents.getPlayers()){
//         std::string name= player.name;
//         std::vector<GamePlayer>::iterator it = std::find_if(newState.players.begin(), newState.players.end(), [&name](GamePlayer const& gPlayer){
//             return gPlayer.name==name;
//         });
//         if(it != newState.players.end()){
//             auto playerVarMap = boost::apply_visitor(Visit_Expression(), newState.perPlayer.get(it->name));
//             playerVarMap.mapp.set("message", Expression{cleanMessage.getData()+ " " + mop.str});
//             newState.perPlayer.set(it->name, playerVarMap.mapp);
//         }
//         std::vector<GameAudience>::iterator itA = std::find_if(newState.audience.begin(), newState.audience.end(), [&name](GameAudience const& gAudience){
//             return gAudience.name == name;
//         });
//         if(itA != newState.audience.end()){
//             auto audiVarMap = boost::apply_visitor(Visit_Expression(), newState.perAudience.get(itA->name));
//             audiVarMap.mapp.set("message", Expression{cleanMessage.getData()+ " " + mop.str});
//             newState.perAudience.set(itA->name, audiVarMap.mapp);
//         }
//     }
//     return newState;
// }

// bool varibaleCompare(const std::pair<std::string, int>& p1, const std::pair<std::string, int>& p2){
//     return p1.second < p2.second ;
// }
// GameState Interpreter:: operator()(Scores const& sc) const{ 
//     GameState newState(state);
//     std::vector<std::pair<std::string, int>> scoreBoard;
//     for(GamePlayer gplayer: state.players){
//         MiddelWare mop = boost::apply_visitor(Visit_Expression(), newState.perPlayer.get(gplayer.name));
//         mop= boost::apply_visitor(Visit_Expression(), mop.mapp.get(sc.variable.getData()));
//         scoreBoard.push_back(std::pair<std::string, int>(gplayer.name, mop.integerr));
//     }
//     std::sort(scoreBoard.begin(), scoreBoard.end(), varibaleCompare);
//     if(sc.sortOrder.getData().compare("false") == 0){
//         std::reverse(scoreBoard.begin(), scoreBoard.end());
//     }

//     MapWrapper<std::string, Expression> scoreMap;
//     for(std::pair<std::string, int>p : scoreBoard){
//         scoreMap.set(p.first, p.second);
//     }
//     newState.variables.set("ScoreBoard", scoreMap);
//     return newState;
//   }


// GameState Interpreter:: operator()(Add const& add) const{
//     GameState newState(state);
//     MiddelWare orignalData = boost::apply_visitor(Visit_Expression(), newState.variables.get(add.variable.getData()));
//     int temp = orignalData.integerr + add.value.getData() ;
//     newState.variables.set(add.variable.getData(), temp);
//     return newState;
// }


// DataPacket Interpreter::getVariablesInvolved(const DataPacket& dp) const{
//     std::string variable = "";
//     std::size_t openingBracketIndex = dp.getData().find("{");
//     std::size_t closingBracketIndex = dp.getData().find("}");
//     variable = dp.getData().substr(openingBracketIndex + 1, closingBracketIndex - (openingBracketIndex + 1));
//     return DataPacket(variable);
// }
// DataPacket Interpreter::getCleanMessage(const DataPacket& dp) const{
//     std::size_t openingBracketIndex = dp.getData().find("{");
//     return DataPacket(dp.getData().substr(0, openingBracketIndex - 1));
// }

} // namespace gameSpecification::rule