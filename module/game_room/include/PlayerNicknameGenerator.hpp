#pragma once

#include "PlayerNickname.hpp"

#include <random>
#include <vector>

namespace arepa::game::room {

/**
 * A class that generates human-friendly player nicknames.
 */
class PlayerNicknameGenerator {
#pragma mark - Constants -
public:
    static PlayerNicknameGenerator DEFAULT;


#pragma mark - Fields -
private:
    std::default_random_engine _rand;


#pragma mark - Fields (Public) -
public:
    std::vector<PlayerNickname> name_pool;
    std::vector<PlayerNickname> adjective_pool;


#pragma mark - Constructors -
public:
    /**
     * Creates a new, empty nickname generator.
     */
    explicit PlayerNicknameGenerator();

    /**
     * Creates a nickname generator.
     * 
     * @param name_pool The name pool.
     * @param adjective_pool The adjective pool.
     */
    explicit PlayerNicknameGenerator(std::vector<PlayerNickname> name_pool, std::vector<PlayerNickname> adjective_pool);


#pragma mark - Operators -
public:
    /**
     * Generates a new, likely-to-be-unique random nickname.
     * @return The new nickname.
     */
    [[nodiscard]] PlayerNickname operator()();
};

}
