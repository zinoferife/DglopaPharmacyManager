#include "common.h"
#include "MainView.h"

BEGIN_EVENT_TABLE(MainView, wxSplitterWindow)
	EVT_LIST_CACHE_HINT(MainView::DATA_VIEW, MainView::OnCacheHint)
	EVT_LIST_ITEM_SELECTED(MainView::DATA_VIEW, MainView::OnItemSelected)
	EVT_LIST_COL_CLICK(MainView::DATA_VIEW, MainView::OnColumnClicked)
	EVT_LIST_COL_RIGHT_CLICK(MainView::DATA_VIEW, MainView::OnColumnRightClicked)
	EVT_LIST_ITEM_RIGHT_CLICK(MainView::DATA_VIEW, MainView::OnRightClicked)
	EVT_LIST_ITEM_ACTIVATED(MainView::DATA_VIEW, MainView::OnItemActivated)
	EVT_TREE_ITEM_ACTIVATED(MainView::TREE_VIEW, MainView::OnTreeItemActivated)
	EVT_TREE_SEL_CHANGED(MainView::TREE_VIEW, MainView::OnTreeSelectionChanged)
	EVT_TREE_SEL_CHANGING(MainView::TREE_VIEW, MainView::OnTreeSelectionChanging)
	EVT_TREE_ITEM_EXPANDED(MainView::TREE_VIEW, MainView::OnTreeExpanded)
	EVT_TREE_ITEM_EXPANDING(MainView::TREE_VIEW, MainView::OnTreeExpanding)
	EVT_TREE_ITEM_COLLAPSED(MainView::TREE_VIEW, MainView::OnTreeCollapsed)
	EVT_TREE_ITEM_COLLAPSING(MainView::TREE_VIEW, MainView::OnTreeCollapsing)
	EVT_TREE_SET_INFO(MainView::TREE_VIEW, MainView::OnTreeSetInfo)
	EVT_TREE_GET_INFO(MainView::TREE_VIEW, MainView::OnTreeGetInfo)
END_EVENT_TABLE()


MainView::MainView(wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize& size)
: wxSplitterWindow(parent, id, position, size, wxSP_NOBORDER | wxSP_PERMIT_UNSPLIT | wxSP_THIN_SASH | wxSP_ARROW_KEYS | wxBORDER_THEME){
	CreateTree();
	CreateDataView();
	SetSashGravity(0.0);
	SetMinimumPaneSize(200);
	SplitVertically(mTree.get(), mDataView.get());
}

void MainView::CreateTree()
{
	mTree = std::make_unique<wxTreeCtrl>(this, TREE_VIEW, wxDefaultPosition, wxDefaultSize, wxTR_FULL_ROW_HIGHLIGHT | wxTR_NO_LINES | wxTR_SINGLE | wxBORDER_THEME);
	//make the tree
	auto RootId = mTree->AddRoot("Root");
	auto ProductId = mTree->AppendItem(RootId, "Products");
	auto SalesId = mTree->AppendItem(RootId, "Sales");
	auto InventoriesId = mTree->AppendItem(RootId, "Inventory");
	auto Custormers = mTree->AppendItem(RootId, "Customers");
}

void MainView::CreateDataView()
{
	mDataView = std::make_unique<DataView>(this, DATA_VIEW);

}

void MainView::OnTreeItemActivated(wxTreeEvent& evt)
{
	auto item = evt.GetItem();
	if (item.IsOk())
	{
		if (mTree->IsExpanded(item)){
			mTree->Collapse(item);
		}else{
			mTree->Expand(item);
		}
	}
}

void MainView::OnTreeGetInfo(wxTreeEvent& evt)
{
}

void MainView::OnTreeSetInfo(wxTreeEvent& evt)
{
}

void MainView::OnTreeCollapsed(wxTreeEvent& evt)
{
}

void MainView::OnTreeCollapsing(wxTreeEvent& evt)
{
}

void MainView::OnTreeExpanded(wxTreeEvent& evt)
{
}

void MainView::OnTreeExpanding(wxTreeEvent& evt)
{
}

void MainView::OnTreeSelectionChanged(wxTreeEvent& evt)
{
}

void MainView::OnTreeSelectionChanging(wxTreeEvent& evt)
{
}

void MainView::OnTreeStateImageClick(wxTreeEvent& evt)
{
}

void MainView::OnTreeItemMenu(wxTreeEvent& evt)
{
}

void MainView::OnCacheHint(wxListEvent& evt)
{
	mDataView->OnCacheHint(evt);
}

void MainView::OnItemSelected(wxListEvent& evt)
{
	mDataView->OnItemSelected(evt);
}

void MainView::OnColumnClicked(wxListEvent& evt)
{
	mDataView->OnColumnClicked(evt);
}

void MainView::OnColumnRightClicked(wxListEvent& evt)
{
	mDataView->OnColumnRightClicked(evt);
}

void MainView::OnRightClicked(wxListEvent& evt)
{
	mDataView->OnRightClicked(evt);
}

void MainView::OnItemActivated(wxListEvent& evt)
{
	mDataView->OnItemActivated(evt);
}
