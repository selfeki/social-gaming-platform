#include "UserNicknameGenerator.hpp"

#include <ctime>
#include <sstream>

#define NN(x) UserNickname(x)

using namespace arepa::game;

UserNicknameGenerator UserNicknameGenerator::DEFAULT = UserNicknameGenerator(
    std::vector({ NN("aardwolf"), NN("beaver"), NN("lemming"), NN("fox"), NN("baboon"), NN("dragon"), NN("elephant"), NN("sloth") }),
    std::vector({ NN("super"), NN("iridescent"), NN("bittersweet"), NN("euphoric"), NN("golden"), NN("temporary"), NN("melancholy") }));

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

UserNicknameGenerator::UserNicknameGenerator()
    : _rand(time(nullptr)) {}

UserNicknameGenerator::UserNicknameGenerator(std::vector<UserNickname> name_pool,
                                             std::vector<UserNickname> adjective_pool)
    : _rand(time(nullptr))
    , name_pool(std::move(name_pool))
    , adjective_pool(std::move(adjective_pool)) {}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Operators -
// ---------------------------------------------------------------------------------------------------------------------

UserNickname UserNicknameGenerator::operator()() {
    std::stringstream name;

    // Add the adjective.
    if (!this->adjective_pool.empty()) {
        name << *this->adjective_pool[this->_rand() % (this->name_pool.size())];
        name << "_";
    }

    // Add the name.
    if (!this->name_pool.empty()) {
        name << *this->name_pool[this->_rand() % (this->name_pool.size())];
    }

    // Add the number.
    name << std::to_string(this->_rand() % 99);

    // Return the nickname.
    return UserNickname(name.str());
}