#pragma once

namespace Engine
{
	template<typename Type, uint32 Size>
	using Array = std::array<Type, Size>;

	template<typename Type>
	using Vector = std::vector<Type, STLAllocator<Type>>;

	template<typename Type>
	using List = std::list<Type, STLAllocator<Type>>;

	template<typename Key, typename Type, typename Pred = std::less<Key>>
	using Map = std::map<Key, Type, Pred, STLAllocator<std::pair<const Key, Type>>>;

	template<typename Key, typename Pred = std::less<Key>>
	using Set = std::set<Key, Pred, STLAllocator<Key>>;

	template<typename Type>
	using Deque = std::deque<Type, STLAllocator<Type>>;

	template<typename Type, typename Container = Deque<Type>>
	using Queue = std::queue<Type, Container>;

	template<typename Type, typename Container = Deque<Type>>
	using Stack = std::stack<Type, Container>;

	template<typename Type, typename Container = Vector<Type>, typename Pred = std::less<typename Container::value_type>>
	using PriorityQueue = std::priority_queue<Type, Container, Pred>;

	using String = std::basic_string<char, std::char_traits<char>, STLAllocator<char>>;

	using WString = std::basic_string<wchar_t, std::char_traits<wchar_t>, STLAllocator<wchar_t>>;

	template<typename Key, typename Type, typename Hasher = std::hash<Key>, typename KeyEq = std::equal_to<Key>>
	using HashMap = std::unordered_map<Key, Type, Hasher, KeyEq, STLAllocator<std::pair<const Key, Type>>>;

	template<typename Key, typename Hasher = std::hash<Key>, typename KeyEq = std::equal_to<Key>>
	using HashSet = std::unordered_set<Key, Hasher, KeyEq, STLAllocator<Key>>;
}