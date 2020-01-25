#include <iostream>
#include <sqlite3.h>
#include <vector>

void dbInsertPlayer(sqlite3* db, std::string playerName, int id);

static int callback(void* data, int rows, char** argv, char** azColName);

void dbDeletePlayer(sqlite3* db, int id);

void dbUpdatePlayer(sqlite3* db, std::string where_col, std::vector<std::string> where_list, std::string set_col, std::vector<std::string> val_list);

void dbSelectValues(sqlite3* db, std::string where_col, std::vector<std::string> select_list);

int main(int argc, char** argv){
    sqlite3* DB;
    char* errormsg;
    int status = 0;
    status = sqlite3_open("players.db", &DB);

    if (status != SQLITE_OK) {
        std::cout << "DB unable to open: " << sqlite3_errmsg(DB) << std::endl;
        return (-1);
    }
    std::cout << "Opened Database Successfully!" << std::endl;

    std::string createTable = "CREATE TABLE IF NOT EXISTS PLAYERS("
                              "UserId INT PRIMARY KEY NOT NULL,"
                              "UserName TEXT NOT NULL,"
                              "Points INT NOT NULL,"
                              "GamePlayed INT NOT NULL,"
                              "GamesWon INT NOT NULL);";

    int createTableStatus = sqlite3_exec(DB, createTable.c_str(), NULL, 0, &errormsg);
    if (createTableStatus != SQLITE_OK){
        std::cout << "Table could not be created" <<sqlite3_errmsg(DB) << std::endl;
    }

    std::vector<std::string> userIDsList = {"1","2"}; //"billy" and "bobby"
    std::vector<std::string> pointsList = {"2","2"};
    std::string selectBilly = "SELECT * FROM PLAYERS";
    std::string data("callback function");

    //INSERT TEST
    dbInsertPlayer(DB, "billy", 1);
    dbInsertPlayer(DB, "bobby", 2);
    std::cout << "After Insert\n";
    int selectStatus = sqlite3_exec(DB, selectBilly.c_str(), callback, (void*)data.c_str(), &errormsg);
    if (selectStatus != SQLITE_OK){
        std::cout << "Select could not be performed: " <<sqlite3_errmsg(DB) << std::endl;
    }

    //UPDATE TEST
    dbUpdatePlayer(DB, "UserId", userIDsList, "Points", pointsList);
    std::cout << "After Update\n";
    selectStatus = sqlite3_exec(DB, selectBilly.c_str(), callback, (void*)data.c_str(), &errormsg);
    if (selectStatus != SQLITE_OK){
        std::cout << "Select could not be performed: " <<sqlite3_errmsg(DB) << std::endl;
    }

    //DELETE TEST
    dbDeletePlayer(DB, 1);
    std::cout << "After Delete\n";
    selectStatus = sqlite3_exec(DB, selectBilly.c_str(), callback, (void*)data.c_str(), &errormsg);
    if (selectStatus != SQLITE_OK){
        std::cout << "Select could not be performed: " <<sqlite3_errmsg(DB) << std::endl;
    }

    //SELECT TEST
    std::cout << "Select Test" << std::endl;
    std::vector<std::string> cols_list = {"UserId","UserName","GamesWon"};
    dbSelectValues(DB,"Points",cols_list);
    //std::cout << selectRetclear << std::endl;

    sqlite3_close(DB);
    return (0);
}

void dbInsertPlayer(sqlite3* db, std::string playerName, int id){
    std::string insertStatement = std::string("INSERT INTO PLAYERS VALUES ("+ std::to_string(id) + ", '"+playerName+"', 0, 0, 0);");
    char* errormsg;
    int insertStatus = sqlite3_exec(db, insertStatement.c_str(), NULL, 0, &errormsg);
    if (insertStatus != SQLITE_OK){
        std::cout << "DB unable to insert:  " << sqlite3_errmsg(db) << std::endl;
    }
}

static int callback(void* data, int rows, char** argv, char** azColName){
    int i;
    fprintf(stderr, "%s: ", (const char*)data);
    for (i = 0; i < rows; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

void dbDeletePlayer(sqlite3* db, int id){
    std::string deleteStatement = "DELETE FROM PLAYERS WHERE UserID = " + std::to_string(id) + ";";
    char* errorMsg;
    int deleteStatus = sqlite3_exec(db, deleteStatement.c_str(), callback, NULL, &errorMsg);
    if (deleteStatus != SQLITE_OK){
        std::cout << "delete unsucessful: " << errorMsg << std::endl;
    }
}

//accepts vector of player IDs and their corresponding new point counts and updates the database
void dbUpdatePlayer(sqlite3* db, std::string where_col, std::vector<std::string> where_list, std::string set_col, std::vector<std::string> val_list) {
    int i = 0;
    char* errorMsg;
    for(std::string n : where_list) {
        std::string updateStatement = "UPDATE PLAYERS SET " + set_col + " = " + val_list[i] + " WHERE " + where_col + " = " + n + ";";
        i++;
        int updateStatus = sqlite3_exec(db, updateStatement.c_str(), callback, NULL, &errorMsg);
        if (updateStatus != SQLITE_OK){
            std::cout << "update unsucessful: " << errorMsg << std::endl;
        }
    }
}
void dbSelectValues(sqlite3* db, std::string where_col, std::vector<std::string> select_list) {
    char* errormsg;
    int i = 0;
    std::string selectCols;
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
    std::string data("callback function");
    int selectStatus = sqlite3_exec(db, selectBilly.c_str(), callback, (void*)data.c_str(), &errormsg);
        if (selectStatus != SQLITE_OK){
           std::cout << "Select could not be performed: " <<sqlite3_errmsg(db) << std::endl;
        }
}