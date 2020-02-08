#include <iostream>  
#include <string>    
#include <sstream>
#include <iterator>
#include <optional>

struct DataPacket{
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
};

template <typename T>
struct PlayersListPacket{
  std::vector<T> players ;
  PlayersListPacket(std::vector<T> players):
    players{players}
    { }
   std::vector<T> getPlayers(){return players;};
};

class GlobalMessage {                              // GlobalMessage only takes in a 'Message' as String.
  public:
  DataPacket message ;
  GlobalMessage(DataPacket message):
    message{message}
    { }
};

template <typename T>
class Message{                                    // Message requires 'Message' as String & 'Receipents' as Player
    public:
    DataPacket message ;
    PlayersListPacket<T> recipents;
    Message(DataPacket message, std::vector<T> recipents):
      message{message},
      recipents{PlayersListPacket<T>(recipents)}
      { }
};

class Scores{
  public:
  DataPacket playerAttribute ;
  Scores(DataPacket attribute):
    playerAttribute{attribute}
    { }
};

template <typename T>
class InputText{
	public:
		DataPacket dP;
		PlayersListPacket<T> player;
		int timeout ;   
		std::string playerAttribute ;
		InputText(DataPacket dp, T player, std::string attribute):
			dP{dp},
			player{PlayersListPacket<T>(player)},
			timeout{0},
			playerAttribute{attribute}
			{ }
		InputText(DataPacket dp, std::vector<T>player, int timeout, std::string attribute):
			dP{dp},
			player{PlayersListPacket<T>(player)},
			timeout{timeout},
			playerAttribute{attribute}
			{ }
};