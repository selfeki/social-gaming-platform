#include <iostream>
#include <sqlite3.h>
#include <vector>
#include "PlayerDatabase.h"

PlayerDatabase::PlayerDatabase(){
          int status = sqlite3_open("player.db", &playerDB);
          std::string createTable = "CREATE TABLE IF NOT EXISTS PLAYERS("
                                    "UserId INT PRIMARY KEY NOT NULL,"
                                    "UserName TEXT NOT NULL,"
                                    "Points INT NOT NULL,"
                                    "GamesPlayed INT NOT NULL,"
                                    "GamesWon INT NOT NULL);";
          char* errorMsg;
          int createTableStatus = sqlite3_exec(playerDB, createTable.c_str(), NULL, 0, &errorMsg);
};

static int callback(void* data, int rows, char** argv, char** azColName){
    int i;
    fprintf(stderr, "%s: ", (const char*)data);
    for (i = 0; i < rows; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

void PlayerDatabase::dbInsertPlayer(std::string playerName, int id) {
    std::string insertStatement = std::string("INSERT INTO PLAYERS VALUES ("+ std::to_string(id) + ", '"+playerName+"', 0, 0, 0);");
    char* errormsg;
    int insertStatus = sqlite3_exec(playerDB, insertStatement.c_str(), NULL, 0, &errormsg);
        //std::cout << "insert unsucessful: " << errormsg;
        if (insertStatus != SQLITE_OK){
            std::cout << "DB unable to insert:  " << sqlite3_errmsg(playerDB) << std::endl;
        }
  }

void PlayerDatabase::dbDeletePlayer(int id) {
        std::string deleteStatement = "DELETE FROM PLAYERS WHERE UserID = " + std::to_string(id) + ";";
        char* errorMsg;
        int deleteStatus = sqlite3_exec(playerDB, deleteStatement.c_str(), callback, NULL, &errorMsg);
        if (deleteStatus != SQLITE_OK)
          std::cout << "delete unsuccessful: " << errorMsg << std::endl;
    }

//accepts vector of player IDs and their corresponding new point counts and updates the database
void PlayerDatabase::dbUpdatePlayer(std::string where_col, std::vector<std::string> where_list, std::string set_col, std::vector<std::string> val_list) {
    int i = 0;
    char* errorMsg;
    for(std::string n : where_list) {
        std::string updateStatement = "UPDATE PLAYERS SET " + set_col + " = " + val_list[i] + " WHERE " + where_col + " = " + n + ";";
        i++;
        int updateStatus = sqlite3_exec(playerDB, updateStatement.c_str(), callback, NULL, &errorMsg);
        if (updateStatus != SQLITE_OK){
            std::cout << "update unsucessful: " << errorMsg << std::endl;
        }
    }
}
void PlayerDatabase::dbSelectValues(std::string where_col, std::vector<std::string> select_list) {
    char* errormsg;
    int i = 0;
    std::string selectCols;
    /*
    //take list of columns to select and concatenate them for SQL SELECT statement
    for(auto const& n : select_list) {
        //there is a better way to do this, let me think on it
        if (i == select_list.size()-1) {
            selectCols += n;
        }
        else {
            selectCols += n + ", ";
            i++;
        }
    }
    std::cout << "dbSelectValues: selectCols = " << selectCols << std::endl;
    std::string selectBilly = "SELECT " + selectCols + " FROM PLAYERS";
    */
    std::string selectBilly = "SELECT * FROM PLAYERS WHERE UserId = 22";
    std::string data("callback function");
    int selectStatus = sqlite3_exec(playerDB, selectBilly.c_str(), callback, (void*)data.c_str(), &errormsg);
        if (selectStatus != SQLITE_OK){
           std::cout << "Select could not be performed: " <<sqlite3_errmsg(playerDB) << std::endl;
        }
}
