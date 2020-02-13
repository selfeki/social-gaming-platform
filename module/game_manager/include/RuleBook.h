#include <iostream>
#include <string>
#include <sstream>
#include <iterator>
#include <optional>
#include <vector>
#include <map>
#include<utility>


struct DataPacket{    // wrapper around string values
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

struct classMapComp{
    bool operator() (const DataPacket& dp1, const DataPacket& dp2)const{
        return dp1.getData().compare(dp2.getData()) ;
    }
};

struct MapOfDataPackets{
    std::map<DataPacket, DataPacket, classMapComp> dataMap ;
    MapOfDataPackets(std::pair<DataPacket, DataPacket> givenPair){dataMap.insert(givenPair);};
    MapOfDataPackets(std::map<DataPacket, DataPacket, classMapComp> maper):
    dataMap{maper}
    { }
    MapOfDataPackets(const MapOfDataPackets& mapOfData) {dataMap.insert(mapOfData.dataMap.begin(), mapOfData.dataMap.end());};
    void insertNew(const DataPacket& dp1, const DataPacket& dp2){ dataMap.insert(std::pair<DataPacket, DataPacket>(dp1, dp2));};
    std::map<DataPacket, DataPacket, classMapComp>& getDataMap(){return dataMap ;} ;
    std::map<DataPacket, DataPacket, classMapComp>::iterator getIterator(){return dataMap.begin();};
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
    State(MapOfDataPackets dataMap, PlayersListPacket players):
    stateMap{dataMap},
    playersInvolved{players}
    { }
    
};

class GlobalMessage {                              // 1. GlobalMessage only takes in a 'Message' as String.
public:                                           // 2. Take the message and get the variable out.
    DataPacket message ;
    GlobalMessage(DataPacket message):
    message{message}
    { }
};

class Message{                                    // Message requires 'Message' as String & 'Receipents' as Player
public:
    DataPacket message ;
    PlayersListPacket recipents;
    Message(DataPacket message, PlayersListPacket recipents):
    message{message},
    recipents{recipents}
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

class InputText{
public:
    DataPacket dP;
    PlayersListPacket player;
    int timeout ;
    std::string playerAttribute ;
    InputText(DataPacket dp, Player player, std::string attribute):
    dP{dp},
    player{PlayersListPacket({player})},
    timeout{0},
    playerAttribute{attribute}
    { }
    InputText(DataPacket dp, PlayersListPacket player, int timeout, std::string attribute):
    dP{dp},
    player{player},
    timeout{timeout},
    playerAttribute{attribute}
    { }
};
