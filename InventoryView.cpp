#include "common.h"
#include "InventoryView.h"

InventoryView::InventoryView(std::uint64_t ProductID, wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize size)
	: wxListCtrl(parent, id, position, size, wxLC_REPORT | wxLC_VIRTUAL | wxLC_HRULES | wxNO_BORDER), mProductId(ProductID), mSortColumn{-1}{

	mDatabaseMgr = std::make_unique<DatabaseManager<Inventories>>(mProductInventoryData, DatabaseInstance::instance());
	


	mDatabaseMgr->CreateTable();
	LoadInventory(nl::clock::now());

	mProductInventoryData.quick_sort<Inventories::date_issued, nl::order_dec<Inventories::elem_t<Inventories::date_issued>>>();
	CreateInventoryView();
	EnableAlternateRowColours();
	SetupImages();
	CreateAttributes();
	SetItemCount(mProductInventoryData.size());
	mSortColOrder.reset();

}

InventoryView::~InventoryView()
{
}

void InventoryView::OnColumnHeaderClick(wxListEvent& evt)
{
	int col = evt.GetColumn();
	if (col > GetColumnCount()) return;
	//skip the first two columns
	const int column = col + 2;
	mSortColumn = col;
	//toogle order for that column
	mSortColOrder[col] = !mSortColOrder[col];
	if (mSortColOrder[col])
	{
		mProductInventoryData.quick_sort(column);
	}
	else{
		mProductInventoryData.quick_sort<false>(column);
	}
}

void InventoryView::OnRightClick(wxListEvent& evt)
{
	//create a context menu
}

void InventoryView::OnItemActivated(wxListEvent& evt)
{

}

void InventoryView::OnAddInventory(wxCommandEvent& evt)
{
	//create a dialog
	//add to the main table, then call notify on the table
	Inventories::row_t new_row;
	while (true)
	{
		InventoryDialog dialog(new_row, this);
		wxWindowID RetCode = dialog.ShowModal();
		if (RetCode == wxID_OK) {
			nl::row_value<Inventories::id>(new_row) = GenRandomId(); //find a way to generate this 
			nl::row_value<Inventories::product_id>(new_row) = mProductId;
			nl::row_value<Inventories::date_issued>(new_row) = nl::clock::now();
			nl::row_value<Inventories::quantity_out>(new_row) = 0;
			CalculateBalance(new_row);
			nl::row_value<Inventories::user_issued>(new_row) = 200345;
			nl::row_value<Inventories::user_checked>(new_row) = 200345;

			StoreInventory(new_row);
			AddInOrder(new_row);

			break;
		}
		else if(RetCode == wxID_CANCEL){
			//canceled
			break;
		}
		else{
			//improper entry
			if(wxMessageBox("Improper entry, try again? ", "Inventory entry", wxICON_ERROR | wxYES_NO) == wxYES) continue;
			break;

		}
	}
}

void InventoryView::OnRemoveInventory(wxCommandEvent& evt)
{
}

void InventoryView::CreateInventoryView()
{
	for (size_t i = 2; i < InventoryInstance::instance().get_column_count(); i++)
	{
		AppendColumn(InventoryInstance::instance().get_name(i));
	}
}

void InventoryView::CalculateBalance(Inventories::row_t& row)
{
	auto [min, max] = mProductInventoryData.min_max_on<Inventories::date_issued>();
	if (max == mProductInventoryData.end()){
		nl::row_value<Inventories::balance>(row) = nl::row_value<Inventories::quantity_in>(row);
		return;
	}
	nl::row_value<Inventories::balance>(row) = nl::row_value<Inventories::balance>(*max) + nl::row_value<Inventories::quantity_in>(row);
}



void InventoryView::AddInOrder(const Inventories::row_t& row)
{
	if (nl::row_value<Inventories::balance>(row) > 0)
		AddImage(nl::row_value<Inventories::id>(row), 1);
	else 
		AddImage(nl::row_value<Inventories::id>(row), 0);

	constexpr size_t date_issued = Inventories::date_issued - 2;

	AddAttribute(nl::row_value<Inventories::id>(row), mJustAdded);
	mProductInventoryData.add_in_order<Inventories::date_issued>(row);
	mSortColOrder.set(date_issued, false);
	
	
	Freeze();
	SetItemCount(mProductInventoryData.size());
	Thaw();
	Refresh();
}

void InventoryView::CreateAttributes()
{
	mJustAdded = std::make_shared<wxListItemAttr>();
	mJustAdded->SetBackgroundColour(wxColour(0, 255, 127));
}

void InventoryView::CreateLoadAllQuery()
{
	nl::query q;
	q.select("*").from(Inventories::table_name).where(fmt::format("{} = \'{:d}\'",
		Inventories::get_col_name<Inventories::product_id>(), mProductId));
	spdlog::get("log")->info("{}", q.get_query());
	mDatabaseMgr->AddQuery("loadall", q);

	mDatabaseMgr->ChangeQuery("load", q);
}


void InventoryView::AddAttribute(std::vector<std::uint64_t>&& ids, std::shared_ptr<wxListItemAttr> attr)
{
	for(auto&& id : ids)
	{
		auto [i, inserted ] = mAttributesTable.insert({ id, attr });
		if (!inserted){
			//assuming that it is change of attribute
			i->second = attr;
		}
	}
}

void InventoryView::AddAttribute(std::uint64_t id, std::shared_ptr<wxListItemAttr> attr)
{
	auto [i, inserted] = mAttributesTable.insert({ id, attr });
	if (!inserted){
		i->second = attr;
	}
}

void InventoryView::RemoveAttribute(std::vector<std::uint64_t>&& ids)
{
	for (auto&& id : ids)
	{
		mAttributesTable.erase(id);
	}
}

void InventoryView::RemoveAttribute(std::uint64_t id)
{
	mAttributesTable.erase(id);
}

void InventoryView::AddImage(std::vector<std::uint64_t>&& ids, int imageId)
{
	for (auto&& id : ids){
		auto [i, inserted] = mImageTable.insert({ id, imageId });
		if (!inserted){
			i->second = imageId;
		}
	}
}

void InventoryView::AddImage(std::uint64_t id, int imageId)
{
	auto [i, inserted] = mImageTable.insert({ id, imageId });
	if (!inserted) {
		i->second = imageId;
	}
}

void InventoryView::RemoveImage(std::vector<std::uint64_t>&& ids)
{
	for (auto&& id : ids)
	{
		mImageTable.erase(id);
	}
}

void InventoryView::RemoveImage(std::uint64_t id)
{
	mImageTable.erase(id);
}

void InventoryView::Check(std::uint64_t id)
{
	mCheckedTable.insert(id);
}

void InventoryView::UnCheck(std::uint64_t id)
{
	mCheckedTable.erase(id);
}

void InventoryView::ResetState()
{
	mAttributesTable.clear();
	mImageTable.clear();
	mCheckedTable.clear();
	mSortColOrder.reset();
}

void InventoryView::LoadInventory(const nl::date_time_t& date)
{
	nl::query loadQ;
	//find the time bounded by {00:00am - 00:00am}
	auto midnight = date - nl::time_since_midnight();
	auto next_midnight = date + nl::time_to_midnight();

	auto time_since_mid = nl::to_representation(midnight);
	auto next_time_since_mid = nl::to_representation(next_midnight);

	//loadQ.select("*").from(Inventories::table_name).where(fmt::format("{} = \'{:d}\'",
	//	Inventories::get_col_name<Inventories::product_id>(), mProductId)).and_(fmt::format("{} > \'{:d}\' AND {} < \'{:d}\'",
	//	Inventories::get_col_name<Inventories::date_issued>(),
	//	time_since_mid,
	//	Inventories::get_col_name<Inventories::date_issued>(),
	//	next_time_since_mid));


	loadQ.select("*").from(Inventories::table_name).where(fmt::format("{} = \'{:d}\'",
		 Inventories::get_col_name<Inventories::product_id>(), mProductId));

	//if using a server, send query to server to get the data from the server
	

	auto string = loadQ.get_query();
	spdlog::get("log")->info("{}", string);


	bool stats = mDatabaseMgr->AddQuery("load", loadQ);
	if (!stats) {
		std::string error = DatabaseInstance::instance().get_error_msg();
		spdlog::get("log")->error("{}", error);
		mDatabaseMgr->ChangeQuery("load", loadQ);
	}
	mDatabaseMgr->LoadTable();
	SetItemCount(mProductInventoryData.size());

}



void InventoryView::StoreInventory(const Inventories::row_t& row)
{
	//write to the database
	mDatabaseMgr->InsertTable(row);
}

void InventoryView::LoadAllInventory()
{
	CreateLoadAllQuery();
	mDatabaseMgr->LoadTable();
	SetItemCount(mProductInventoryData.size());

}

wxItemAttr* InventoryView::OnGetItemAttr(long item) const
{
	if (item < 0 || item > mProductInventoryData.size()) return nullptr;
	auto& tuple = mProductInventoryData[item];
	auto iter = mAttributesTable.find(nl::row_value<Inventories::id>(tuple));
	if (iter == mAttributesTable.end()) return nullptr;
	return &(*(iter->second));
}

int InventoryView::OnGetItemImage(long item) const
{
	if (item > mProductInventoryData.size()) return -1;
	auto& row = mProductInventoryData[item];
	auto iter = mImageTable.find(nl::row_value<Inventories::id>(row));
	if (iter != mImageTable.end()) return iter->second;
	return -1;
}

wxString InventoryView::OnGetItemText(long item, long column) const
{
	if (item < mProductInventoryData.size() && column < mProductInventoryData.get_column_count())
	{
		// the list column starts from 0 and the data column needs to  display from the second column
		long col = column + 2; //skip the frist two columns 
		return mProductInventoryData.get_as_string(item, col);
	}
	return wxString();
}

bool InventoryView::OnGetItemIsChecked(long item) const
{
	if (item > mProductInventoryData.size()) return false;
	auto& row = mProductInventoryData[item];
	return (mCheckedTable.find(nl::row_value<Inventories::id>(row)) == mCheckedTable.end());
}

void InventoryView::SetupImages()
{
	auto list = mProductInventoryData.where([&](const Inventories::row_t& b) {
		return  nl::row_value<Inventories::balance>(b) == 0;
	}).isolate_column<Inventories::id>();
	AddImage(std::move(list), 0);
	
	list = mProductInventoryData.where([&](const Inventories::row_t& b) {
		return nl::row_value<Inventories::balance>(b) > 0;
	}).isolate_column<Inventories::id>();
	AddImage(std::move(list), 1);
}

void InventoryView::ResetProductInventoryList(std::uint64_t productID)
{
	//clear states
	ResetState();

	mProductId = productID;
	LoadInventory(nl::clock::now());

	mProductInventoryData.quick_sort<Inventories::date_issued, nl::order_dec<Inventories::elem_t<Inventories::date_issued>>>();
	SetupImages();
	SetItemCount(mProductInventoryData.size());
}
