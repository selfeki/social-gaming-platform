/////////////////////////////////////////////////////////////////////////////
//                         Single Threaded Networking
//
// This file is distributed under the MIT License. See the LICENSE file
// for details.
/////////////////////////////////////////////////////////////////////////////


#include "Server.h"
#include "command.h"
//#include "game_manager.h"
//#include "dsl_interpreter.h"
//#include "json_parser.h"
#include "cereal.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>



using networking::Server;
using networking::Connection;
using networking::Message;
using commandSpace::commandType;
using commandSpace::Command;


std::vector<Connection> clients;
json json_parser;
//initialize game manager
//GameManager game_manager

//message types possible
enum MessageType { COMMAND, GAME_CONFIG, NORMAL  };


/*Nikola's code */
//function to format a message object to a particular client
std::deque<Message> sendToClient(const Connection& client, const std::string& log){
  std::deque<Message> outgoing;
  outgoing.push_back({client,log});
  return outgoing;
}

std::ostringstream memberCommand(){
  std::ostringstream result;
  result<<"Command: Member List\n";
  for (auto client : clients){
    result<<client.id<<"\n";
  }
  return result;
} 

/*Nikola's code */

void onConnect(Connection c) {
  std::cout << "New connection found: " << c.id << "\n";
  clients.push_back(c);
}


void onDisconnect(Connection c) {
  std::cout << "Connection lost: " << c.id << "\n";
  auto eraseBegin = std::remove(std::begin(clients), std::end(clients), c);
  clients.erase(eraseBegin, std::end(clients));
}


struct MessageResult {
  std::string result;
  bool shouldShutdown;
  Connection sentFrom;
  commandType userCommand;
};


/*
* processMessages is going to be a very involved function where 
* the meat of the game server logic will take place. I suggest 
* breaking it up into sub routines
*/

std::vector<MessageResult> processMessages(Server& server, const std::deque<Message>& incoming) {
  std::vector<MessageResult> processedMessages;
  std::ostringstream result;
  Command userCommand;
  bool quit = false;
  for (auto& message : incoming) {
    Connection sentFrom = message.connection;
    
    MessageType msg_type;




    commandType commandRecieved = userCommand.evaluateMessage(message.text);
    /*
    * FOR 
    */


    switch(commandRecieved){
        case commandType::message:
          result << message.connection.id << "> " << message.text << "\n";
          break;
        case commandType::listMember:
          result << message.connection.id << "> " << message.text << "\n";
          result<<memberCommand().str();
          break;
        case commandType::quitFromServer:
          server.disconnect(message.connection);
          break;
        case commandType::shutdownServer:
          std::cout << "Shutting down.\n";
          quit = true;
          break;
        default:
          result << message.connection.id << "> " << message.text << "\n";
          result<<"Command not defined.\n";
          break;
    }
    processedMessages.push_back({result.str(),quit,sentFrom,commandRecieved});
  }
  return processedMessages;
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

  try {
    //game_manager.setUp(server_config)
  } catch (.../* const GameManagerError& e */ ){ //I suggest custom error handing class to catch the various configuration errors
    std::cerr << "Game configuration failed";
              //<< e.what() << '\n'
              //<< e.where() << '\n,
  }

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
    //I suggest the following

    std::vector<MessageResult> processedMessages = processMessages(server, incoming);

    
    for(auto message : processedMessages){
      //If user input is a message send it to every client
      if(message.userCommand != commandType::message){
        std::cout<<"Command recieved"<<std::endl;
        auto outgoing = sendToClient(message.sentFrom,message.result);
        server.send(outgoing);
      }else{
        std::cout<<"Message recived"<<std::endl;
        auto outgoing = buildOutgoing(message.result);
        server.send(outgoing);
      }
      
      if(message.shouldShutdown){
        shouldQuit = message.shouldShutdown;
      }
    }

    if (shouldQuit || errorWhileUpdating) {
      break;
    }

    sleep(1);
  }

  return 0;
}

