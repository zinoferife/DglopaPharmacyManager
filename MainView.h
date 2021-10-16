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


#include "TableMonoState.h"
#include "DataModelBase.h"
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
		VIEW_BOOK,
		PRODUCT_VIEW,
		SALES_VIEW
	};

	//font id
	enum	
	{
		TREE_MAIN,
		TREE_CHILD
	};

	//image index
	enum
	{
		user,
		download,
		folder,
		folder_open,
		action_add,
		action_remove,
		file
	};

//creation functions
public:
	void CreateProductView();
	void CreateSalesView();
	void CreatePageBook();
	void CreateTreeCtrl();
	void CreateImageLists();
	void SetDefaultArt();
	void SetUpFonts();
	wxTreeItemId AddToTree(wxTreeItemId parent, const std::string& name, int imageId = -1,
		int imageIdSel = -1);
	bool AddToViewMap(wxWindow* win, wxTreeItemId item);
//tree ctrl events
private:
	void OnTreeItemSelectionChanging(wxTreeEvent& evt);
	void OnTreeItemSelectionChanged(wxTreeEvent& evt);
	void OnTreeItemActivated(wxTreeEvent& evt);
	void OnTreeItemExpanding(wxTreeEvent& evt);
	void OnTreeItemExpanded(wxTreeEvent& evt);
	void OnTreeItemRightClick(wxTreeEvent& evt);
	void OnTreeItemMenu(wxTreeEvent& evt);
	void OnTreeItemGetToolTip(wxTreeEvent& evt);
	void OnTreeItemGetInfo(wxTreeEvent& evt);
	void OnTreeItemSetInfo(wxTreeEvent& evt);

//AuiNotebook events
private:
	void OnBookClosed(wxAuiNotebookEvent& evt);
	void OnBookClose(wxAuiNotebookEvent& evt);
	void OnBookTabRClick(wxAuiNotebookEvent& evt);
	void OnBookPageChanged(wxAuiNotebookEvent& evt);
	void OnBookPageChanging(wxAuiNotebookEvent& evt);

private:
	void RegisterNotifications();
	void UnregisterNotifications();

	void OnUserNotification(const Users::table_t& table, const Users::notification_data& data);

private:
	//page id, to dataView control
	
	std::unique_ptr<wxAuiNotebook> mViewBook{};
	std::unique_ptr<wxTreeCtrl> mTreeCtrl{};
	std::pair<wxTreeItemId, std::unique_ptr<ProductView>> mPView{};
	std::pair<wxTreeItemId, std::unique_ptr<wxDataViewCtrl>> mSView{};

	//for remvoing to addiing 
	std::unordered_map<wxTreeItemId, wxWindow*> mDataViewsMap{};
	bool mSignedIn;
private:
	std::array<wxFont,5> mMainViewFonts;
	std::unique_ptr<wxImageList> mImageList;
	wxTreeItemId mRootID;
	wxColour mTreeTextColour;
	wxColour mMainViewColour;
	DECLARE_EVENT_TABLE()

};

