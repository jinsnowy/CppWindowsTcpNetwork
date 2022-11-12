#pragma once

#include <vector>
#include <queue>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <array>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <atomic>

using namespace std;

template<typename Type, size_t size>
using Array = array<Type, size>;

template<typename Type>
using Vector = vector<Type>;

template<typename Type>
using List = list<Type>;

template<typename Key, typename Type, typename Pred = less<Key>>
using Map = map<Key, Type, Pred>;

template<typename Key, typename Pred = less<Key>>
using Set = set<Key, Pred>;

template<typename Type>
using Deque = deque<Type>;

template<typename Type, typename Container = Deque<Type>>
using Queue = queue<Type, Container>;

template<typename Type, typename Container = Deque<Type>>
using Stack = stack<Type, Container>;

template<typename Type, typename Container = Vector<Type>, typename Pred = less<typename Container::value_type>>
using PriorityQueue = priority_queue<Type, Container, Pred>;

using String = basic_string<char, char_traits<char>>;

using WString = basic_string<wchar_t, char_traits<wchar_t>>;

template<typename Key, typename Type>
using HashMap = unordered_map<Key, Type>;

template<typename Key>
using HashSet = unordered_set<Key>;

template<typename T>
using Atomic = atomic<T>;