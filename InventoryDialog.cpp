#include "common.h"
#include "InventoryDialog.h"

BEGIN_EVENT_TABLE(InventoryDialog, wxDialog)
EVT_BUTTON(wxID_OK, InventoryDialog::OnOk)
EVT_BUTTON(wxID_CANCEL, InventoryDialog::OnCancel)
EVT_BUTTON(InventoryDialog::ID_CALENDAR, InventoryDialog::OnCalendar)
END_EVENT_TABLE()

InventoryDialog::InventoryDialog(Inventories::row_t& row_, wxWindow* parent)
: wxDialog(parent, wxID_ANY, "Inventory entry"), row(row_) {
	CreateDialog();
	SizeDialog();
}

bool InventoryDialog::TransferDataFromWindow()
{
	nl::row_value<Inventories::quantity_in>(row) = mQuantityInControl->GetValue();
	nl::date_time_t exp_date;
	bool done = false;
	try {
		exp_date = nl::from_string_date(mExpiryDate->GetValue().ToStdString());	
	}catch (std::logic_error& error) {
			wxMessageBox(fmt::format("{} is invalid, date format should be in \'YYYY-MM-DD\'"), mExpiryDate->GetValue().ToStdString(), wxICON_ERROR | wxOK);
			return false;
	}
	return true;
}

bool InventoryDialog::TransferDataToWindow()
{
	return true;
}

void InventoryDialog::CreateDialog()
{
	texts = new wxStaticText[2];
	texts[0].Create(this, wxID_ANY, "Quantity in: ", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	texts[1].Create(this, wxID_ANY, "Expiry date: ", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	
	mOkCancel = new wxButton[2];
	mOkCancel[0].Create(this, wxID_OK, "OK");
	mOkCancel[0].Create(this, wxID_CANCEL, "Cancel");
	//look for a calender icon
	mCalenderButton = new wxBitmapButton(this, ID_CALENDAR, wxArtProvider::GetBitmap("application"));

	mQuantityInControl = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS | wxALIGN_LEFT, 0, 
		std::numeric_limits<typename Inventories::elem_t<Inventories::quantity_in>>::max());
	mExpiryDate = new wxTextCtrl(this, wxID_ANY);
}

void InventoryDialog::SizeDialog()
{
	wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* okCancleSizer = new wxBoxSizer(wxHORIZONTAL);

	okCancleSizer->AddStretchSpacer();
	okCancleSizer->Add(&mOkCancel[0], wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));
	okCancleSizer->Add(&mOkCancel[1], wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));

	wxFlexGridSizer* flexSizer = new wxFlexGridSizer(2,2, 5,5);
	flexSizer->Add(&texts[0], wxSizerFlags().Align(wxLEFT).Border(wxALL, 5));
	flexSizer->Add(mQuantityInControl, wxSizerFlags().Align(wxLEFT).Border(wxALL, 5));

	flexSizer->Add(&texts[1], wxSizerFlags().Align(wxLEFT).Border(wxALL, 5));
	flexSizer->Add(mExpiryDate, wxSizerFlags().Align(wxLEFT).Border(wxALL, 5));

	boxSizer->Add(flexSizer, wxSizerFlags().Align(wxLEFT).Border(wxALL, 5));
	boxSizer->Add(okCancleSizer, wxSizerFlags().Expand().Border(wxALL, 5));

	topSizer->Add(boxSizer, wxSizerFlags().Expand().Border(wxALL, 5));
	SetSizer(topSizer);
	topSizer->SetSizeHints(this);
}

void InventoryDialog::OnOk(wxCommandEvent& evt)
{
	wxWindowID ret_code;
	if (Validate() && TransferDataFromWindow()){
		ret_code = wxID_OK;
	}else{
		ret_code = ID_IMPROPER_DATE;
	}

	if (IsModal()) EndModal(ret_code);
	else
	{
		SetReturnCode(ret_code);
		this->Show(false);
	}
}

void InventoryDialog::OnCancel(wxCommandEvent& evt)
{
	if (wxMessageBox(wxT("Are you sure you want to cancel Inventory entry"), wxT("Inventory entry"), wxYES | wxNO) == wxYES)
	{
		if (IsModal()) EndModal(wxID_CANCEL);
		else
		{
			SetReturnCode(wxID_CANCEL);
			this->Show(false);
		}
	}
}

void InventoryDialog::OnCalendar(wxCommandEvent& evt)
{

}
