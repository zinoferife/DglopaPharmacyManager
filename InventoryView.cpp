#include "common.h"
#include "InventoryView.h"

InventoryView::InventoryView(std::uint64_t ProductID, wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize size)
	: wxListCtrl(parent, id, position, size, wxLC_REPORT | wxLC_VIRTUAL | wxLC_HRULES | wxNO_BORDER), mProductId(ProductID), mSortColumn{-1}{
	(typename Inventories::relation_t&)mProductInventoryData = InventoryInstance::instance().where<Inventories::product_id>(
		[&](std::uint64_t id) {
			return id == mProductId;
		}
	);
	mProductInventoryData.quick_sort<Inventories::date_issued, nl::order_dec<Inventories::elem_t<Inventories::date_issued>>>();
	CreateInventoryView();
	EnableAlternateRowColours();
	SetupImages();
	CreateAttributes();
	SetItemCount(mProductInventoryData.size());
	mSortColOrder.reset();

	InventoryInstance::instance().sink().add_listener<InventoryView, &InventoryView::OnNotification>(this);
}

InventoryView::~InventoryView()
{
	InventoryInstance::instance().sink().remove_listener<InventoryView, &InventoryView::OnNotification>(this);
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
			nl::row_value<Inventories::id>(new_row) = 10; //find a way to generate this 
			nl::row_value<Inventories::product_id>(new_row) = mProductId;
			nl::row_value<Inventories::date_issued>(new_row) = nl::clock::now();
			nl::row_value<Inventories::quantity_out>(new_row) = 0;
			CalculateBalance(new_row);
			nl::row_value<Inventories::user_issued>(new_row) = 200345;
			nl::row_value<Inventories::user_checked>(new_row) = 200345;

			InventoryInstance::instance().add(new_row);
			InventoryInstance::instance().notify(nl::notifications::add, InventoryInstance::instance().size() - 1);
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

void InventoryView::OnUpdateNotification(nl::notifications notif, const Inventories::table_t& table, size_t col, const size_t& row)
{
}

void InventoryView::OnNotification(nl::notifications notif, const Inventories::table_t& table, const size_t& row)
{
	switch (notif)
	{
	case nl::notifications::add:
		auto& row_ = table[row];
		if (mProductId == nl::row_value<Inventories::product_id>(row_))
		{
			AddInOrder(row_);
		}
	}

}

void InventoryView::AddInOrder(const Inventories::row_t& row)
{
	if (nl::row_value<Inventories::balance>(row) > 0)
		AddImage(nl::row_value<Inventories::id>(row), 1);
	else 
		AddImage(nl::row_value<Inventories::id>(row), 0);

	size_t date_issued = Inventories::date_issued - 2;
	AddAttribute(nl::row_value<Inventories::id>(row), mJustAdded);
	mProductInventoryData.add(row);
	mSortColOrder.set(date_issued, false);
	mProductInventoryData.quick_sort<false>(Inventories::date_issued);
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
	auto list = mProductInventoryData.where<Inventories::balance>([&](std::uint32_t b) {
		return  b == 0;
	}).isolate_column<Inventories::id>();
	AddImage(std::move(list), 0);
	
	list = mProductInventoryData.where<Inventories::balance>([&](std::uint32_t b) {
		return b > 0;
	}).isolate_column<Inventories::id>();
	AddImage(std::move(list), 1);
}

void InventoryView::ResetProductInventoryList(std::uint64_t productID)
{
	//clear states
	mImageTable.clear();
	mCheckedTable.clear();
	mAttributesTable.clear();
	mSortColOrder.reset();

	mProductId = productID;
	(typename Inventories::relation_t&)mProductInventoryData = InventoryInstance::instance().where<Inventories::product_id>(
		[&](std::uint64_t id) {
			return id == mProductId;
		}
	);
	mProductInventoryData.quick_sort<Inventories::date_issued, nl::order_dec<Inventories::elem_t<Inventories::date_issued>>>();
	SetupImages();
	SetItemCount(mProductInventoryData.size());
}
