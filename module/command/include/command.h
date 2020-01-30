
#include <string>


namespace commandSpace {

enum commandType {
    listMember,// = "/member";
    listRoom ,//= "/room"
    createRoom ,   //= "/create"
    joinRoom,   //command to join a room
    nullCommand //does belong to any command 
};

class Command{
public:
    Command();
    //void recieveCommand();
    void evaluateMessage(const std::string& message);
    //void requestInfoToServer(enum  command);
    void returnError();
    void printResult();

private:
    commandType commandRecieved;



};

}