#ifndef XYPHER_COMMON_H
#define XYPHER_COMMON_H

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <variant>
#include <unordered_map>
#include <set>
#include <iostream>
#include <cassert>

namespace xypher {

using String = std::string;
using StringView = std::string_view;

template<typename T>
using Unique = std::unique_ptr<T>;

template<typename T>
using Shared = std::shared_ptr<T>;

template<typename T>
using Vec = std::vector<T>;

template<typename T>
using Optional = std::optional<T>;

template<typename K, typename V>
using Map = std::unordered_map<K, V>;

template<typename T>
using Set = std::set<T>;

template<typename T, typename... Args>
Unique<T> makeUnique(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T, typename... Args>
Shared<T> makeShared(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

}

#endif
