#include<utility>
#include <numeric>
#include <string>
#include "DSLInterpreter.h"

State Interpreter:: operator()(GlobalMessage const& gM) const{
    DataPacket variable = getVariablesInvolved(gM.message);
    MapOfDataPackets dataMap(std::pair<DataPacket, DataPacket>(DataPacket("type"), DataPacket("Main_Screen")));
    dataMap.insertNew(DataPacket("message"), gM.message);
    if(variable.getData().length() > 0){
        dataMap.insertNew(DataPacket("variable"), variable);
    }
    return State(dataMap, players, state.arithmeticVarMap);
}

State Interpreter:: operator()(MessageRule const& m) const{
    DataPacket variable = getVariablesInvolved(m.message.getData());
    MapOfDataPackets dataMap(std::pair<DataPacket, DataPacket>(DataPacket("type"), DataPacket("to_recipents")));
    dataMap.insertNew(DataPacket("message"), m.message);
    if(variable.getData().length()>0){
        dataMap.insertNew(DataPacket("variable"), variable);
    }
    return State(dataMap, m.recipents, state.arithmeticVarMap);
}

State Interpreter:: operator()(Scores const& sc) const{
    DataPacket variable = getVariablesInvolved(sc.variable) ;
    MapOfDataPackets dataMap(std::pair<DataPacket, DataPacket>(DataPacket("type"), DataPacket("Main_Screen")));
    dataMap.insertNew(DataPacket("sort_by"), variable);
    if(sc.sortOrder.getData().compare("true") == 0){
        dataMap.insertNew(DataPacket("sort_order"), DataPacket("ascending"));
    }
    else if(sc.sortOrder.getData().length() == 0){
        dataMap.insertNew(DataPacket("sort_order"), DataPacket("none"));
    }
    else{
        dataMap.insertNew(DataPacket("sort_order"), DataPacket("descending"));
    }
    dataMap.insertNew(DataPacket("to_display"), DataPacket("player_id"));
    dataMap.insertNew(DataPacket("display_with"), variable);
    return State(dataMap, players, state.arithmeticVarMap);
}

State Interpreter:: operator()(Add const& add) const{
    MapOfArithmeticVariables arithMap(state.arithmeticVarMap);
    ArithmeticValue ar(arithMap.getDataMap().find(add.variable)->second);
    ar.setData(ar.getData() + add.value.getData());
    arithMap.getDataMap()[add.variable] = ar;
    return State(state.stateMap, players, arithMap);
}

State Interpreter:: operator()(InputVote const& inputVote) const{ // To change to consider Time Out
    if(State(state).stateMap.getDataMap()[DataPacket("input_recieved")].getData().compare("true") != 0){
        Rule promptMessage((MessageRule(inputVote.messageTosend, inputVote.players)));
        State st = boost::apply_visitor(Interpreter(players.getPlayers(), state), promptMessage);
        std::vector<std::string> vectorOfChoices;
        for(DataPacket choice: inputVote.choices){
            vectorOfChoices.push_back(choice.getData());
        }
        std::string choicesString = std::accumulate(vectorOfChoices.begin(), vectorOfChoices.end(), std::string(" "));
        Rule promptChoices((MessageRule(DataPacket(choicesString), inputVote.players)));
        st = boost::apply_visitor(Interpreter(players.getPlayers(), state), promptChoices);
        return State(st);
    }
    MapOfArithmeticVariables choicesMap(std::pair<DataPacket, ArithmeticValue>(DataPacket("None"), ArithmeticValue(0)));
    for(Player p1: players.getPlayers()){
        DataPacket choice = State(state).stateMap.getDataMap().find(p1.name)->second;
        if(choicesMap.getDataMap().find(choice) == choicesMap.getDataMap().end()){
            choicesMap.insertNew(choice, ArithmeticValue(1));
        }else{
            choicesMap.getDataMap()[choice]= choicesMap.getDataMap()[choice].getData() + 1;
        }
    }
    DataPacket bestChoice = std::max_element(choicesMap.getDataMap().begin(), choicesMap.getDataMap().end(),
                                             [](const std::pair<DataPacket, ArithmeticValue> pair1, const std::pair<DataPacket, ArithmeticValue> pair2){
                                                 return pair1.second.getData() < pair2.second.getData() ;
                                             })->first;
    State st(state);
    st.stateMap.insertNew(inputVote.resultVariable, bestChoice) ;
    st.stateMap.getDataMap()[DataPacket("input_recieved")] = DataPacket("false");
    return st;
    
}
State Interpreter:: operator()( InputText const& iT) const{
    if(State(state).stateMap.getDataMap()[DataPacket("input_recieved")].getData().compare("true") != 0){
        Rule promptMessage((MessageRule(iT.messageTosend, iT.player)));
        State st = boost::apply_visitor(Interpreter(players.getPlayers(), state), promptMessage);
        return st;
    }
    State st(state);
    DataPacket dataRecieved = st.stateMap.getDataMap()[DataPacket("input_recieved")] ;
    st.stateMap.insertNew(iT.resultVariable, dataRecieved);
    st.stateMap.getDataMap()[DataPacket("input_recieved")] = DataPacket("false");
    return st ;
}

State Interpreter:: takeInput(int timeout, PlayersListPacket player, std::string attribute){
    int seconds = 0 ;
    std::string str = "" ;
    if(timeout){
        while(seconds <= timeout){
            sleep(1);
            seconds++;
            std::getline(std::cin, str);
            if(str.size() > 0){
                break ;
            }
        }
    }
    else{
        std::getline(std::cin, str);
    }
    return state;
}

DataPacket Interpreter::getVariablesInvolved(DataPacket dp) const{
    std::string variable = "";
    std::size_t openingBracketIndex = dp.getData().find("{");
    std::size_t closingBracketIndex = dp.getData().find("}") ;
    variable  = dp.getData().substr(openingBracketIndex+1, closingBracketIndex - (openingBracketIndex+1));
    return DataPacket(variable) ;
}

// Main function for sample testing
