#include <iostream>
#include <sqlite3.h>
#include <vector>
#include "database.h"

int callback(void* data, int rows, char** argv, char** azColName) {
    if (data != NULL) {
        //SelectValues called
        std::vector<std::string>& dataVect = *reinterpret_cast<std::vector<std::string>*>(data);
        for (int i = 0; i < rows; i++) {
            dataVect.push_back(argv[i]);
        }
    }
    else {
        //stdout print called
        for (int i = 0; i < rows; i++) {
            printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        }
        printf("\n");
    }      
    return 0;
}

Database::~Database() {
    sqlite3_close(db);
}


bool Database::validInput(const std::string& userInput) {
    size_t found = userInput.find_first_of(";,");
    if (found != std::string::npos) {
        return false;
    }
    return true;
}


void Database::OpenDB() {
    char* errormsg;
    int status = 0;
    std::string dbName = "server.db";
    status = sqlite3_open(dbName.c_str(), &db);
    /*
    //error check
    if (status != SQLITE_OK) {
        std::cout << "Database unable to open: " << sqlite3_errmsg(db) << std::endl;
    }
    std::cout << "Opened Database Successfully!" << std::endl;
    */
}

void Database::CloseDB() {
    sqlite3_close(db);
}

sqlite3* Database::dbPointer() {
    return db;
}

void Database::CreateTable(const std::string& tableName) {
    if (validInput(tableName)) {
        char* errormsg;
        std::string createTable = "CREATE TABLE IF NOT EXISTS "+ tableName +"("
                                  "UserId INT PRIMARY KEY NOT NULL,"
                                  "UserName TEXT NOT NULL,"
                                  "Points INT NOT NULL,"
                                  "GamePlayed INT NOT NULL,"
                                  "GamesWon INT NOT NULL);";

        int createTableStatus = sqlite3_exec(db, createTable.c_str(), NULL, 0, &errormsg);
        /*
        //error check
        if (createTableStatus != SQLITE_OK){
            std::cout << "Table could not be created" <<sqlite3_errmsg(db) << std::endl;
        }
        */
    }
}

void Database::InsertPlayer(const std::string& table, const std::string& playerName, const std::string& id){
    if (validInput(table) && validInput(playerName) && validInput(id)) {
        std::string insertStatement = "INSERT INTO " + table + " VALUES (" + id + ", '" + playerName + "', 0, 0, 0);";
        char* errormsg;
        int insertStatus = sqlite3_exec(db, insertStatement.c_str(), NULL, 0, &errormsg);
        /*
        //error check
        if (insertStatus != SQLITE_OK){
            std::cout << "DB unable to insert:  " << sqlite3_errmsg(db) << std::endl;
        }
        */
    }
}

void Database::DeletePlayer(const std::string& table, const std::string& id){
    if (validInput(table) && validInput(id)) {
        std::string deleteStatement = "DELETE FROM " + table + " WHERE UserID = " + id + ";";
        char* errorMsg;
        int deleteStatus = sqlite3_exec(db, deleteStatement.c_str(), callback, NULL, &errorMsg);
        /*
        //error check
        if (deleteStatus != SQLITE_OK){
            std::cout << "delete unsucessful: " << errorMsg << std::endl;
        }
        */
    }
}

/*
accepts pointer to database (db), table name in db (table), vector of columns and respective values to populate SQL WHERE (where_col, where_list), column (set_col) and a vector 
of values (set_list) ordered as where_list to populate SQL SET
dbUpdatePlayer(DB, "PLAYERS", "PlayerName", {"billy","bob"}, "Points", "2")
= SQL: UPDATE PLAYERS SET Points = 2 WHERE PlayerName = billy
= SQL: UPDATE PLAYERS SET Points = 2 WHERE PlayerName = bob
*/

void Database::UpdatePlayer(const std::string& table, const std::string& where_col, const std::vector<std::string>& where_list, const std::string& set_col, const std::string& set_val) {
    int isValid = 1;
    for(std::string it : where_list) {
        if (!validInput(it)) {
            isValid--;
        }
    }
    if (validInput(table) && validInput(where_col) && isValid && validInput(set_val)) {
        int i = 0;
        char* errorMsg;
        for(std::string n : where_list) {
            std::string updateStatement = "UPDATE " + table + " SET " + set_col + " = " + set_val + " WHERE " + where_col + " = " + n + ";";
            i++;
            int updateStatus = sqlite3_exec(db, updateStatement.c_str(), callback, NULL, &errorMsg);
            /*
            //error check
            if (updateStatus != SQLITE_OK){
                std::cout << "update unsucessful: " << errorMsg << std::endl;
            }
            */
        }
    }
}

/*
accepts pointer to database (db), vector to store SELECT return values (data), vector of columns and respective values to populate SQL WHERE (where_col, where_list), and vector 
of columns (select_list) whose values are to return to the caller (pushed into data)
dbUpdatePlayer(DB, "PLAYERS", {"billy","bob"}, Points, "2", {"PlayerName","PlayerID"})
= data Vector populated with: {"billy","1"} 
*/

void Database::SelectValues(const std::string& table, std::vector<std::string>& data, const std::string& where_col, const std::string& where_val, const std::vector<std::string>& select_list) {
    char* errormsg;
    std::string selectCols;
    //take list of columns to select and concatenate them for SQL SELECT statement
    for(auto n : select_list) {
        selectCols += n + ", ";
    }
    selectCols.pop_back(); //remove last " " from string
    selectCols.pop_back(); //remove last "," from string
    //std::cout << "dbSelectValues: selectCols = " << selectCols << std::endl;
    int isValid = 1;
    for(std::string it : select_list) {
        if (!validInput(it)) {
            isValid--;
        }
    }
    if (validInput(table) && validInput(where_col) && validInput(where_val) && isValid) {
        std::string selectStatement = "SELECT " + selectCols + " FROM " + table + " WHERE " + where_col + " = " + where_val + ";";
        int selectStatus = sqlite3_exec(db, selectStatement.c_str(), callback, static_cast<void*>(&data), &errormsg);
        /*
        //error check
        if (selectStatus != SQLITE_OK){
            std::cout << "Select could not be performed: " <<sqlite3_errmsg(db) << std::endl;
        }
        */
    }
}