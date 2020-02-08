/////////////////////////////////////////////////////////////////////////////
//                         Single Threaded Networking
//
// This file is distributed under the MIT License. See the LICENSE file
// for details.
/////////////////////////////////////////////////////////////////////////////

#include "Server.h"
#include "GameManager.h"
#include "jsonconfig.h"
#include "command.h"
//#include "dsl_interpreter.h"
//#include "json_parser.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <iterator>
#include <string>
#include <unistd.h>
#include <vector>

using networking::Server;
using networking::Connection;
using networking::Message;
using namespace commandSpace;


typedef uintptr_t UniqueConnectionID;
typedef messageReturn<UniqueConnectionID> messageReturnAlias;
typedef GameManager<UniqueConnectionID> GameManagerAlias;

//std::vector<Connection> clients;

std::string default_json = "templates/server/default.json";

GameManagerAlias game_manager;

//map from connectionID to connection object
std::unordered_map<UniqueConnectionID, Connection> idConnectionMap;

//messages returned from game manager
std::deque<messageReturnAlias> gameMessageQueue;

//messages ready to be sent to networking
std::deque<Message> networkMessageQueue;


/*
transform game messages in game message queue to network messages in
network message queue, then empty the game message queue. These act like queues,
sof first game message processed is first to be put into the network queue.
*/
bool gameMessagesToNetworkMessages() {
  bool quit = false;
  while(!gameMessageQueue.empty()) {

    messageReturnAlias message = gameMessageQueue.front();
    gameMessageQueue.pop_front();
    std::string log = message.message;

    if(message.shouldShutdown){
      quit = true;
    }

    UniqueConnectionID player  = message.sendTo;
    Connection client = idConnectionMap.at(player);
    std::cout << client.id << "\n";
    std::cout << log << "\n";
    networkMessageQueue.push_back({client, log});

  }
  return quit;
}





//message types possible (tentative)
enum MessageType { COMMAND, GAME_CONFIG, NORMAL  };


MessageType getMessageType(const std::string &_message) {
  if(_message[0] == '/') {    //it is a command
    return MessageType::COMMAND;
  } else {     //it is a regular message
    return MessageType::NORMAL;
  }
}


/*
* Interpret command and call appropriate game_manager api function (there might be a better way to do this)
*/
std::vector<messageReturnAlias> parseCommandAndCollectResponse(const std::string& message, UniqueConnectionID id){

  std::vector<messageReturnAlias> game_manager_message;

  //tokenize the string, split by ' '
  std::vector<std::string> tokens;
  std::istringstream iss(message);
  std::copy(std::istream_iterator<std::string>(iss),
              std::istream_iterator<std::string>(),
              std::back_inserter(tokens));

  //command class to translate to enumerator 
  Command command(tokens[0]);
  switch (command.getCommandType()){
    case commandType::listMember :
      game_manager_message = game_manager.returnRoomMembersCommand(id);
      break;
    case commandType::listRoom :
      game_manager_message = game_manager.returnRoomCommand(id);
      break;
    case commandType::createRoom :
      game_manager_message = game_manager.createRoomCommand(id);
      break;
    case commandType::joinRoom:
      game_manager_message = game_manager.joinRoomCommand(id, tokens[1]);
      break;
    case commandType::kickUser:
      game_manager_message = game_manager.kickPlayerCommand(id, tokens[1]);
      break;
    case commandType::quitFromServer:
      game_manager_message = game_manager.leaveRoomCommand(id);
      break;
    case commandType::initGame:
      game_manager_message = game_manager.initRoomCommand(id);
      break;
    case commandType::shutdownServer:

      break;
  };

  return game_manager_message;
}



/*Nikola's code */
//function to format a message object to a particular client
std::deque<Message> sendToClient(const Connection& client, const std::string& log){
  std::deque<Message> outgoing;
  outgoing.push_back({client,log});
  return outgoing;
}
/*Nikola's code */

//Whenever a client connections, this function is called it inserts connection and connection id into map
void onConnect(Connection c) {
  std::cout << "New connection found: " << c.id << "\n";
  std::pair<UniqueConnectionID, Connection> entry{c.id, c};
  idConnectionMap.insert(entry);

  //override game manager and send back server welcome message...
  networkMessageQueue.push_back({c, "Welcome to the server! Enter a command..."});
}

//Called whenenver a client disconnects. Should handle disconneting player from game room
void onDisconnect(Connection c) {
  std::cout << "Connection lost: " << c.id << "\n";
  idConnectionMap.erase(c.id);
  //...

}


void
processMessages(Server& server, const std::deque<Message>& incoming) {

  for (auto& message : incoming) {
    Connection sentFrom = message.connection;
    MessageType msg_type = getMessageType(message.text);

    Command command(message.text);
    commandType recieved = command.getCommandType();
    //variable type input is type alias to string defined in command.h
    std::vector<input> tokens = command.getTokens();
    if(recieved==commandType::message)
    {
      std::vector<messageReturnAlias> game_messages = game_manager.handleGameMessage(message.text, sentFrom.id);
        for(auto game_message : game_messages) {
          gameMessageQueue.push_back(game_message);
        } 
    }
    else {
      //handle command in game manager
      std::vector<messageReturnAlias> cmd_messages;
      switch (recieved){
        case commandType::listMember :
          cmd_messages = game_manager.returnRoomMembersCommand(sentFrom.id);
        break;
        case commandType::listRoom :
          cmd_messages = game_manager.returnRoomCommand(sentFrom.id);
          break;
        case commandType::createRoom :
          cmd_messages = game_manager.createRoomCommand(sentFrom.id);
          break;
        case commandType::joinRoom:
          cmd_messages = game_manager.joinRoomCommand(sentFrom.id, tokens[1]);
          break;
        case commandType::kickUser:
          cmd_messages = game_manager.kickPlayerCommand(sentFrom.id, tokens[1]);
          break;
        case commandType::quitFromServer:
          cmd_messages = game_manager.leaveRoomCommand(sentFrom.id);
          break;
        case commandType::initGame:
          cmd_messages = game_manager.initRoomCommand(sentFrom.id);
          break;
        case commandType::shutdownServer:

          break;
        };
        //create message vector to send out 
        for(auto cmd_message : cmd_messages) {
          gameMessageQueue.push_back(cmd_message);
        }
      }
      
    };
    /*
    switch(msg_type) {
      case MessageType::COMMAND:
      {
        //get response to command from game manager, push responses to gameMessageQueue
        std::vector<messageReturnAlias> cmd_messages = parseCommandAndCollectResponse(message.text, sentFrom.id);
        for(auto cmd_message : cmd_messages) {
          gameMessageQueue.push_back(cmd_message);
        }

        break;
      }
      case MessageType::NORMAL:
      {
        //messages with no '/' prefix will be interepreted as gameplay
        std::vector<messageReturnAlias> game_messages = game_manager.handleGameMessage(message.text, sentFrom.id);
        for(auto game_message : game_messages) {
          gameMessageQueue.push_back(game_message);
        }
        break;
      }
      default:
        break;
   }
   */
  }


/*
std::deque<Message> buildOutgoing(const std::string& log) {
  std::deque<Message> outgoing;
  for (auto client : clients) {
    outgoing.push_back({client, log});
  }
  return outgoing;
}
*/

std::string
getHTTPMessage(const char* htmlLocation) {
  if (access(htmlLocation, R_OK ) != -1) {
    std::ifstream infile{htmlLocation};
    return std::string{std::istreambuf_iterator<char>(infile),
                       std::istreambuf_iterator<char>()};
  } else {
    std::cerr << "Unable to open HTML index file:\n"
              << htmlLocation << "\n";
    std::exit(-1);
  }
}


int
main(int argc, char* argv[]) {
  
  s_config server_config;

  if (argc < 2) {
    server_config = server_config::load_file(default_json,true);
  } else {
    server_config = server_config::load_file(argv[1],true);
  }
  
  if (server_config.err) {
    return 1;
  }
  
  //example
  //g_config game = game_config::load_file("templates/game/rps.json", true);
  //std::cout << game.player_count["min"] << std::endl;

  /*
  Try to configurate game_manager... with custom error handling to give useful
  error messages?
  */
  try {
    //game_manager.setUp(server_config);
  } catch (.../*const GameManagerException& e*/){
    std::cerr << "Server configuration failed";
              //<< e.what() << '\n'
              //<< e.where() << '\n,
  }

  unsigned short port = server_config.port;

  //getHTTPMessage takes char array not string, LAME
  int str_size = server_config.htmlpath.size()+1;
  char html[str_size+1];
  server_config.htmlpath.copy(html, str_size+1);
  html[str_size] = '\0';

  Server server{port, getHTTPMessage(html), onConnect, onDisconnect};

  /*
  * Main Game Server Loop
  */

  while (true) {

    bool errorWhileUpdating = false;

    try {
      server.update();
    } catch (std::exception& e) {
      std::cerr << "Exception from Server update:\n"
                << " " << e.what() << "\n\n";
      errorWhileUpdating = true;
    }
    bool shouldQuit = false;


    //Get all messages recieved since server.update()
    auto incoming = server.receive();

    //Process messages and put them in gameMessagesQueue (global queue)
    processMessages(server, incoming);

    //if an admin runs a comman to shutdown server, shouldQuit will be set to true
    shouldQuit = gameMessagesToNetworkMessages();
    server.send(networkMessageQueue);
    networkMessageQueue.clear();

    if (shouldQuit || errorWhileUpdating) {
      break;
    }
    sleep(1);
  }
  return 0;
}
