#include "cereal.h"
#include <exception>




class GameManagerException: public std::exception {
public:
    GameManagerException(std::string _info, int _err_id, std::string _json_field);
    std::string getInfo() const;
    int getCode() const;
    std::string getJsonField() const;
private:
    std::string info;
    int error_id;
    std::string json_field;
}; 

class GameManager {

public:

    GameManager();

    void setUp(json server_config);

};
