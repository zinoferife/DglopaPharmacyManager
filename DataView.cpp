#include "common.h"
#include "DataView.h"
BEGIN_EVENT_TABLE(DataView, wxListCtrl)

END_EVENT_TABLE()



DataView::DataView(const std::string& database, wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize& size, long style)
:	wxListCtrl(parent, id, position, size, style),
	connection(DatabaseInstance::instance()), mCurrentView(Views::Product_v){
	if (!connection.is_open())
	{
		mLogger->error("{}", connection.get_error_msg());
	}
}

DataView::~DataView()
{
}

void DataView::OnProductView(ViewEvent& event)
{
}

void DataView::OnSalesView(ViewEvent& event)
{
}

void DataView::OnInventoryView(ViewEvent& event)
{
}

void DataView::OnUsersView(ViewEvent& event)
{
}

void DataView::OnCustomersView(ViewEvent& event)
{
}

void DataView::OnCategoryChange(ViewEvent& event)
{
}

void DataView::OnCacheHint(wxListEvent& event)
{
}

void DataView::OnItemSelected(wxListEvent& event)
{
}

void DataView::OnColumnClick(wxListEvent& event)
{
}

void DataView::OnColumnRightClick(wxListEvent& evnet)
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

void DataView::SetLogger(std::shared_ptr<spdlog::logger> Logger)
{
}

void DataView::ResetList()
{
}

void DataView::DoSelectView()
{
}
