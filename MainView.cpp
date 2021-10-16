#include "common.h"
#include "MainView.h"
BEGIN_EVENT_TABLE(MainView, wxPanel)
EVT_TREE_ITEM_ACTIVATED(MainView::TREE, MainView::OnTreeItemActivated)
EVT_TREE_SEL_CHANGED(MainView::TREE, MainView::OnTreeItemSelectionChanged)
EVT_TREE_SEL_CHANGING(MainView::TREE, MainView::OnTreeItemSelectionChanging)
EVT_TREE_GET_INFO(MainView::TREE, MainView::OnTreeItemGetInfo)
EVT_TREE_SET_INFO(MainView::TREE, MainView::OnTreeItemSetInfo)
EVT_TREE_ITEM_EXPANDED(MainView::TREE, MainView::OnTreeItemExpanded)
EVT_TREE_ITEM_EXPANDING(MainView::TREE, MainView::OnTreeItemExpanding)
EVT_TREE_ITEM_RIGHT_CLICK(MainView::TREE, MainView::OnTreeItemRightClick)
EVT_TREE_ITEM_MENU(MainView::TREE, MainView::OnTreeItemMenu)
EVT_TREE_ITEM_GETTOOLTIP(MainView::TREE, MainView::OnTreeItemGetToolTip)

EVT_AUINOTEBOOK_PAGE_CLOSE(MainView::VIEW_BOOK, MainView::OnBookClose)
EVT_AUINOTEBOOK_PAGE_CLOSED(MainView::VIEW_BOOK, MainView::OnBookClosed)
EVT_AUINOTEBOOK_PAGE_CHANGED(MainView::VIEW_BOOK, MainView::OnBookPageChanged)
EVT_AUINOTEBOOK_PAGE_CHANGING(MainView::VIEW_BOOK, MainView::OnBookPageChanging)
EVT_AUINOTEBOOK_TAB_RIGHT_UP(MainView::VIEW_BOOK, MainView::OnBookTabRClick)
END_EVENT_TABLE()

MainView::MainView(wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize& size)
	: wxPanel(parent, id, position, size), mMainViewColour(wxColour(240, 255, 255)), mSignedIn{false}{
	SetDefaultArt();
	SetUpFonts();
	RegisterNotifications();
	CreateImageLists();
	CreatePageBook();
	CreateTreeCtrl();

	wxSizer* MainSizer = new wxBoxSizer(wxVERTICAL);
	wxSizer* ViewSizer = new wxBoxSizer(wxHORIZONTAL);
	ViewSizer->Add(mTreeCtrl.get(), wxSizerFlags().Proportion(0).Expand().Border(wxLEFT | wxTOP, 5))->SetMinSize(wxSize(200,-1));
	ViewSizer->Add(mViewBook.get(), wxSizerFlags().Proportion(1).Expand());
	MainSizer->Add(ViewSizer, wxSizerFlags().Proportion(1).Expand().Border(wxALL, 1));
	SetSizer(MainSizer);
	GetSizer()->SetSizeHints(this);
}

MainView::~MainView()
{
	//allow the window hierarchy destory the objects
	UnregisterNotifications();
	mViewBook.release();
	mTreeCtrl.release();
	mSView.second.release();
	mPView.second.release();
}

void MainView::CreateProductView()
{
	mPView.second = std::make_unique<ProductView>(this, wxID_ANY);
	mPView.second->Hide();
}

void MainView::CreateSalesView()
{
	mSView.second = std::make_unique<wxDataViewCtrl>(mViewBook.get(), SALES_VIEW, wxDefaultPosition, wxDefaultSize, wxDV_ROW_LINES | wxNO_BORDER);
	wxDataViewModel* SalesModel = new DataModel<Sales>(SalesInstance::instance());
	mSView.second->AssociateModel(SalesModel);
	SalesModel->DecRef();
	for (size_t i = 0; i < Sales::column_count; i++)
	{
		mSView.second->AppendTextColumn(SalesInstance::instance().get_name(i), i);
	}
	for (size_t i = 0; i < 30; i++)
	{
		SalesInstance::instance().add(i, i, i * 2, (i * 10 % 5), 100, nl::clock::now(), "0.08");
	}
	Sales::notification_data data{};
	data.count = 30;
	SalesInstance::instance().notify(nl::notifications::add_multiple, data);
	mSView.second->Hide();
}

void MainView::CreatePageBook()
{
	mViewBook = std::make_unique<wxAuiNotebook>(this, VIEW_BOOK, wxDefaultPosition, wxDefaultSize, wxAUI_NB_DEFAULT_STYLE | wxNO_BORDER);
	mViewBook->SetImageList(mImageList.get());
	CreateSalesView();
	CreateProductView();
	mViewBook->SetForegroundColour(*wxWHITE);
	mViewBook->ClearBackground();
}

void MainView::CreateTreeCtrl()
{
	mTreeCtrl = std::make_unique<wxTreeCtrl>(this, TREE, wxDefaultPosition, wxDefaultSize, wxTR_FULL_ROW_HIGHLIGHT | wxTR_HIDE_ROOT | wxTR_SINGLE | wxTR_NO_LINES | wxTR_ROW_LINES | wxNO_BORDER);
	mTreeCtrl->SetImageList(mImageList.get());
	mTreeCtrl->SetIndent(5);
	mTreeCtrl->SetBackgroundColour(mMainViewColour);



	//create the tree elements
	mRootID = mTreeCtrl->AddRoot("Root");
	
	
	auto generalID = AddToTree(mRootID, "General", folder, folder_open);
	auto activityId = AddToTree(generalID, "Activities", file);


	auto transactionsID= AddToTree(mRootID, "Transactions", folder, folder_open);
	mSView.first = AddToTree(transactionsID, "Sales", file);
	AddToViewMap(mSView.second.get(), mSView.first);
	auto invoiceId =    AddToTree(transactionsID, "Invoice", file);
	auto orderId =      AddToTree(transactionsID, "Order", file);

	auto pharmacyID = AddToTree(mRootID, "Pharamacy", folder, folder_open);
	auto patientsId = AddToTree(pharmacyID, "Patients", file);
	auto prescriptionsId = AddToTree(pharmacyID, "Prescriptions", file);

	auto inventoryID = AddToTree(mRootID, "Inventories", folder, folder_open);
	mPView.first = AddToTree(inventoryID, "Products", file);
	AddToViewMap(mPView.second.get(),  mPView.first);
	
	auto miscID = AddToTree(mRootID, "Micellenous", folder, folder_open );


	
	mTreeCtrl->ExpandAll();
}

void MainView::CreateImageLists()
{
	mImageList = std::make_unique<wxImageList>(16, 16);
	mImageList->Add(wxArtProvider::GetBitmap("user"));
	mImageList->Add(wxArtProvider::GetBitmap("download"));
	mImageList->Add(wxArtProvider::GetBitmap("folder"));
	mImageList->Add(wxArtProvider::GetBitmap("folder_open"));
	mImageList->Add(wxArtProvider::GetBitmap("action_add"));
	mImageList->Add(wxArtProvider::GetBitmap("action_remove"));
	mImageList->Add(wxArtProvider::GetBitmap("file"));
}


void MainView::SetDefaultArt()
{
	SetBackgroundColour(mMainViewColour);
	ClearBackground();
}

void MainView::SetUpFonts()
{
	//font used for all the views 
	mMainViewFonts[TREE_MAIN].SetPointSize(9);
	mMainViewFonts[TREE_MAIN].SetFamily(wxFONTFAMILY_SWISS);
	mMainViewFonts[TREE_MAIN].SetFaceName("Bookman");
	mMainViewFonts[TREE_MAIN].SetWeight(wxFONTWEIGHT_BOLD);

	mMainViewFonts[TREE_CHILD].SetPointSize(9);
	mMainViewFonts[TREE_CHILD].SetFamily(wxFONTFAMILY_SWISS);
}

wxTreeItemId MainView::AddToTree(wxTreeItemId parent, const std::string& name, int imageId, int imageIdSel)
{
	if(parent == mRootID)
	{
		auto id = mTreeCtrl->AppendItem(parent, name, imageId, imageIdSel);
		mTreeCtrl->SetItemFont(id, mMainViewFonts[TREE_MAIN]);
		return id;
	}
	auto id = mTreeCtrl->AppendItem(parent, name, imageId, imageIdSel);
	mTreeCtrl->SetItemFont(id, mMainViewFonts[TREE_CHILD]);
	return id;
}

bool MainView::AddToViewMap(wxWindow* win, wxTreeItemId item)
{
	//win can be null pointer
	auto [iter, inserted] = mDataViewsMap.insert({ item, win });
	return inserted;
}



void MainView::OnTreeItemSelectionChanging(wxTreeEvent& evt)
{
	
}

void MainView::OnTreeItemSelectionChanged(wxTreeEvent& evt)
{
	
}

void MainView::OnTreeItemActivated(wxTreeEvent& evt)
{
	if (mSignedIn)
	{
		auto item = evt.GetItem();
		if (item.IsOk())
		{
			auto winIter = mDataViewsMap.find(item);
			if (winIter != mDataViewsMap.end()) {
				wxWindow* win = winIter->second;
				auto pageIndex = mViewBook->GetPageIndex(win);
				if (pageIndex != wxNOT_FOUND) {
					if (!win->IsShown()) win->Show();
					mViewBook->SetSelection(pageIndex);
				}
				else {
					if (win != nullptr) {
						mViewBook->AddPage(win, mTreeCtrl->GetItemText(item), true, mTreeCtrl->GetItemImage(item));
					}
				}
			}
		}
	}else{
		wxMessageBox("Please sign in, click the lock icon to sign in", "SIGN IN", wxICON_ERROR | wxOK);
	}
}

void MainView::OnTreeItemExpanding(wxTreeEvent& evt)
{

}

void MainView::OnTreeItemExpanded(wxTreeEvent& evt)
{

}

void MainView::OnTreeItemRightClick(wxTreeEvent& evt)
{
	
}

void MainView::OnTreeItemMenu(wxTreeEvent& evt)
{
	
}

void MainView::OnTreeItemGetToolTip(wxTreeEvent& evt)
{
	
}

void MainView::OnTreeItemGetInfo(wxTreeEvent& evt)
{
	
}

void MainView::OnTreeItemSetInfo(wxTreeEvent& evt)
{
	
}

void MainView::OnBookClosed(wxAuiNotebookEvent& evt)
{

}

void MainView::OnBookClose(wxAuiNotebookEvent& evt)
{
	auto pageIndex = evt.GetSelection();
	if (pageIndex != wxNOT_FOUND) {
		mViewBook->RemovePage(pageIndex);
	}
	evt.Veto();
}

void MainView::OnBookTabRClick(wxAuiNotebookEvent& evt)
{
}

void MainView::OnBookPageChanged(wxAuiNotebookEvent& evt)
{
}

void MainView::OnBookPageChanging(wxAuiNotebookEvent& evt)
{
}

void MainView::RegisterNotifications()
{
	UsersInstance::instance().sink<nl::notifications::evt>().add_listener<MainView, &MainView::OnUserNotification>(this);
}

void MainView::UnregisterNotifications()
{
	UsersInstance::instance().sink<nl::notifications::evt>().remove_listener<MainView, & MainView::OnUserNotification>(this);
}

void MainView::OnUserNotification(const Users::table_t& table, const Users::notification_data& data)
{
	switch (data.event_type)
	{
	//sign in
	case 1:
	{
		wxWindow* win = mPView.second.get();
		auto pageIndex = mViewBook->GetPageIndex(win);
		if (pageIndex != wxNOT_FOUND) {
			if (!win->IsShown()) win->Show();
			mViewBook->SetSelection(pageIndex);
		}
		else {
			if (win != nullptr) {
				mViewBook->AddPage(win, mTreeCtrl->GetItemText(mPView.first), true, mTreeCtrl->GetItemImage(mPView.first));
			}
		}
		mSignedIn = true;
	}
	break;
	//sign out 
	case 0:
	{
		size_t active_page = mViewBook->GetSelection();
		mViewBook->RemovePage(active_page);
		size_t page_count = mViewBook->GetPageCount();
		for (size_t i = 0; i < page_count; i++){
			mViewBook->RemovePage(i);
		}
		mSignedIn = false;
	}
	break;
	default:
		break;
	}
}
