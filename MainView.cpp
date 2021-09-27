#include "common.h"
#include "MainView.h"
BEGIN_EVENT_TABLE(MainView, wxPanel)
EVT_TREE_SEL_CHANGED(MainView::TREE, MainView::OnTreeItemSelectionChanged)
EVT_TREE_SEL_CHANGED(MainView::TREE, MainView::OnTreeItemSelectionChanging)
END_EVENT_TABLE()

MainView::MainView(wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize& size)
: wxPanel(parent, id, position, size), mMainViewColour(wxColour(240, 255, 255)){
	SetDefaultArt();
	SetUpFonts();
	CreatePageBook();
	CreateTreeCtrl();

	wxSizer* MainSizer = new wxBoxSizer(wxVERTICAL);
	wxSizer* ViewSizer = new wxBoxSizer(wxHORIZONTAL);
	ViewSizer->Add(mTreeCtrl.get(), wxSizerFlags().Proportion(0).Expand().Border(wxLEFT | wxTOP, 5))->SetMinSize(wxSize(200,-1));
	ViewSizer->Add(mPageBook.get(), wxSizerFlags().Proportion(1).Expand());
	MainSizer->Add(ViewSizer, wxSizerFlags().Proportion(1).Expand().Border(wxALL, 1));
	SetSizer(MainSizer);
	GetSizer()->SetSizeHints(this);
}

MainView::~MainView()
{
	//allow the window hierarchy destory the objects
	mPageBook.release();
	mProductView.second.release();
	mSalesView.second.release();
	mPView.release();
}

void MainView::CreatePView()
{
	mPView = std::make_unique<ProductView>(this, wxID_ANY);
	mPageBook->AddPage(mPView.get(), "Product", true, 0);
}

void MainView::CreateSalesView()
{
	SalesInstance::instance().as<Sales::id>("Sale number");
	SalesInstance::instance().as<Sales::product_id>("product number");
	SalesInstance::instance().as<Sales::customer_id>("customer number");
	SalesInstance::instance().as<Sales::user_id>("user number");
	SalesInstance::instance().as<Sales::amount>("amount");
	SalesInstance::instance().as<Sales::date>("date");
	SalesInstance::instance().as<Sales::price>("price");

	mSalesView.second = std::make_unique<wxDataViewCtrl>(mPageBook.get(), SALES_VIEW, wxDefaultPosition, wxDefaultSize, wxDV_ROW_LINES | wxNO_BORDER);
	wxDataViewModel* SalesModel = new SalesDataModel(SalesInstance::instance());
	mSalesView.second->AssociateModel(SalesModel);
	SalesModel->DecRef();
	for (size_t i = 0; i < Sales::column_count; i++)
	{
		mSalesView.second->AppendTextColumn(SalesInstance::instance().get_name(i), i);
	}
	for (size_t i = 0; i < 3; i++)
	{
		SalesInstance::instance().add(i, i, i * 2, (i * 10 % 5), 100, nl::clock::now(), "0.08");
		SalesInstance::instance().notify(nl::notifications::add, i);
	}
}

void MainView::CreatePageBook()
{
	mPageBook = std::make_unique<wxAuiNotebook>(this, PAGE_BOOK, wxDefaultPosition, wxDefaultSize, wxAUI_NB_DEFAULT_STYLE | wxNO_BORDER);
	CreatePageBookImageList();
	CreateSalesView();
	CreatePView();
	mPageBook->SetThemeEnabled(false);
	mPageBook->SetForegroundColour(*wxWHITE);
	mPageBook->AddPage(mSalesView.second.get(), "Sales", false, 1);
}

void MainView::CreateTreeCtrl()
{
	mTreeCtrl = std::make_unique<wxTreeCtrl>(this, TREE, wxDefaultPosition, wxDefaultSize, wxTR_FULL_ROW_HIGHLIGHT | wxTR_HIDE_ROOT | wxTR_SINGLE | wxTR_NO_LINES | wxTR_ROW_LINES | wxNO_BORDER);
	CreateTreeCtrlImageList();
	mTreeCtrl->SetIndent(5);
	mTreeCtrl->SetBackgroundColour(mMainViewColour);



	//create the tree elements
	mRootID = mTreeCtrl->AddRoot("Root");
	
	
	auto generalID = AddToTree(mRootID, "General", 1, 2);
	auto activityId = AddToTree(generalID, "Activities", 3, 4);


	auto transactionsID= AddToTree(mRootID, "Transactions", 1, 2);
	mSalesView.first =  AddToTree(transactionsID, "Sales", 3, 4);
	auto invoiceId =    AddToTree(transactionsID, "Invoice", 3, 4);
	auto orderId =      AddToTree(transactionsID, "Order", 3, 4);

	auto pharmacyID = AddToTree(mRootID, "Pharamacy", 1, 2);
	auto patientsId = AddToTree(pharmacyID, "Patients", 3, 4);
	auto prescriptionsId = AddToTree(pharmacyID, "Prescriptions", 3, 4);

	auto inventoryID = AddToTree(mRootID, "Inventories", 1, 2);
	mProductView.first = AddToTree(inventoryID, "Products", 3, 4);
	
	auto miscID = AddToTree(mRootID, "Micellenous", 1, 2);



	mTreeCtrl->ExpandAll();
}

void MainView::CreateImageLists()
{
}

void MainView::CreatePageBookImageList()
{
	wxImageList* imgList = new wxImageList(16, 16);
	imgList->Add(wxArtProvider::GetBitmap("user"));
	imgList->Add(wxArtProvider::GetBitmap("download"));
	mPageBook->AssignImageList(imgList);
}

void MainView::CreateTreeCtrlImageList()
{
	wxImageList* imgList = new wxImageList(16, 16);
	imgList->Add(wxArtProvider::GetBitmap("user"));
	imgList->Add(wxArtProvider::GetBitmap("folder"));
	imgList->Add(wxArtProvider::GetBitmap("folder_open"));
	imgList->Add(wxArtProvider::GetBitmap("action_add"));
	imgList->Add(wxArtProvider::GetBitmap("action_remove"));
	mTreeCtrl->AssignImageList(imgList);
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
		mTreeIdSet.insert(id);
		return id;
	}
	auto id = mTreeCtrl->AppendItem(parent, name, imageId, imageIdSel);
	mTreeCtrl->SetItemFont(id, mMainViewFonts[TREE_CHILD]);
	return id;
}

void MainView::OnTreeItemSelectionChanging(wxTreeEvent& evt)
{
	spdlog::get("log")->info("In selection changing");
}

void MainView::OnTreeItemSelectionChanged(wxTreeEvent& evt)
{

}
