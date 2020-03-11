#pragma once

#include <functional>
#include <vector>
#include <optional>

namespace arepa {

/**
 * Searches for something in a map, returning an optional based on whether or not it was found.
 * This is a useful replacement.
 *
 * @tparam T The type.
 * @param vec The vector.
 * @param lambda The search lambda.
 * @return An optional of what was found.
 */
template <typename V, typename K, typename _Map>
constexpr std::optional<V> find_in_map(const _Map& map, const K& key) {
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
template <typename T, typename Predicate>
constexpr std::optional<T> find_if(const std::vector<T>& vec, Predicate lambda) {
    auto find = std::find_if(vec.begin(), vec.end(), lambda);
    if (find == vec.end()) {
        return std::nullopt;
    } else {
        return { *find };
    }
}


}