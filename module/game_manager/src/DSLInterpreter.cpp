#include<utility>
#include "DSLInterpreter.h"

State Interpreter:: operator()(GlobalMessage const& gM) const{
    DataPacket variable = getVariablesInvolved(gM.message);
    MapOfDataPackets dataMap(std::pair<DataPacket, DataPacket>(DataPacket("type"), DataPacket("Main_Screen")));
    dataMap.insertNew(DataPacket("message"), gM.message);
    if(variable.getData().length() > 0){
        dataMap.insertNew(DataPacket("variable"), variable);
    }
    return State(dataMap, players);
}

State Interpreter:: operator()(Message const& m) const{
    DataPacket variable = getVariablesInvolved(m.message.getData());
    MapOfDataPackets dataMap(std::pair<DataPacket, DataPacket>(DataPacket("type"), DataPacket("to_recipents")));
    dataMap.insertNew(DataPacket("message"), m.message);
    if(variable.getData().length()>0){
        dataMap.insertNew(DataPacket("variable"), variable);
    }
    return State(dataMap, m.recipents);
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
    return State(dataMap, players);
}

State Interpreter:: operator()( InputText const& iT) const{
    // return State(iT.dP.getData(), PlayersListPacket(iT.player), std::nullopt) ;
}

State Interpreter:: takeInput(int timeout, PlayersListPacket player, std::string attribute){
    // int seconds = 0 ;
    // std::string str = "" ;
    // if(timeout){
    //     while(seconds <= timeout){
    //         sleep(1);
    //         seconds++;
    //         std::getline(std::cin, str);
    //         if(str.size() > 0){
    //             break ;
    //         }
    //     }
    // }
    // else{
    //     std::getline(std::cin, str);
    // }
    // return State(DataPacket(str), player, std::nullopt);
}

DataPacket Interpreter::getVariablesInvolved(DataPacket dp) const{
    std::cout<<"HAHA"<<std::endl;
    std::string variable = "";
    std::size_t openingBracketIndex = dp.getData().find("{");
    std::size_t closingBracketIndex = dp.getData().find("}") ;
    variable  = dp.getData().substr(openingBracketIndex+1, closingBracketIndex - (openingBracketIndex+1));
    return DataPacket(variable) ;
}

// Main function for sample testing
// int main(){
//     std::vector<Player> players ;
//     std::vector<Player> pl2;

//     Player p1("p1") ;
//     Player p2("p2") ;

//     p1.wins = 2 ;
//     p2.wins = 3 ;
//     players.push_back(p1);
//     players.push_back(p2);
//     pl2.push_back(p2) ;


//     DataPacket dpMe ("Good Job! {player.name}") ;
//     DataPacket dpGM("Welcome {player.Mehtab}") ;
//     DataPacket vari("{player.wins}");
//     DataPacket so("False");
//     // DataPacket dpScores("wins");
//     // DataPacket dpAttr("command") ;
//     Message m(dpMe, pl2) ;
//     GlobalMessage gM(dpGM) ;
//     Scores scores(vari, so);
//     // Scores scores(dpScores) ;
//     // InputText<Player> iT (DataPacket("Please Enter :"), {p1}, 0, "command" );
//     //std::cout<<gM.message.getData()<<std::endl;

//     Rule ruleM(m);
//     Rule ruleGM ((gM));
//     Rule ruleSc ((scores));
//     // Rule ruleM ((m));
//     // Rule ruleS ((scores));
//     // Rule ruleIn ((iT)) ;

//     State dp = boost::apply_visitor(Interpreter(players), ruleSc);
//     std::map<DataPacket, DataPacket, classMapComp>::iterator it = dp.stateMap.getDataMap().begin() ;

//     for(; it != dp.stateMap.getDataMap().end(); it++){
//             std::cout<<it->first.getData()<< " :: "<<it->second.getData()<<std::endl;
//         }

//     return 0 ;
// }
