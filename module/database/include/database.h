#ifndef DATABASE_H
#define DATABASE_H
#include <iostream>
#include <sqlite3.h>
#include <vector>

int callback(void* data, int rows, char** argv, char** azColName); //used by testSuite and SelectValues function call

class Database {
    private:
        sqlite3* db;
        bool validInput(const std::string& userInput);
    public:
        ~Database();
        void OpenDB();
        void CloseDB();
        sqlite3* dbPointer(); //used for debugging in testSuite
        void InsertPlayer(const std::string& table, const std::string& playerName, const std::string& id);
        void CreateTable(const std::string& tableName);
        void DeletePlayer(const std::string& table, const std::string& id);
        void UpdatePlayer(const std::string& table, const std::string& where_col, const std::vector<std::string>& where_list, const std::string& set_col, const std::string& set_val);
        void SelectValues(const std::string& table, std::vector<std::string>& data, const std::string& where_col, const std::string& where_val, const std::vector<std::string>& select_list);
};

#endif