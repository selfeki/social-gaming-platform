#include <iostream>
#include <string>
#include <sstream>
#include <iterator>
#include <optional>
#include <vector>
#include <map>
#include<utility>


struct DataPacket{         //  Wrapper for Strings
    std::string data ;
    DataPacket():
    data{""}
    { }
    DataPacket(std::string str):
    data{str}
    { }
    DataPacket(const DataPacket& dp):
    data{dp.data}
    { }
    std::string getData() const {return data;};
    bool operator<(const DataPacket& dp2) const {return this->data > dp2.getData();};
};

struct ArithmeticValue{       // Wrapper for Integers
    int data;
    ArithmeticValue():
    data{0}
    { }
    ArithmeticValue(int val):
    data{val}
    { }
    ArithmeticValue(const ArithmeticValue& aValue):
    data{aValue.data}
    { }
    int getData() const {return data;};
    void setData(int val) {data = val;};
    bool operator < (const ArithmeticValue& aValue) const {return data > aValue.getData() ;};
};

struct classMapComp{
    bool operator() (const DataPacket& dp1, const DataPacket& dp2)const{
        return dp1.getData().compare(dp2.getData()) ;
    }
};

struct MapOfDataPackets{
    std::map<DataPacket, DataPacket, classMapComp> dataMap ;
    MapOfDataPackets(std::pair<DataPacket, DataPacket> givenPair){dataMap.insert(givenPair);};
    MapOfDataPackets(const MapOfDataPackets& mapOfData) {dataMap.insert(mapOfData.dataMap.begin(), mapOfData.dataMap.end());};
    void insertNew(const DataPacket& dp1, const DataPacket& dp2){ dataMap.insert(std::pair<DataPacket, DataPacket>(dp1, dp2));};
    std::map<DataPacket, DataPacket, classMapComp>& getDataMap(){return dataMap ;} ;
    std::map<DataPacket, DataPacket, classMapComp>::iterator getIterator(){return dataMap.begin();};
};

struct MapOfArithmeticVariables{
    std::map<DataPacket, ArithmeticValue, classMapComp> dataMap ;
    MapOfArithmeticVariables(std::pair<DataPacket, ArithmeticValue> givenPair){dataMap.insert(givenPair);};
    MapOfArithmeticVariables(const MapOfArithmeticVariables& arithMap){dataMap.insert(arithMap.dataMap.begin(), arithMap.dataMap.end());};
    void insertNew(const DataPacket& dp1, const ArithmeticValue& ap){ dataMap.insert(std::pair<DataPacket, ArithmeticValue>(dp1, ap));};
    std::map<DataPacket, ArithmeticValue, classMapComp>& getDataMap(){return dataMap ;} ;
    std::map<DataPacket, ArithmeticValue, classMapComp>::iterator getIterator(){return dataMap.begin();};
};

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

struct PlayersListPacket{
    std::vector<Player> players ;
    PlayersListPacket(std::vector<Player> players):
    players{players}
    { }
    std::vector<Player> getPlayers() const {return players;};
};


class State{
public:
    MapOfDataPackets stateMap ;
    PlayersListPacket playersInvolved ;
    MapOfArithmeticVariables arithmeticVarMap ;
    State(MapOfDataPackets dataMap, PlayersListPacket players, MapOfArithmeticVariables arithmeticVarmap):
    stateMap{dataMap},
    playersInvolved{players},
    arithmeticVarMap{arithmeticVarmap}
    { }
    State(const State& st):
    stateMap{st.stateMap},
    playersInvolved{st.playersInvolved},
    arithmeticVarMap{st.arithmeticVarMap}
    { }
};

class GlobalMessage {
public:
    DataPacket message ;
    GlobalMessage(DataPacket message):
    message{message}
    { }
};

class MessageRule{                                    
public:
    DataPacket message ;
    PlayersListPacket recipents;
    MessageRule(DataPacket message, PlayersListPacket recipents):
    message{message},
    recipents{recipents}
    { }
    MessageRule(const MessageRule& messageRule):
    message{messageRule.message},
    recipents{messageRule.recipents}
    { }
};

class Scores{
public:
    DataPacket variable ;
    DataPacket sortOrder ;
    Scores(DataPacket variable, DataPacket sortOrder):
    variable{variable},
    sortOrder{sortOrder}
    { }
};

class Add{
public:
    DataPacket variable;
    ArithmeticValue value;
    Add(DataPacket variable, ArithmeticValue arithVal):
    variable{variable},
    value{arithVal}
    { }
};

class InputVote{
public:
    PlayersListPacket players;
    DataPacket messageTosend ;
    std::vector<DataPacket> choices ;
    DataPacket resultVariable;
    ArithmeticValue timeout = 10 ;
    InputVote(PlayersListPacket players, DataPacket message, std::vector<DataPacket> choices, DataPacket resultVariable):
    players{players},
    messageTosend{message},
    choices{choices},
    resultVariable{resultVariable}
    { }
};

class InputText{
public:
    DataPacket messageTosend;
    PlayersListPacket player;
    ArithmeticValue timeout = 0;
    DataPacket resultVariable ;
    InputText(DataPacket message, Player player, DataPacket resVar):
    messageTosend{message},
    player{PlayersListPacket({player})},
    resultVariable{resVar}
    { }
    InputText(DataPacket message, PlayersListPacket player, ArithmeticValue timeout, DataPacket resVar):
    messageTosend{message},
    player{player},
    timeout{timeout},
    resultVariable{resVar}
    { }
};
