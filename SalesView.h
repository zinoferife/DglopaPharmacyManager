#pragma once
#include "Views.h"
#include "Tables.h"
#include "Instances.h"
#include <spdlog/spdlog.h>

template<typename base>
class ViewHandler<Sales, base> : public base
{
public:
	ViewHandler()
	{
		//join on create, The tables should be already be loaded
		//if not a loaded notification from the tables should call this functions 
		JoinCustomersTableToSales();
		JoinProductTableToSales();
	}

	virtual ~ViewHandler() {}
	virtual void CreateList(Sales& data, wxListCtrl* control) {
		control->ClearAll();
		for (size_t i = 0; i < Sales::column_count - 1; i++)
		{
			wxListItem itemCol;
			itemCol.SetText(data.get_name(i));
			itemCol.SetAlign(wxLIST_FORMAT_CENTER);
			control->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
			control->InsertColumn(i, itemCol);
		}
		control->SetItemCount(data.size());
	}

	//TODO: REMOVE THIS, UNNESSARY
	virtual void ShowList(Sales& data, wxListCtrl* control) {
		 
	}

	//events handling, forwared from the list view
public:
	virtual void OnClicked(Sales& data, wxListCtrl* control, wxListEvent& evt) {
		std::shared_ptr<spdlog::logger> log = spdlog::get("log");
		if (control)
		{
			const wxListItem& item = evt.GetItem();
			control->SetItemBackgroundColour(item.GetId(), *wxRED);
			log->info("{}, was clicked", item.GetId());
		}
	}
	virtual void OnRightClicked(Sales& data, wxListCtrl* control, wxListEvent& evt) {
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
	virtual void OnItemSelected(Sales& data, wxListCtrl* control, wxListEvent& evt) {
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
	virtual void OnItemActivated(Sales& data, wxListCtrl* control, wxListEvent& evt) {
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
	virtual wxString OnGetItemText(Sales& data, long item, long column) const {
		switch (column)
		{
			case Sales::id:
				return fmt::format("{:d}", data.get<Sales::id>(item));
			case Sales::product_id:
			{
				if (mSalesProductTable.empty()) return std::string("No Product");
				return mSalesProductTable.get<1>(item);
			}
			case Sales::customer_id:
			{
				if (mSalesCustomersTable.empty()) return std::string("No customer");
				return mSalesCustomersTable.get<1>(item);
			}
			case Sales::amount:
				return fmt::format("{:d}", data.get<Sales::amount>(item));
			case Sales::date:
				return nl::to_string_date(data.get<Sales::date>(item));
			case Sales::user_id:
				return fmt::format("{:d}", data.get<Sales::user_id>(item));
			default:
				return std::string();
		}
	}
	virtual int OnGetItemImage(Sales& data, long item) const {
		return -1;
	}
	virtual int OnGetItemColumnImage(Sales& data, long item, long column) const {
		return -1;
	}
	
	virtual wxListItemAttr* OnGetItemAttr(Sales& data, long item) const {
		return nullptr;
	}
	virtual bool OnGetItemIsChecked(Sales& data, long item) const {
		return nullptr;
	}

	/*
	virtual void OnItemDeleted(T& data, wxListCtrl* control, wxListEvent& evt){}

	virtual void OnItemInserted(T& data, wxListCtrl* control, wxListEvent& evt){}

	virtual void OnKeyDown(T& data, wxListCtrl* control, wxListEvent& evt){}

	virtual void OnColumnClicked(T& data, wxListCtrl* control, wxListEvent& evt){}

	virtual void onColumnRightClicked(T& data, wxListCtrl* control, wxListEvent& evt){}

	virtual void OnCacheHint(T& data, wxListCtrl* control, wxListEvent& evt){}

	virtual void OnChecked(T& data, wxListCtrl* control, wxListEvent& evt){}

	virtual void OnUnchecked(T& data, wxListCtrl* control, wxListEvent& evt){}
	*/
	protected:
		void JoinProductTableToSales()
		{
			if (!ProductInstance::instance().empty())
			{
				//This says: From sale table select the sales_product_id, to that relation, join a relation from a subquery (the subquery is: from product table select Product_id and Product_name), 
				//join on Sales_product_id and Product_id, from that join, select the Sales_product_id and Product_name
				mSalesProductTable = std::move(SalesInstance::instance() //from sales instance
					.select<Sales::product_id>() // select sales_product_id
					.join_on<0, 0>(ProductInstance::instance() // join on Sales_product_id and Products_id which are 0,0 in the two new relations
						.select<Products::id, Products::name>()) 
					.select<0, 2>()); // select sales_product_id and product_name wich are 0 and 2 from the final relation
			}
		}

		void JoinCustomersTableToSales()
		{
			if (!CustomersInstance::instance().empty())
			{
				//the exact query as above
				mSalesCustomersTable = std::move( SalesInstance::instance().select<Sales::customer_id>()
					.join_on<0,0>(CustomersInstance::instance()
						.select<Customers::id, Customers::name>())
					.select<0,2>());
			}
		}

	private:
		//this might be stupid in the long run but for now, keep a view of the sale product id and product name 
		//this view is updated through listeners, on both the sales table and the product table 
		typedef typename std::tuple<Sales::elem_t<Sales::product_id>, Products::elem_t<Products::name>>  SalesProductType;
		nl::relation<std::vector<SalesProductType>> mSalesProductTable;

		//same as before this a view of the custormers and the sales table that the sale view uses
		//it is updated also by listeners on the customers and sales table 
		typedef typename std::tuple<Sales::elem_t<Sales::customer_id>, Customers::elem_t<Customers::name>> SalesCustomerType;
		nl::relation<std::vector<SalesCustomerType>> mSalesCustomersTable;



};