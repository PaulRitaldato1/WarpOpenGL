#pragma once
#include <memory>
#include <vector>
#include <array>
#include <unordered_map>
#include <tuple>
#include <string>
#include <map>
#include <stack>

//these are all just typedefs for if I want to replace these with my own implementations later on

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using uint = unsigned int;

using string = std::string;

template <typename T, typename J>
using Tuple = std::tuple<T, J>;

template <typename T, size_t J>
using Array = std::array<T, J>;

template <typename T>
using Vector = std::vector<T>;

template <typename T>
using Ref = std::shared_ptr<T>;

template <typename T>
using URef = std::unique_ptr<T>;

template <typename T, typename J>
using HashMap = std::unordered_map<T, J>;

template <typename T, typename J>
using Map = std::map<T, J>;

template<typename T>
using Stack = std::stack<T>;