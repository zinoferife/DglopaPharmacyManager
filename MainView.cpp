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
	mMainViewManager = std::make_unique<wxAuiManager>(this);
	SetDefaultArt();
	SetUpFonts();
	RegisterNotifications();
	CreateImageLists();
	CreatePageBook();
	CreateTreeCtrl();
	SetMainViewLayout();
}

MainView::~MainView()
{
	if (mMainViewManager) {
		mMainViewManager->UnInit();
	}
	//allow the window hierarchy destory the objects
	UnregisterNotifications();
	mMainViewManager.reset(nullptr);
	mViewBook.release();
	mTreeCtrl.release();
	mPView.second.release();
	mSView.second.release();
	mPrescriptionView.second.release();
	if (!mDataViewsMap.empty()) { mDataViewsMap.clear(); }
}

void MainView::CreateProductView()
{
	mPView.second = std::make_unique<ProductView>(this, wxID_ANY);
	mPView.second->Hide();
	TestTupleBuffer();
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

void MainView::CreatePrescriptionView()
{
	mPrescriptionView.second = std::make_unique<PrescriptionView>(mViewBook.get(), PRESCRIPTION_VIEW, wxDefaultPosition, wxDefaultSize);
	mPrescriptionView.second->Hide();
}

void MainView::CreatePageBook()
{
	mViewBook = std::make_unique<wxAuiNotebook>(this, VIEW_BOOK, wxDefaultPosition, wxDefaultSize, wxAUI_NB_DEFAULT_STYLE | wxNO_BORDER);
	mViewBook->SetImageList(mImageList.get());
	CreateSalesView();
	CreateProductView();
	CreatePrescriptionView();
	mViewBook->SetBackgroundColour(*wxWHITE);
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
	
	auto pharmacyID = AddToTree(mRootID, "Pharamacy", hospital, -1);
	mPView.first = AddToTree(pharmacyID, "Products", file);
	AddToViewMap(mPView.second.get(),  mPView.first);
	auto patientsId = AddToTree(pharmacyID, "Patients", file);
	mPrescriptionView.first = AddToTree(pharmacyID, "Prescriptions", file);
	AddToViewMap(mPrescriptionView.second.get(), mPrescriptionView.first);
	auto posionId = AddToTree(pharmacyID, "Posion book", file);


	auto transactionsID= AddToTree(mRootID, "Transactions", health_file, -1);
	mSView.first = AddToTree(transactionsID, "Sales", file);
	AddToViewMap(mSView.second.get(), mSView.first);
	auto invoiceId = AddToTree(transactionsID, "Invoice", file);
	auto orderId =  AddToTree(transactionsID, "Order", file);


	
	mTreeCtrl->ExpandAll();
}

void MainView::CreateImageLists()
{
	mImageList = std::make_unique<wxImageList>(16, 16);
	mImageList->Add(wxArtProvider::GetBitmap("user"));
	mImageList->Add(wxArtProvider::GetBitmap("download"));
	mImageList->Add(wxArtProvider::GetBitmap("hospital"));
	mImageList->Add(wxArtProvider::GetBitmap("health-file"));
	mImageList->Add(wxArtProvider::GetBitmap("action_add"));
	mImageList->Add(wxArtProvider::GetBitmap("action_remove"));
	mImageList->Add(wxArtProvider::GetBitmap("drug"));
}


void MainView::SetDefaultArt()
{
	wxColour colour = wxTheColourDatabase->Find("Aqua");
	wxAuiDockArt* art = mMainViewManager->GetArtProvider();
	art->SetMetric(wxAUI_DOCKART_CAPTION_SIZE, 24);
	art->SetMetric(wxAUI_DOCKART_GRIPPER_SIZE, 5);
	art->SetMetric(wxAUI_DOCKART_SASH_SIZE, 5);
	art->SetColour(wxAUI_DOCKART_SASH_COLOUR,  *wxWHITE);
	art->SetColour(wxAUI_DOCKART_BACKGROUND_COLOUR, colour);
	art->SetColour(wxAUI_DOCKART_BORDER_COLOUR, *wxWHITE);
	art->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);
	art->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_HORIZONTAL);
	mMainViewManager->SetFlags(mMainViewManager->GetFlags() | wxAUI_MGR_ALLOW_ACTIVE_PANE | wxAUI_MGR_VENETIAN_BLINDS_HINT);


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

void MainView::SetMainViewLayout()
{
	mMainViewManager->AddPane(mTreeCtrl.get(), wxAuiPaneInfo().Name("TreeCtrl").Caption(wxT("Modules")).Resizable().MinSize(wxSize(200, -1))
		.Floatable(false).Left().Hide());
	mMainViewManager->AddPane(mViewBook.get(), wxAuiPaneInfo().Name("ViewBook").Caption(wxT("Dashborad")).CenterPane());

	mMainViewManager->Update();

}

void MainView::ShowModuleSelect()
{
	auto& pane = mMainViewManager->GetPane("TreeCtrl");
	if (pane.IsOk() && !pane.IsShown()) {
		pane.Show();
		mMainViewManager->Update();
	}
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
			else {
				auto treeText = mTreeCtrl->GetItemText(item).ToStdString();
				wxMessageBox(fmt::format("Still working on \"{}\" page, please be patient.", treeText), "Opps", wxICON_WARNING | wxOK);
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

void MainView::ImportJson(const fs::path& path)
{
	if (path.extension().string() != ".json"){
		wxMessageBox(fmt::format("{} is not a JSON file", path.filename().string()), "JSON IMPORT", wxICON_WARNING);
		return;
	}
	std::fstream file(path, std::ios::in);
	if (!file.is_open()){
		wxMessageBox(fmt::format("invalid json file {}", path.filename().string()), "JSON IMPORT", wxICON_WARNING);
		return;
	}
	mPView.second->ImportProductsFromJson(file);
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
		auto& pane = mMainViewManager->GetPane("TreeCtrl");
		if (pane.IsOk()) {
			pane.Show();
			mMainViewManager->Update();
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

void MainView::TestTupleBuffer()
{
	nl::tuple_buffer<Products::row_t> buffer;
	buffer.set_state((size_t)Products::id, (size_t)Products::name);
	auto& row = ProductInstance::instance()[2];
	nl::write_tuple_buffer(row, buffer);

	Products::row_t new_tuple;
	nl::read_tuple_buffer(new_tuple, buffer);

	spdlog::get("log")->info("{}", nl::row_value<Products::name>(new_tuple));

}
