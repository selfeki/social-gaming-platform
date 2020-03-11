#pragma once

#include <functional>
#include <optional>
#include <vector>

namespace arepa {

/**
 * Searches for something in a map, returning an optional based on whether or not it was found.
 * This is a useful replacement.
 *
 * @param map The map.
 * @param key The search key.
 * @return An optional of what was found.
 */
template <typename _Map>
constexpr std::optional<typename _Map::value_type::second_type> find_in_map(const _Map& map, const typename _Map::key_type& key) {
    auto find = map.find(key);
    if (find == map.end()) {
        return std::nullopt;
    } else {
        return { find->second };
    }
}

/**
 * Searches for something in a vector, returning an optional based on whether or not it was found.
 * This is a useful replacement for std::find_if.
 *
 * @tparam T The type.
 * @param vec The vector.
 * @param lambda The search lambda.
 * @return An optional of what was found.
 */
template <typename T, typename _Predicate>
constexpr std::optional<T> find_if(const std::vector<T>& vec, _Predicate lambda) {
    auto find = std::find_if(vec.begin(), vec.end(), lambda);
    if (find == vec.end()) {
        return std::nullopt;
    } else {
        return { *find };
    }
}


}