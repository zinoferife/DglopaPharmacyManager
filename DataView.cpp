#include "common.h"
#include "DataView.h"
BEGIN_EVENT_TABLE(DataView, wxListCtrl)

END_EVENT_TABLE()



DataView::DataView( wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize& size, long style)
	:wxListCtrl(parent, id, position, size, style), mCurrentView{ Views::Product_v }, i(100){
	std::shared_ptr<spdlog::logger> log = spdlog::get("log");
	if (!DatabaseInstance::instance().is_open()) {
		log->error("Database is not open, try connecting to a database");
	}
	log->info("Selecting Product view");
	DoSelectView();
}

DataView::~DataView()
{
}

void DataView::OnCacheHint(wxListEvent& event)
{
}

void DataView::OnItemSelected(wxListEvent& event)
{
	switch (mCurrentView)
	{
	case Views::Product_v:
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
	case Views::Product_v:
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
	case Views::Product_v:
		nl::detail::field<Products>(*this).OnRightClicked(ProductInstance::instance(), this, evt);
		break;
	default:
		break;
	}
}

wxString DataView::OnGetItemText(long item, long column) const
{
	switch (mCurrentView)
	{
	case Views::Product_v:
		return nl::detail::field<Products>(*this).OnGetItemText(ProductInstance::instance(), item, column);
	default:
		return wxString();
	}
}

int DataView::OnGetItemImage(long item) const
{
	switch (mCurrentView)
	{
	case Views::Product_v:
		return nl::detail::field<Products>(*this).OnGetItemImage(ProductInstance::instance(), item);
	default:
		return -1;
	}
}

int DataView::OnGetItemColumnImage(long item, long column) const
{
	switch (mCurrentView)
	{
	case Views::Product_v:
		return nl::detail::field<Products>(*this).OnGetItemColumnImage(ProductInstance::instance(), item, column);
	default:
		return -1;
	}
}

wxItemAttr* DataView::OnGetItemColumnAttr(long item, long column) const
{
	switch (mCurrentView)
	{
	case Views::Product_v:
		return nl::detail::field<Products>(*this).OnGetItemColumnAttr(ProductInstance::instance(), item, column);
	default:
		return ((wxItemAttr*)&mDefaultAttr);
	}
}

wxItemAttr* DataView::OnGetItemAttr(long item) const
{
	switch (mCurrentView)
	{
	case Views::Product_v:
		return nl::detail::field<Products>(*this).OnGetItemAttr(ProductInstance::instance(), item);
	default:
		return ((wxItemAttr*)& mDefaultAttr);
	}
}

bool DataView::OnGetItemIsChecked(long item) const
{
	return false;
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
	switch (mCurrentView)
	{
	case Views::Product_v:
		log->info("Creating Product view");
		nl::detail::field<Products>(*this).CreateList(ProductInstance::instance(), this);
		break;
	default:
		break;
	}
}
