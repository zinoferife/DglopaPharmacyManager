#pragma once
#include "Views.h"
#include "Tables.h"
#include "Instances.h"
#include <spdlog/spdlog.h>


class ProductsVisitor : public nl::base_visitor,
	public nl::visitor<nl::detail::remove_duplicate_t<typename Products::tuple_t>>
{
public:
	wxListCtrl* list;
	std::uint64_t id;

	virtual void Visit(std::string& value, size_t col)
	{
		VisitFunction(value, col);
	}

	virtual void Visit(std::uint64_t& value, size_t col)
	{
		if (col == Products::category_id) return;
		VisitFunction(value, col);
	}

	virtual void Visit(std::uint32_t& value, size_t col)
	{
		VisitFunction(value, col);
	}

	template<typename T>
	void VisitFunction(const T& t, size_t col)
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


template<typename T>
class ViewHandler<Products, T> : public T
{
public:
	static constexpr size_t ColumnCount = Products::column_count;
	virtual ~ViewHandler() {}

	ViewHandler()
	{
		//default attributes
		mAttrs[1].SetBackgroundColour(*wxRED);
	}

	virtual void CreateList(Products& data, wxListCtrl* control)
	{
		std::shared_ptr<spdlog::logger> mLog = spdlog::get("log");
		if (control)
		{
			control->ClearAll();
			for (size_t i = 0; i < Products::column_count - 1; i++)
			{
				wxListItem itemCol;
				itemCol.SetText(data.get_name(i));
				itemCol.SetAlign(wxLIST_FORMAT_CENTER);
				control->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
				control->InsertColumn(i, itemCol);
			}
			control->SetItemCount(data.size());
		}
		else
		{
			mLog->error("PRODUCT_VIEW: invalid control passed in");
		}
	}

	virtual void ShowList(Products& data, wxListCtrl* control)
	{
		if (control)
		{
			
		}
	}

	virtual void OnClicked(Products& data, wxListCtrl* control, wxListEvent& evt)
	{
		std::shared_ptr<spdlog::logger> log = spdlog::get("log");
		if (control)
		{
			const wxListItem& item = evt.GetItem();
			control->SetItemBackgroundColour(item.GetId(), *wxRED);
			log->info("{}, was clicked", item.GetId());
		}
	}

	virtual void OnRightClicked(Products& data, wxListCtrl* control, wxListEvent& evt){
		std::shared_ptr<spdlog::logger> log = spdlog::get("log");
		if (control)
		{
			const wxListItem& item = evt.GetItem();
			if (item.GetId() != -1)
			{
				control->SetItemBackgroundColour(item.GetId(), *wxBLUE);
				log->info("{}, was right clicked", item.GetId());
			}
		}
	}
	virtual void OnItemSelected(Products& data, wxListCtrl* control, wxListEvent& evt){
		std::shared_ptr<spdlog::logger> log = spdlog::get("log");
		if (control)
		{
			long index = evt.m_itemIndex;
			if (index != -1)
			{
				mSelected = index;
				control->RefreshItem(index);
				log->info("{}, was selected", index);
			}
		}
	}

	virtual void OnItemActivated(Products& data, wxListCtrl* control, wxListEvent& evt){
		std::shared_ptr<spdlog::logger> log = spdlog::get("log");
		if (control)
		{
			long index = evt.m_itemIndex;
			if (index == -1)
			{
				log->info("{}, was Activated", index);
				return;
			}
			mSelected = index;
			control->RefreshItem(index);
			log->info("{}, was Activated", index);
		}
	}

	virtual wxString OnGetItemText(Products& data, long item, long column) const
	{
		switch (column)
		{
		case Products::id:
			return fmt::format("{:d}", ProductInstance::instance().get<Products::id>(item));
		case Products::name:
			return ProductInstance::instance().get<Products::name>(item);
		case Products::package_size:
			return fmt::format("{:d}", ProductInstance::instance().get<Products::package_size>(item));
		case  Products::stock_count:
			return fmt::format("{:d}", ProductInstance::instance().get<Products::stock_count>(item));
		case Products::unit_price:
			return ProductInstance::instance().get<Products::unit_price>(item);
		default:
			break;
		}
	}
	virtual int OnGetItemImage(Products& data, long item) const 
	{
		return -1;
	}

	virtual int OnGetItemColumnImage(Products& data, long item, long column) const 
	{
		return -1;
	}

	virtual wxItemAttr* OnGetItemColumnAttr(Products& data, long item, long column) const 
	{
		return ((wxItemAttr*)&mAttrs[0]);
	}

	virtual wxListItemAttr* OnGetItemAttr(Products& data, long item) const {
		//get item attributes here, test the state and send attributes bassed on that
		//seleteced attribute
		if (mSelected == item)
		{
			return ((wxListItemAttr*)& mAttrs[1]);
		}
		return ((wxListItemAttr*)& mAttrs[0]);
	}

	virtual bool OnGetItemIsChecked(Products& data, long item) const {
		return false;
	}


	std::array<int, Products::column_count> ImageList;
private:
	//should be false, true is for test
	bool IsInit{ true };
	long mSelected{-1};
	std::unordered_map<Categories::elem_t<Categories::id>, typename Products::relation_t> ByCategories{};
	std::array<wxListItemAttr, 4> mAttrs{};
};