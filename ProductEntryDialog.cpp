#include "common.h"
#include "ProductEntryDialog.h"
BEGIN_EVENT_TABLE(ProductEntryDialog, wxDialog)
EVT_BUTTON(wxID_OK, ProductEntryDialog::OnOk)
EVT_BUTTON(wxID_CANCEL, ProductEntryDialog::OnCancel)
END_EVENT_TABLE()
IMPLEMENT_ABSTRACT_CLASS(ProductEntryDialog, wxDialog)

ProductEntryDialog::search_table ProductEntryDialog::mSearchTable{};

ProductEntryDialog::ProductEntryDialog(wxWindow* window)
	: wxDialog(window, wxID_ANY, wxEmptyString), validate_float{ 0.0f }{
	choices.resize(3);
	CreateDialog();
	SizeDialog();
	SetBackgroundColour(*wxWHITE);
}

bool ProductEntryDialog::TransferDataFromWindow()
{
	nl::row_value<Products::id>(product) = GenRandomId();
	nl::row_value<Products::name>(product) = std::move(mProductNameCtrl->GetValue().ToStdString());
	nl::row_value<Products::unit_price>(product) = std::move(mProductUnitPriceCtrl->GetValue().ToStdString());
	nl::row_value<Products::package_size>(product) = std::move(mProductPackageSizeCtrl->GetValue());
	nl::row_value<Products::stock_count>(product) = 0;
	
	std::string category = std::move(mProductCategoryCtrl->GetValue().ToStdString());
	std::transform(category.begin(), category.end(), category.begin(), [&](char& c) {
		return std::toupper(c);
	});
	Categories::iterator iter = CategoriesInstance::instance().find_on<Categories::name>(category);
	if (iter == CategoriesInstance::instance().end())
	{
		Categories::notification_data data;
		data.row_iterator = CategoriesInstance::instance().add(GenRandomId(), category);
		CategoriesInstance::instance().notify<nl::notifications::add>(data);
		nl::row_value<Products::category_id>(product) = nl::row_value<Categories::id>(*data.row_iterator);
	}
	else {
		nl::row_value<Products::category_id>(product) = nl::row_value<Categories::id>(*iter);
	}
	//details
	nl::row_value<ProductDetails::id>(productDetail) = nl::row_value<Products::id>(product);
	nl::row_value<ProductDetails::active_ing>(productDetail) = std::move(mProductActiveIngredentCtrl->GetValue().ToStdString());
	nl::row_value<ProductDetails::dir_for_use>(productDetail) = std::move(mProductDirForUseCtrl->GetValue().ToStdString());
	nl::row_value<ProductDetails::p_class>(productDetail) = choices[mProductClassCtrl->GetSelection()];

	//do auto correct update
	mSearchTable.add_in_order<0>({ nl::row_value<Products::name>(product) });
	mSearchTable.add_in_order<0>({ category });

	//json string
	mSearchTable.add_in_order<0>({nl::row_value<ProductDetails::dir_for_use>(productDetail)});
	
	std::string view; 
	std::string data = nl::row_value<ProductDetails::active_ing>(productDetail);
	data.erase(std::remove(data.begin(), data.end(), ' '), data.end());
	std::stringstream stream(data);
	std::getline(stream, view, ',');
	mSearchTable.add_in_order<0>({view});
	while (!stream.eof()){
		std::getline(stream, view, ',');
		mSearchTable.add_in_order<0>({view});
	}


	return true;
}

bool ProductEntryDialog::TransferDataToWindow()
{
	//transfer the combo box data
	if (CategoriesInstance::instance().empty()) return true;
	auto names = CategoriesInstance::instance().isolate_column<Categories::name>();
	for (auto&& name : names){
		mProductCategoryCtrl->AppendString(name);
		mSearchTable.add(name);
	}
	mSearchTable.unique<0>();
	return true;
}


void ProductEntryDialog::CreateDialog()
{
	choices[0] = "OTC";
	choices[1] = "P";
	choices[2] = "POM";
	mProductClassCtrl = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(200, -1), choices);
	mProductClassCtrl->SetSelection(0);

	mProductCategoryCtrl = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200, -1));
	mProductCategoryCtrl->AutoComplete(new SearchAutoComplete<Categories, Categories::name>(CategoriesInstance::instance()));
	mProductCategoryCtrl->SetValidator(wxTextValidator{ wxFILTER_EMPTY });

	

	mProductNameCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200, -1));
	mProductNameCtrl->AutoComplete(new SearchAutoComplete<search_table, 0>(mSearchTable));
	mProductNameCtrl->SetValidator(wxTextValidator{ wxFILTER_EMPTY });


	mProductActiveIngredentCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200, -1));
	mProductActiveIngredentCtrl->SetHint("Ingredenent1, Ingredent2...");
	mProductActiveIngredentCtrl->AutoComplete(new SearchAutoComplete<search_table, 0>(mSearchTable));

	mProductUnitPriceCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200, -1));
	mProductUnitPriceCtrl->SetHint("00.00");
	mProductUnitPriceCtrl->SetValidator(wxFloatingPointValidator<float>(2, &validate_float, wxNUM_VAL_ZERO_AS_BLANK));

	mProductPackageSizeCtrl = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200, -1), wxSP_ARROW_KEYS | wxALIGN_LEFT, 0, 10000);

	pane = new wxCollapsiblePane(this, wxID_ANY, "Add details");
	pane->SetBackgroundColour(*wxWHITE);
	wPane = pane->GetPane();
	wPane->SetBackgroundColour(*wxWHITE);
	mProductDescCtrl = new wxTextCtrl(wPane, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_RICH | wxVSCROLL);
	mProductDirForUseCtrl = new wxTextCtrl(wPane, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_RICH | wxVSCROLL);
	mProductDirForUseCtrl->AutoComplete(new SearchAutoComplete<search_table, 0>(mSearchTable));

	descp[0] = new wxStaticText(this, wxID_ANY, wxT("PRODUCT ENTRY"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	descp[0]->SetFont(wxFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

	descp[1] = new wxStaticText(this, wxID_ANY, wxT("Please enter a new product"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	descp[2] = new wxStaticText(this, wxID_ANY, wxT("Product class:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	descp[3] = new wxStaticText(this, wxID_ANY, wxT("Product Category:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	descp[4] = new wxStaticText(this, wxID_ANY, wxT("Product Name:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	descp[5] = new wxStaticText(this, wxID_ANY, wxT("Product Active Ingredent:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	descp[6] = new wxStaticText(wPane, wxID_ANY, wxT("Product Description:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	descp[7] = new wxStaticText(wPane, wxID_ANY, wxT("Product Direction for use:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	descp[8] = new wxStaticText(this, wxID_ANY, wxT("Product Unit price(N):"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	descp[9] = new wxStaticText(this, wxID_ANY, wxT("Product package size"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);

	mOkButton = new wxButton(this, wxID_OK, "Ok");
	mCancelButton = new wxButton(this, wxID_CANCEL, "Cancel");
	SetDefaultItem(mOkButton);
}

void ProductEntryDialog::SizeDialog()
{
	wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* okCancleSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* classSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* collSizer = new wxBoxSizer(wxVERTICAL);
	wxFlexGridSizer* flexSizer = new wxFlexGridSizer(6, 3, 5, 5);

	boxSizer->Add(descp[0], 0, wxALIGN_LEFT | wxALL, 5);
	boxSizer->Add(descp[1], 0, wxALIGN_LEFT | wxALL, 5);
	boxSizer->AddSpacer(10);


	flexSizer->Add(descp[2], 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	flexSizer->Add(mProductClassCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	flexSizer->AddStretchSpacer();

	flexSizer->Add(descp[3], 0, wxALIGN_LEFT | wxALL, 5);
	flexSizer->Add(mProductCategoryCtrl, 0, wxGROW | wxALL, 5);
	flexSizer->AddStretchSpacer();


	flexSizer->Add(descp[4], 0, wxALIGN_LEFT | wxALL, 5);
	flexSizer->Add(mProductNameCtrl, 0, wxGROW | wxALL, 5);
	flexSizer->AddStretchSpacer();

	flexSizer->Add(descp[5], 0, wxALIGN_LEFT | wxALL, 5);
	flexSizer->Add(mProductActiveIngredentCtrl, 0, wxGROW | wxALL, 5);
	flexSizer->AddStretchSpacer();

	flexSizer->Add(descp[8], 0, wxALIGN_LEFT | wxALL, 5);
	flexSizer->Add(mProductUnitPriceCtrl, 0, wxALIGN_LEFT | wxALL, 5);
	flexSizer->AddStretchSpacer();

	flexSizer->Add(descp[9], 0, wxALIGN_LEFT | wxALL, 2);
	flexSizer->Add(mProductPackageSizeCtrl, 0, wxALIGN_LEFT | wxALL, 2);
	flexSizer->AddStretchSpacer();

	boxSizer->Add(flexSizer, 0, wxALIGN_LEFT | wxALL, 5);

	collSizer->Add(descp[6], 0, wxALIGN_LEFT | wxALL, 5);
	collSizer->Add(mProductDescCtrl, 0, wxGROW | wxALL, 5);

	collSizer->Add(descp[7], 0, wxALIGN_LEFT | wxALL, 5);
	collSizer->Add(mProductDirForUseCtrl, 0, wxGROW | wxALL, 5);

	wPane->SetSizer(collSizer);
	collSizer->SetSizeHints(wPane);

	boxSizer->Add(pane, wxSizerFlags().Expand().Border(wxALL, 5));
	boxSizer->AddSpacer(10);

	wxStaticLine* line = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	boxSizer->Add(line, 0, wxGROW | wxALL, 2);

	okCancleSizer->AddStretchSpacer();
	okCancleSizer->Add(mOkButton, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	okCancleSizer->Add(mCancelButton, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	boxSizer->Add(okCancleSizer, 0, wxEXPAND | wxALL, 5);
	SetSizer(topSizer);
	topSizer->Add(boxSizer, 0, wxGROW | wxALL, 2);
	topSizer->SetSizeHints(this);
}


void ProductEntryDialog::OnOk(wxCommandEvent& evt)
{
	if (Validate() && TransferDataFromWindow()){
		if (IsModal()) EndModal(wxID_OK);
		else{
			SetReturnCode(wxID_OK);
			this->Show(false);
		}
	}
	else{
		if (IsModal()) EndModal(ID_ERROR);
		else {
			SetReturnCode(ID_ERROR);
			this->Show(false);
		}
	}
}

void ProductEntryDialog::OnCancel(wxCommandEvent& evt)
{
	if (wxMessageBox("Are you sure you want to cancel product entry?", "Product entry", wxICON_INFORMATION | wxYES_NO) == wxYES)
	{
		if (IsModal()) EndModal(wxID_CANCEL);
		else {
			SetReturnCode(wxID_CANCEL);
			this->Show(false);
		}
	}
}

