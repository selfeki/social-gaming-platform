#include <string>
#include <vector>


namespace commandSpace {

enum commandType {
    listMember,
    listRoom,
    createRoom,
    joinRoom,
    kickUser,   
    nullCommand,
    quitFromServer,
    shutdownServer,
    message
};

//template <class T>
class Command{
public:
    Command();
    commandType evaluateMessage(const std::string& message);
    void requestInfoToServer(const commandType& command);
    std::string returnCommandNotFoundError();
    commandType getCommandType() const;

private:
    commandType commandRecieved;
    std::string userInput;  
};

}