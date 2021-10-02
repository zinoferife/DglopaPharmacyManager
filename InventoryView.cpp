#include "common.h"
#include "InventoryView.h"

InventoryView::InventoryView(std::uint64_t ProductID, wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize size)
: wxListCtrl(parent, id, position, size, wxLC_REPORT | wxLC_VIRTUAL | wxLC_HRULES),mProductId(ProductID){
	(typename Inventories::relation_t&)mProductInventoryData = InventoryInstance::instance().where<Inventories::product_id>(
		[&](std::uint64_t id) {
			return id == mProductId;
		}
	);
	CreateInventoryView();
	SetItemCount(mProductInventoryData.size());
}

void InventoryView::CreateInventoryView()
{
	for (size_t i = 2; i < InventoryInstance::instance().get_column_count(); i++)
	{
		AppendColumn(InventoryInstance::instance().get_name(i));
	}
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

int InventoryView::OnGetItemColumnImage(long item, long column) const
{
	return -1;
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

void InventoryView::ResetProductInventoryList(std::uint64_t productID)
{
	//clear states
	mImageTable.clear();
	mCheckedTable.clear();
	mAttributesTable.clear();

	mProductId = productID;
	(typename Inventories::relation_t&)mProductInventoryData = InventoryInstance::instance().where<Inventories::product_id>(
		[&](std::uint64_t id) {
			return id == mProductId;
		}
	);
	SetItemCount(mProductInventoryData.size());
}
