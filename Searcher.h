#pragma once
#include <string>
#include <regex>
#include <type_traits>


#include <rel_view.h>

template<size_t SearchColumn, typename Data>
class Searcher
{
public:
	using search_col_t = typename Data::template elem_t<SearchColumn>;
	Searcher(Data& reference)
		:mDataRef(reference)
	{}

	inline auto Search(const std::string& search_for) const
	{
		if constexpr (std::is_convertible_v<search_col_t, std::string>)
		{
			return mDataRef.like_index<SearchColumn>(ConstructRegex(search_for));
		}
		return std::vector<size_t>{};
	}

	std::regex ConstructRegex(const std::string& text) const
	{
		std::string temp;
		for (auto& c : text)
		{
			temp += fmt::format("[{:c}|{:c}]", (char)std::tolower(c), (char)std::toupper(c));
		}
		temp += "(?:.*)?";
		return std::move(std::regex(std::move(temp)));
	}

	inline auto operator()(const std::string& search_for)
	{
		if constexpr (std::is_convertible_v<search_col_t, std::string>)
		{
			return mDataRef.like_index<SearchColumn>(ConstructRegex(search_for));
		}
		return std::vector<size_t>{};
	}

private:
	Data& mDataRef;
};


//specilisation for views
template<size_t SearchColumn, typename... args>
class Searcher<SearchColumn, nl::rel_view<args...>>
{
public:
	typedef nl::rel_view<args...> Data;
	using search_col_t = std::decay_t<typename Data::template elem_t<SearchColumn>>;
	Searcher(Data& reference)
		:mDataRef(reference)
	{}

	inline auto Search(const std::string& search_for) const
	{
		if constexpr (std::is_convertible_v<search_col_t, std::string>)
		{
			return mDataRef.like_index<SearchColumn>(ConstructRegex(search_for));
		}
		return std::vector<size_t>{};
	}

	std::regex ConstructRegex(const std::string& text) const
	{
		std::string temp;
		for (auto& c : text)
		{
			temp += fmt::format("[{:c}|{:c}]", (char)std::tolower(c), (char)std::toupper(c));
		}
		temp += "(?:.*)?";
		return std::move(std::regex(std::move(temp)));
	}

	inline auto operator()(const std::string& search_for)
	{
		if constexpr (std::is_convertible_v<search_col_t, std::string>)
		{
			return mDataRef.like_index<SearchColumn>(ConstructRegex(search_for));
		}
		return std::vector<size_t>{};
	}

private:
	Data& mDataRef;
};