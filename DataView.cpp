#include "common.h"
#include "DataView.h"
BEGIN_EVENT_TABLE(DataView, wxListCtrl)
END_EVENT_TABLE()



DataView::DataView( wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize& size, long style)
	:wxListCtrl(parent, id, position, size, style), mCurrentView{ Views::Product_view }, i(100){
	std::shared_ptr<spdlog::logger> log = spdlog::get("log");
	if (!DatabaseInstance::instance().is_open()) {
		log->error("Database is not open, try connecting to a database");
	}
	log->info("Selecting Product view");
	DoSelectView();
	log->info("Setting notification pipeline");
	ProductInstance::instance().sink().add_listener<DataView, &DataView::OnProductTableNotification>(this);
	SalesInstance::instance().sink().add_listener<DataView, &DataView::OnSaleTableNotification>(this);
	SetItemCount(ProductInstance::instance().size());
}

DataView::~DataView()
{
	ProductInstance::instance().sink().remove_listener<DataView, &DataView::OnProductTableNotification>(this);
	SalesInstance::instance().sink().remove_listener<DataView, &DataView::OnSaleTableNotification>(this);
}

void DataView::ChangeView(Views view)
{
	if (mCurrentView != view)
	{
		mCurrentView = view;
		DoSelectView();
	}
}

void DataView::OnCacheHint(wxListEvent& event)
{
}

void DataView::OnItemSelected(wxListEvent& event)
{
	switch (mCurrentView)
	{
	case Views::Product_view:
		nl::detail::field<Products>(*this).OnItemSelected(ProductInstance::instance(), this, event);
		break;
	default:
		break;
	}

}

void DataView::OnItemActivated(wxListEvent& evt)
{
	switch (mCurrentView)
	{
	case Views::Product_view:
		nl::detail::field<Products>(*this).OnItemActivated(ProductInstance::instance(), this, evt);
		break;
	default:
		break;
	}

}

void DataView::OnColumnClicked(wxListEvent& event)
{
}

void DataView::OnColumnRightClicked(wxListEvent& evnet)
{
}

void DataView::OnRightClicked(wxListEvent& evt)
{
	switch (mCurrentView)
	{
	case Views::Product_view:
		nl::detail::field<Products>(*this).OnRightClicked(ProductInstance::instance(), this, evt);
		break;
	case Views::Sale_view:
		nl::detail::field<Sales>(*this).OnRightClicked(SalesInstance::instance(), this, evt);
		break;
	default:
		break;
	}
}

wxString DataView::OnGetItemText(long item, long column) const
{
	switch (mCurrentView)
	{
	case Views::Product_view:
		return nl::detail::field<Products>(*this).OnGetItemText(ProductInstance::instance(), item, column);
	case Views::Sale_view:
		return nl::detail::field<Sales>(*this).OnGetItemText(SalesInstance::instance(), item, column);
	default:
		return wxString();
	}
}

int DataView::OnGetItemImage(long item) const
{
	switch (mCurrentView)
	{
	case Views::Product_view:
		return nl::detail::field<Products>(*this).OnGetItemImage(ProductInstance::instance(), item);
	case Views::Sale_view:
		return nl::detail::field<Sales>(*this).OnGetItemImage(SalesInstance::instance(), item);
	default:
		return -1;
	}
}

int DataView::OnGetItemColumnImage(long item, long column) const
{
	switch (mCurrentView)
	{
	case Views::Product_view:
		return nl::detail::field<Products>(*this).OnGetItemColumnImage(ProductInstance::instance(), item, column);
	default:
		return -1;
	}
}

wxListItemAttr* DataView::OnGetItemAttr(long item) const
{
	switch (mCurrentView)
	{
	case Views::Product_view:
		return nl::detail::field<Products>(*this).OnGetItemAttr(ProductInstance::instance(), item);
	default:
		return ((wxListItemAttr*)& mDefaultAttr);
	}
}

bool DataView::OnGetItemIsChecked(long item) const
{
	return false;
}

void DataView::OnProductTableNotification(nl::notifications notif, typename Products::table_t& data, const typename Products::row_t& row)
{
	if (mCurrentView == Views::Product_view)
	{
		switch (notif)
		{
		case nl::notifications::add:
			SetItemCount(data.size());
			Refresh();
		default:
			break;
		}
	}

}

void DataView::OnSaleTableNotification(nl::notifications notif, typename Sales::table_t& data, const typename Sales::row_t& row)
{
	
}

void DataView::SetColumnImage(size_t column, int image)
{
	wxListItem item;
	item.SetMask(wxLIST_MASK_IMAGE);
	item.SetImage(image);
	SetColumn(column, item);
}

void DataView::CreateImageList()
{
}


void DataView::ResetList()
{
}

void DataView::DoSelectView()
{
	std::shared_ptr<spdlog::logger> log = spdlog::get("log");
	Freeze();
	switch (mCurrentView)
	{
	case Views::Product_view:
		log->info("Creating Product view");
		nl::detail::field<Products>(*this).CreateList(ProductInstance::instance(), this);
		break;
	case Views::Sale_view:
		log->info("Creating Sale view");
		nl::detail::field<Sales>(*this).CreateList(SalesInstance::instance(), this);
		break;
	default:
		break;
	}
	Thaw();
}
