#pragma once

#include "UserNickname.hpp"

#include <random>
#include <vector>

namespace arepa::chat {

/**
 * A class that generates human-friendly player nicknames.
 */
class UserNicknameGenerator {
#pragma mark - Constants -
public:
    static UserNicknameGenerator DEFAULT;


#pragma mark - Fields -
private:
    std::default_random_engine _rand;


#pragma mark - Fields (Public) -
public:
    std::vector<UserNickname> name_pool;
    std::vector<UserNickname> adjective_pool;


#pragma mark - Constructors -
public:
    /**
     * Creates a new, empty nickname generator.
     */
    explicit UserNicknameGenerator();

    /**
     * Creates a nickname generator.
     * 
     * @param name_pool The name pool.
     * @param adjective_pool The adjective pool.
     */
    explicit UserNicknameGenerator(std::vector<UserNickname> name_pool, std::vector<UserNickname> adjective_pool);


#pragma mark - Operators -
public:
    /**
     * Generates a new, likely-to-be-unique random nickname.
     * @return The new nickname.
     */
    [[nodiscard]] UserNickname operator()();
};

}
