#pragma once
#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/aui/aui.h>
#include <wx/aui/auibar.h>
#include <wx/listctrl.h>
#include <wx/artprov.h>
#include <wx/srchctrl.h>


#include "Tables.h"
#include "Instances.h"
#include "DataModelBase.h"
#include "DatabaseManger.h"
#include "SearchAutoComplete.h"
#include "InventoryDialog.h"

#include <nl_uuid.h>

#include <spdlog/spdlog.h>

#include <unordered_map>
#include <bitset>
#include <memory>
#include <random>
#include <functional>


class InventoryView : public wxListCtrl
{
public:
	InventoryView(std::uint64_t ProductID, wxWindow* parent, wxWindowID id, const wxPoint& position = wxDefaultPosition,
			const wxSize size = wxDefaultSize);
	virtual ~InventoryView();
protected:
	virtual wxListItemAttr* OnGetItemAttr(long item) const override;
	virtual int OnGetItemImage(long item) const override;
	virtual wxString OnGetItemText(long item, long column) const override;
	virtual bool OnGetItemIsChecked(long item) const override;

	void SetupImages();

public:
	void ResetProductInventoryList(std::uint64_t productID);

	void AddAttribute(std::vector<std::uint64_t>&& ids, std::shared_ptr<wxListItemAttr> attr);
	void AddAttribute(std::uint64_t id, std::shared_ptr<wxListItemAttr> attr);
	void RemoveAttribute(std::vector<std::uint64_t>&& ids);
	void RemoveAttribute(std::uint64_t id);

	void AddImage(std::vector<std::uint64_t>&& ids, int imageId);
	void AddImage(std::uint64_t id, int imageId);
	void RemoveImage(std::vector<std::uint64_t>&& ids);
	void RemoveImage(std::uint64_t id);

	void Check(std::uint64_t id);
	void UnCheck(std::uint64_t id);

	void ResetState();
	inline constexpr std::uint64_t GetProductId() const
	{
		return mProductId;
	}

	//getters for the state tables 
	inline constexpr Inventories& GetActiveInventoryData()
	{
		return mProductInventoryData;
	}

public:
	void OnColumnHeaderClick(wxListEvent& evt);
	void OnRightClick(wxListEvent& evt);
	void OnItemActivated(wxListEvent& evt);

	inline Inventories& GetProductInventory() { return mProductInventoryData; }
	//tool bar handlers
	void OnAddInventory(wxCommandEvent& evt);
	void OnRemoveInventory(wxCommandEvent& evt);
private:
	void CreateInventoryView();
	void CalculateBalance(Inventories::row_t& row);

	//inventory table notification handlers 
	void OnAddNotification(const Inventories::table_t& table, const Inventories::notification_data& data);


	//utiliy functions 
	void AddInOrder(const Inventories::row_t& row);
	void CreateAttributes();
private:
	//different attributes
	std::shared_ptr<wxListItemAttr> mJustAdded;

	//different images


	std::int32_t mSortColumn;
	std::uint64_t mProductId;
	std::bitset<Inventories::column_count - 2> mSortColOrder;
	Inventories mProductInventoryData;
	std::unique_ptr<DatabaseManger<Inventories>> mDatabaseMgr;
	std::unordered_map<std::uint64_t, std::shared_ptr<wxListItemAttr>> mAttributesTable{};
	std::unordered_map<std::uint64_t, int> mImageTable{};
	std::unordered_set<std::uint64_t> mCheckedTable{};
};

