#pragma once
#include "common.h"
#include <relation.h>
#include <wx/listctrl.h>
#include <fmt/format.h>
#include <functional>
#include <memory>
#include <visitor.h>
//this is not my best work loool 
//please dont judge me i tried.


template<typename relation_t>
void prepare_list(wxListCtrl* list, const relation_t& rel)
{
	for (size_t i = 0; i < relation_t::column_count; i++)
	{
		wxListItem itemCol;
		itemCol.SetText(rel.get_name(i));
		itemCol.SetAlign(wxLIST_FORMAT_LEFT);
		list->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
		list->InsertColumn(i, itemCol);
	}
}


template<typename relation_t>
class list_control_visitor : public nl::base_visitor, public nl::visitor<nl::detail::remove_duplicate_t<typename relation_t::tuple_t>, void>
{
public:
	wxListCtrl *list;
	std::uint64_t id;
	virtual void visit(std::string& value, size_t col) {visit_function(value, col);}
	virtual void visit(std::uint32_t& value, size_t col) {visit_function(value, col);}

	virtual void visit(std::uint16_t& value, size_t col) {visit_function(value, col);}
	virtual void visit(std::uint64_t& value, size_t col) {visit_function(value, col);}

	virtual void visit(std::int16_t& value, size_t col) {visit_function(value, col);}

	virtual void visit(std::int32_t& value, size_t col) {visit_function(value, col);}
	virtual void visit(std::int64_t& value, size_t col) {visit_function(value, col);}

	virtual void visit(float& value, size_t col) {visit_function(value, col);}
	virtual void visit(double& value, size_t col) {visit_function(value, col);}

	template<typename T>
	void visit_function(const T& t, size_t col)
	{
		if (col == 0) return;
		wxListItem item;
		if constexpr (std::is_convertible_v<T, std::string>) {
			//single string type
			item.SetId(id);
			item.SetMask(wxLIST_MASK_TEXT);
			item.SetText(t);
			item.SetColumn(col);
			list->SetItem(item);
		}
		else {
			if constexpr (fmt::is_formattable<T>::value) {
				item.SetId(id);
				item.SetMask(wxLIST_MASK_TEXT);
				item.SetText(fmt::format("{}", t));
				item.SetColumn(col);
				list->SetItem(item);
			}
		}
	}
};


template<typename relation_t>
void ShowList(relation_t& rel, wxListCtrl* list)
{
	if (list){
		//need to appened a new list
		if (!list->IsEmpty()){
			list->DeleteAllItems();
		}
		list_control_visitor<relation_t> lcv;
		lcv.list = list;
		//function(&lcv);
		size_t i = 0;
		for (auto& row : rel)
		{
			wxListItem item;
			const std::uint64_t id = i++;
			
			item.SetId(id);
			item.SetText(fmt::format("{}", nl::row_value<relation_t::id>(row)));
			list->InsertItem(item);
			lcv.id = id;
			nl::detail::loop<relation_t::column_count - 2>::template accept(lcv, row);
		}
	}
}
