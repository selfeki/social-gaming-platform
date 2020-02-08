#include <iostream>
#include <sqlite3.h>
#include <vector>
#include "database.h"

///////////////////////////////////////////////////////////////////////////////
//COMPILE TESTSUITE WITH: //g++ testSuite.cpp database.cpp -l sqlite3 -o test//
//MAKE SURE TO INCLUDE database.h IN SOURCE FILE                             //
///////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv){
    char* errormsg;
    Database DB;
    DB.OpenDB();
    std::string select = "SELECT * FROM PLAYERS";

    //CREATE TABLE TEST
    std::string table = "PLAYERS";
    DB.CreateTable(table);


    //INSERT TEST
    std::string playerName1 = "billy";
    std::string playerName2 = "joe";
    std::string userId1 = "1";
    std::string userId2 = "2";
    DB.InsertPlayer(table, playerName1, userId1);
    DB.InsertPlayer(table, playerName2, userId2);
    std::cout << "After Insert\n";
    int selectStatus = sqlite3_exec(DB.dbPointer(), select.c_str(), callback, NULL, &errormsg);
    if (selectStatus != SQLITE_OK){
        std::cout << "Select could not be performed: " <<sqlite3_errmsg(DB.dbPointer()) << std::endl;
    }

    //UPDATE TEST
    std::string UserId = "UserId";
    std::vector<std::string> userIDsList = {"1","2"}; //"billy" and "bobby"
    std::string Points = "Points";
    std::string points_val = "2";
    DB.UpdatePlayer(table, UserId, userIDsList, Points, points_val);
    std::cout << "After Update\n";
    selectStatus = sqlite3_exec(DB.dbPointer(), select.c_str(), callback, NULL, &errormsg);
    if (selectStatus != SQLITE_OK){
        std::cout << "Select could not be performed: " <<sqlite3_errmsg(DB.dbPointer()) << std::endl;
    }

    //DELETE TEST
    DB.DeletePlayer(table, userId1);
    std::cout << "After Delete\n";
    selectStatus = sqlite3_exec(DB.dbPointer(), select.c_str(), callback, NULL, &errormsg);
    if (selectStatus != SQLITE_OK){
        std::cout << "Select could not be performed: " <<sqlite3_errmsg(DB.dbPointer()) << std::endl;
    }

    //SELECT TEST
    std::vector<std::string> cols_list = {"UserId","UserName"};
    std::vector<std::string> selectResults = {};
    std::cout << "Select Test" << std::endl;
    DB.SelectValues(table, selectResults, Points, userId2, cols_list); //SELECT UserID and UserName WHERE Points = 2
    //SELECT call return values stored in dataTest
    for(auto i : selectResults) {
        std::cout << i << std::endl;
    }

    //INVALID INSERT TEST
    std::string badString1 = "DROP TABLE LOL;";
    std::string badString2 = "DROP TABLE, LOL";
    std::string badUserId1 = "3";
    std::string badUserId2 = "4";
    DB.InsertPlayer(table, badString1, badUserId1);
    DB.InsertPlayer(table, badString2, badUserId2);
    std::cout << "\nAfter Bad Insert\n";
    selectStatus = sqlite3_exec(DB.dbPointer(), select.c_str(), callback, NULL, &errormsg);
    if (selectStatus != SQLITE_OK){
        std::cout << "Select could not be performed: " <<sqlite3_errmsg(DB.dbPointer()) << std::endl;
    }

    DB.CloseDB();
    return (0);
}