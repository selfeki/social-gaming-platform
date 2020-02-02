/////////////////////////////////////////////////////////////////////////////
//                         Single Threaded Networking
//
// This file is distributed under the MIT License. See the LICENSE file
// for details.
/////////////////////////////////////////////////////////////////////////////

#include "Server.h"
#include "GameManager.h"

//#include "dsl_interpreter.h"
//#include "json_parser.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

using networking::Server;
using networking::Connection;
using networking::Message;


typedef uintptr_t UniqueConnectionID;
typedef messageReturn<UniqueConnectionID> messageReturnAlias;
typedef GameManager<UniqueConnectionID> GameManagerAlias;


//typedef commandSpace::Command<Connection, int> CommandType;
//typedef commandSpace::MessageResult<Connection, int> MessageResultType;

//messages to send to server and a shouldQuit flag.
//using namespace commandSpace;
//using commandSpace::commandType;
//using commandSpace::Command<std::string, std::string>;
//using commandSpace::MessageResult<std::string, std::string>;

std::vector<Connection> clients;


json json_parser;
//initialize game manager
GameManagerAlias game_manager;

std::unordered_map<UniqueConnectionID, Connection> idConnectionMap;

std::deque<messageReturnAlias> gameMessageQueue;
std::deque<Message> networkMessageQueue;


/*
  transform game messages in game message queue to network messages in
  network message queue, then empty game message queue 
*/
bool gameMessagesToNetworkMessages() {
  bool quit = false;
  for(auto& message : gameMessageQueue) {

    std::string log = message.message;
    Connection client;
    if(message.shouldShutdown){
      quit = true;
    }
    for(auto& player : message.sendTo) {

      client = idConnectionMap[player];

      networkMessageQueue.push_back({client, log});
    }
  }
  gameMessageQueue.clear();
  return quit;
}





//message types possible
enum MessageType { COMMAND, GAME_CONFIG, NORMAL  };

MessageType getMessageType(const std::string &_message) {

  if(_message[0] == '/') {    //it is a command
    return MessageType::COMMAND;
  } else {     //it is a regular message
    return MessageType::NORMAL;
  }
}

/*
* Interpret command and call appropriate game_manager api function
*/


std::vector<messageReturnAlias> parseCommandAndCollectResponse(const std::string& message, UniqueConnectionID id){
  
  std::vector<messageReturnAlias> return_message;

  if(message == "/member"){
    return_message = game_manager.returnRoomMembersCommand(id);
  }
  else if(message == "/room"){
    return_message = game_manager.returnRoomCommand(id);
  }
  else if(message == "/create"){
    return_message = game_manager.createRoomCommand(id);
  }
  else if(message == "/join"){
    return_message = game_manager.joinRoomCommand(id);
  }
  else if(message == "/kick"){
    return_message = game_manager.kickPlayerCommand(id);
  }
  else if(message == "/quit"){
    return_message = game_manager.leaveRoomCommand(id);
  }
  else if(message == "/initgame"){
    return_message = game_manager.initRoomCommand(id);
  }
  else if(message == "/shutdown"){
    //return_message = game_manager.createRoomCommand(id);
  }
  else {
    //return_message = game_manager.createRoomCommand(id);
  }
  return return_message;
}






/*Nikola's code */
//function to format a message object to a particular client
std::deque<Message> sendToClient(const Connection& client, const std::string& log){
  std::deque<Message> outgoing;
  outgoing.push_back({client,log});
  return outgoing;
}


/*Nikola's code */

void onConnect(Connection c) {
  std::cout << "New connection found: " << c.id << "\n";
  //clients.push_back(c);
  std::pair<UniqueConnectionID, Connection> entry{c.id, c};
  idConnectionMap.insert(entry);

  //override game manager and send back server welcome message...

  networkMessageQueue.push_back({c, "Welcome to the server! Enter a command..."});


}


void onDisconnect(Connection c) {
  std::cout << "Connection lost: " << c.id << "\n";
  //auto eraseBegin = std::remove(std::begin(clients), std::end(clients), c);
  //clients.erase(eraseBegin, std::end(clients));
  idConnectionMap.erase(c.id);
  //tell the game manager the player disconnected
}


/*
* processMessages is going to be a very involved function where 
* a lot of the game server logic will take place. I suggest 
* breaking it up into sub routines
*/

void
processMessages(Server& server, const std::deque<Message>& incoming) {
  //std::vector<messageReturnAlias> processedMessages;

  //bool quit = false;
  for (auto& message : incoming) {
    Connection sentFrom = message.connection;
    MessageType msg_type = getMessageType(message.text);


   switch(msg_type) {
      case MessageType::COMMAND:
      {
        /*
        * game_manager should encapsulate the entire logic of the game server and simply return a deque/queue
        * of messages each with a client to send to. 
        * */
        std::vector<messageReturnAlias> cmd_messages = parseCommandAndCollectResponse(message.text, sentFrom.id);
        for(auto cmd_message : cmd_messages) {
          gameMessageQueue.push_back(cmd_message);
        }

        break;
      }
      case MessageType::NORMAL:
      {
        /* 
        * Any message without a special prefix will be interpreted as a gameplay message. 
        * Will be send to the DSL interpreter. I think it may be easiest to have
        * the game objects and the DSL intepreter library be called from inside
        * the game manager.
        * 
        * Example use case: If all players have to send something to a game, then each time a
        * player sends something, game_manager could return a vector of messages to
        * each player in that particular game reading something like  "<player> has submitted their message..."
        * After each player has submitted, the game would continue on.
        * 
        * */
        std::cout << "sup\n";


        std::vector<messageReturnAlias> game_messages = game_manager.handleGameMessage(message.text, sentFrom.id);
        for(auto game_message : game_messages) {
          gameMessageQueue.push_back(game_message);
        }
        std::cout << "sup\n";

        break;
      }
      default:

        break;
   }
  }
  //convert messages returned from game manager class to a structure interpretable by the networking module.
}

std::deque<Message> buildOutgoing(const std::string& log) {
  std::deque<Message> outgoing;
  for (auto client : clients) {
    outgoing.push_back({client, log});
  }
  return outgoing;
}


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
  if (argc < 3) {
    std::cerr << "Usage:\n  " << argv[0] << " <port> <html response>\n"
              << "  e.g. " << argv[0] << " 4002 ./webchat.html\n";
    return 1;
  }

  json server_config;

  if(argc == 4) {
    //get server config json from command line
    try {
      server_config = json::parse(argv[3]);
    }
    catch(json::parse_error& e) {
      std::cerr << "Your JSON isn't right bro.\n"
                << "message: " << e.what() << '\n'
                << "exception id" << e.id << '\n'
                << "byte position of error: " << e.byte << std::endl;
      return 1;
    }

  } else {
    //else use default game config
     server_config =
     {
         {"something", "something"},
         {"a number", 42},
         {"another number", 69},
         {"something", true},
         {"a bunch of somethings", {{"something", 69}, {"something", 69}}},
         {"array of somethings", {69, 42, 69}}
     };
  }

  /*
  Configurate game_manager...
  */

  try {
    //game_manager.setUp(server_config);
  } catch (.../*const GameManagerException& e*/){ //custom error handing class to catch the various configuration errors?
    std::cerr << "Server configuration failed";
              //<< e.what() << '\n'
              //<< e.where() << '\n,
  }

  std::cout << "howdy" << std::endl;
  /*
  * Command module will have to keep track of state of game server (rooms, players)
  * to implement its commands, so pass in pointer to game_manager object. 
  */

  unsigned short port = std::stoi(argv[1]);
  Server server{port, getHTTPMessage(argv[2]), onConnect, onDisconnect};

  while (true) {
    
    /*
    * Main Game Server Loop
    */

    bool errorWhileUpdating = false;
    try {
      server.update();
    } catch (std::exception& e) {
      std::cerr << "Exception from Server update:\n"
                << " " << e.what() << "\n\n";
      errorWhileUpdating = true;
    }
    bool shouldQuit = false;

    /*
    * Get all messages recieved since server.update()
    */ 
    auto incoming = server.receive();



    //std::pair<std::deque<Message>, bool> processedMessages = processMessages(server, incoming);
    processMessages(server, incoming);

    for(auto &msg : incoming) {
      std::cout << msg.text << "\n";
    }


    shouldQuit = gameMessagesToNetworkMessages();


    //shouldQuit = processedMessages.second;

    server.send(networkMessageQueue);    
    
    networkMessageQueue.clear();

    /*
    for(auto message : processedMessages){


      //auto outgoing = buildOutgoing(message.result);
      server.send(outgoing);

      
      if(message.shouldShutdown){
        shouldQuit = message.shouldShutdown;
      }
    }
    */
    

    if (shouldQuit || errorWhileUpdating) {
      break;
    }

    sleep(1);
  }

  return 0;
}

