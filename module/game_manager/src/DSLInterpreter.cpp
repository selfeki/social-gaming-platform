#include<utility>
#include <numeric>
#include <string>
#include <variant>
#include "DSLInterpreter.h"
#include <boost/scoped_ptr.hpp>


GameState Interpreter:: operator()(GlobalMessage const& gM) const{
    GameState newState(state);
    DataPacket variable = getVariablesInvolved(gM.message);
    DataPacket cleanMessage = getCleanMessage(gM.message);
    MiddelWare mop = boost::apply_visitor(Visit_Expression(), newState.variables.get(variable.getData()));
    if(mop.integerr){
        newState.variables.set("GlobalMessage", Expression{cleanMessage.getData() + " " + std::to_string(mop.integerr)});
    }
    if(mop.str.length()>0){
        newState.variables.set("GlobalMessage", Expression{cleanMessage.getData() + " " + mop.str});
    }
    return newState;
}

GameState Interpreter:: operator()(MessageRule const& m) const{
    GameState newState(state);
    DataPacket variable = getVariablesInvolved(m.message.getData());
    DataPacket cleanMessage = getCleanMessage(m.message.getData());
    MiddelWare mop = boost::apply_visitor(Visit_Expression(), newState.variables.get(variable.getData()));
    for(Player player: m.recipents.getPlayers()){
        std::string name= player.name;
        std::vector<GamePlayer>::iterator it = std::find_if(newState.players.begin(), newState.players.end(), [&name](GamePlayer const& gPlayer){
            return gPlayer.name==name;
        });
        if(it != newState.players.end()){
            auto playerVarMap = boost::apply_visitor(Visit_Expression(), newState.perPlayer.get(it->name));
            playerVarMap.mapp.set("message", Expression{cleanMessage.getData()+ " " + mop.str});
            newState.perPlayer.set(it->name, playerVarMap.mapp);
        }
        std::vector<GameAudience>::iterator itA = std::find_if(newState.audience.begin(), newState.audience.end(), [&name](GameAudience const& gAudience){
            return gAudience.name == name;
        });
        if(itA != newState.audience.end()){
            auto audiVarMap = boost::apply_visitor(Visit_Expression(), newState.perAudience.get(itA->name));
            audiVarMap.mapp.set("message", Expression{cleanMessage.getData()+ " " + mop.str});
            newState.perAudience.set(itA->name, audiVarMap.mapp);
        }
    }
    return newState;
}

bool varibaleCompare(const std::pair<std::string, int>& p1, const std::pair<std::string, size_t>& p2){
    return p1.second < p2.second ;
}
GameState Interpreter:: operator()(Scores const& sc) const{ 
    GameState newState(state);
    std::vector<std::pair<std::string, int>> scoreBoard;
    for(GamePlayer gplayer: state.players){
        MiddelWare mop = boost::apply_visitor(Visit_Expression(), newState.perPlayer.get(gplayer.name));
        mop= boost::apply_visitor(Visit_Expression(), mop.mapp.get(sc.variable.getData()));
        scoreBoard.push_back(std::pair<std::string, int>(gplayer.name, mop.integerr));
    }
    std::sort(scoreBoard.begin(), scoreBoard.end(), varibaleCompare);
    if(sc.sortOrder.getData().compare("false") == 0){
        std::reverse(scoreBoard.begin(), scoreBoard.end());
    }

    MapWrapper<std::string, Expression> scoreMap;
    for(std::pair<std::string, int>p : scoreBoard){
        scoreMap.set(p.first, p.second);
    }
    newState.variables.set("ScoreBoard", scoreMap);
    return newState;
  }


GameState Interpreter:: operator()(Add const& add) const{
    GameState newState(state);
    MiddelWare orignalData = boost::apply_visitor(Visit_Expression(), newState.variables.get(add.variable.getData()));
    int temp = orignalData.integerr + add.value.getData() ;
    newState.variables.set(add.variable.getData(), temp);
    return newState;
}

// State Interpreter:: operator()(InputVote const& inputVote) const{ // To change to consider Time Out
//     if(State(state).stateMap.getDataMap()[DataPacket("input_recieved")].getData().compare("true") != 0){
//         Rule promptMessage((MessageRule(inputVote.messageTosend, inputVote.players)));
//         State st = boost::apply_visitor(Interpreter(players.getPlayers(), state), promptMessage);
//         std::vector<std::string> vectorOfChoices;
//         for(DataPacket choice: inputVote.choices){
//             vectorOfChoices.push_back(choice.getData());
//         }
//         std::string choicesString = std::accumulate(vectorOfChoices.begin(), vectorOfChoices.end(), std::string(" "));
//         Rule promptChoices((MessageRule(DataPacket(choicesString), inputVote.players)));
//         st = boost::apply_visitor(Interpreter(players.getPlayers(), state), promptChoices);
//         return State(st);
//     }
//     MapOfArithmeticVariables choicesMap(std::pair<DataPacket, ArithmeticValue>(DataPacket("None"), ArithmeticValue(0)));
//     for(Player p1: players.getPlayers()){
//         DataPacket choice = State(state).stateMap.getDataMap().find(p1.name)->second;
//         if(choicesMap.getDataMap().find(choice) == choicesMap.getDataMap().end()){
//             choicesMap.insertNew(choice, ArithmeticValue(1));
//         }else{
//             choicesMap.getDataMap()[choice]= choicesMap.getDataMap()[choice].getData() + 1;
//         }
//     }
//     DataPacket bestChoice = std::max_element(choicesMap.getDataMap().begin(), choicesMap.getDataMap().end(),
//         [](const std::pair<DataPacket, ArithmeticValue> pair1, const std::pair<DataPacket, ArithmeticValue> pair2){
//                 return pair1.second.getData() < pair2.second.getData() ;
//         })->first;
//     State st(state);
//     st.stateMap.insertNew(inputVote.resultVariable, bestChoice) ;
//     st.stateMap.getDataMap()[DataPacket("input_recieved")] = DataPacket("false");
//     return st;
 
// }
// State Interpreter:: operator()( InputText const& iT) const{
// 	if(State(state).stateMap.getDataMap()[DataPacket("input_recieved")].getData().compare("true") != 0){
//         Rule promptMessage((MessageRule(iT.messageTosend, iT.player)));
//         State st = boost::apply_visitor(Interpreter(players.getPlayers(), state), promptMessage);
//         return st;
//     }
//     State st(state);
//     DataPacket dataRecieved = st.stateMap.getDataMap()[DataPacket("input_recieved")] ;
//     st.stateMap.insertNew(iT.resultVariable, dataRecieved);
//     st.stateMap.getDataMap()[DataPacket("input_recieved")] = DataPacket("false");
//     return st ;
// }

// State Interpreter:: takeInput(int timeout, PlayersListPacket player, std::string attribute){
//     	int seconds = 0 ;
//     	std::string str = "" ;
//     	if(timeout){
//     		while(seconds <= timeout){
// 	    		sleep(1);
// 	    		seconds++;
// 	    		std::getline(std::cin, str);
// 	    		if(str.size() > 0){
// 	    			break ;
// 	    		}
// 	    	}
//     	}
//     	else{
//     		std::getline(std::cin, str);
//     	}
//     	return state;
//     }

DataPacket Interpreter::getVariablesInvolved(const DataPacket& dp) const{
    std::string variable = "";
    std::size_t openingBracketIndex = dp.getData().find("{");
    std::size_t closingBracketIndex = dp.getData().find("}") ;
    variable  = dp.getData().substr(openingBracketIndex+1, closingBracketIndex - (openingBracketIndex+1));
    return DataPacket(variable) ;
}
DataPacket Interpreter::getCleanMessage(const DataPacket& dp) const{
    std::size_t openingBracketIndex = dp.getData().find("{");
    return DataPacket(dp.getData().substr(0, openingBracketIndex - 1));
}
