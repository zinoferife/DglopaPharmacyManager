#pragma once
#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/treebook.h>
#include <wx/dataview.h>
#include <wx/aui/auibook.h>
#include <wx/aui/tabart.h>
#include <wx/treectrl.h>
#include <wx/artprov.h>

#include <memory>
#include <spdlog/spdlog.h>
#include <unordered_set>
#include <unordered_map>


#include "DataModel.h"
#include "ProductView.h"

namespace std
{
	template<>
	struct hash<wxTreeItemId>
	{
		size_t operator()(const wxTreeItemId& value) const noexcept
		{
			return std::hash<std::uint32_t>{}(wxPtrToUInt(value.GetID()));
		}
	};
}
class MainView : public wxPanel
{
public:
	MainView(wxWindow* parent, wxWindowID id,
		const wxPoint& position = wxDefaultPosition, const wxSize& size = wxDefaultSize);
	virtual ~MainView();
	//ids
	enum
	{
		TREE = wxID_HIGHEST + 500,
		PAGE_BOOK,
		PRODUCT_VIEW,
		SALES_VIEW
	};

	//font id
	enum	
	{
		TREE_MAIN,
		TREE_CHILD
	};

//creation functions
public:
	void CreatePView();
	void CreateSalesView();
	void CreatePageBook();
	void CreateTreeCtrl();
	void CreateImageLists();
	void CreatePageBookImageList();
	void CreateTreeCtrlImageList();
	void SetDefaultArt();
	void SetUpFonts();
	wxTreeItemId AddToTree(wxTreeItemId parent, const std::string& name, int imageId = -1,
		int imageIdSel = -1);

//tree ctrl events
private:
	void OnTreeItemSelectionChanging(wxTreeEvent& evt);
	void OnTreeItemSelectionChanged(wxTreeEvent& evt);

private:
	//page id, to dataView control
	std::pair<wxTreeItemId, std::unique_ptr<wxDataViewCtrl>> mProductView{};
	std::pair<wxTreeItemId, std::unique_ptr<wxDataViewCtrl>> mSalesView{};
	
	//set of parent treeID, used to filter out activations
	std::unordered_set<wxTreeItemId> mTreeIdSet{};
	
	//map of treeId to page index
	std::unordered_map<wxTreeItemId, std::size_t> mDataViews{};
	std::unique_ptr<wxAuiNotebook> mPageBook{};
	std::unique_ptr<wxTreeCtrl> mTreeCtrl{};
	std::unique_ptr<ProductView> mPView{};
	std::array<wxFont,5> mMainViewFonts;

private:
	wxTreeItemId mRootID;
	wxColour mTreeTextColour;
	wxColour mMainViewColour;
	DECLARE_EVENT_TABLE()

};

