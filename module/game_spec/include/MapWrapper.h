#pragma once

// Todo: replace std::unordered_map with Abseil flat_hash_map
#include <iostream>
#include <unordered_map>

template <class keyTy, class valTy>
using MapWrapper = std::unordered_map<keyTy, valTy>;

// template <class keyTy, class valTy>
// struct MapWrapper {
// public:
//     void
//     set(keyTy key, valTy value) {
//         assignments[key] = value;
//     }

//     valTy
//     get(const keyTy& key) const {
//         auto found = assignments.find(key);
//         if (found != assignments.end()) {
//             return { found->second };
//         } else {
//             std::cerr << "Invalid key: " << key << '\n';
//             throw;
//         }
//     }

// private:
//     std::unordered_map<keyTy, valTy> assignments;
// };