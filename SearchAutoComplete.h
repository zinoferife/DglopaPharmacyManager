#pragma once
#include <wx/wx.h>
#include <wx/textcompleter.h>
#include <regex>
#include <fmt/format.h>

#include <type_traits>

template<typename Data, size_t ColumnCompleteOn>
class SearchAutoComplete : public wxTextCompleterSimple
{
public:
	SearchAutoComplete(Data& reference)
	:mDataRef(reference){
	
	}

	virtual void GetCompletions(const wxString& prefix, wxArrayString& completions) override
	{
		if constexpr (std::is_convertible_v<typename Data::template elem_t<ColumnCompleteOn>, std::string>)
		{
			auto DataRelation = mDataRef.like<ColumnCompleteOn>(ConstructRegex(prefix.ToStdString()));
			if (!DataRelation.empty())
			{
				auto column = mDataRef.isolate_column<ColumnCompleteOn>();
				for (auto&& colItem : column)
				{
					completions.push_back(colItem);
				}
			}
		}

	}

	std::regex ConstructRegex(const std::string&& text)
	{
		std::string temp;
		for (auto& c : text)
		{
			if(!std::isalnum(c)) continue;
			temp += fmt::format("[{:c}|{:c}]", (char)std::tolower(c), (char)std::toupper(c));
		}
		temp += "(?:.*)?";
		return std::move(std::regex(std::move(temp)));
	}

protected:
	Data& mDataRef;

};

