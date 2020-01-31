#ifndef PLAYER_DATABASE_H
#define PLAYER_DATABASE_H
#include <iostream>
#include <vector>
#include <sqlite3.h>

class PlayerDatabase {
    private:
    sqlite3* playerDB;

    public:
        PlayerDatabase();

        void dbInsertPlayer(std::string playerName, int id);
        void dbDeletePlayer(int id);
        void dbUpdatePlayer(std::string where_col, std::vector<std::string> where_list, std::string set_col, std::vector<std::string> val_list);
        void dbSelectValues(std::string where_col, std::vector<std::string> select_list);

};

#endif
