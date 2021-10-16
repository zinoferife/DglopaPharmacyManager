#include "common.h"
#include "InventoryDialog.h"

BEGIN_EVENT_TABLE(InventoryDialog, wxDialog)
EVT_BUTTON(wxID_OK, InventoryDialog::OnOk)
EVT_BUTTON(wxID_CANCEL, InventoryDialog::OnCancel)
EVT_BUTTON(InventoryDialog::ID_CALENDAR, InventoryDialog::OnCalendar)
END_EVENT_TABLE()

InventoryDialog::InventoryDialog(Inventories::row_t& row_, wxWindow* parent)
: wxDialog(parent, wxID_ANY, wxEmptyString), row(row_) {
	CreateDialog();
	SizeDialog();
}

bool InventoryDialog::TransferDataFromWindow()
{
	nl::row_value<Inventories::quantity_in>(row) = mQuantityInControl->GetValue();
	std::uint64_t value = 0;
	if (mInvoiceWayBill->GetValue().ToULongLong(&value))
	{
		nl::row_value<Inventories::invoice_way_bill_no>(row) = value;
	}
	else return false;

	nl::date_time_t exp_date;
	try {
		std::string value = mExpiryDate->GetValue().ToStdString();
		if (!value.empty()) {
			exp_date = nl::from_string_date(value);
			nl::row_value<Inventories::date_expiry>(row) = exp_date;
		}
		else return false;
	}catch (std::exception& error) {
			//wxMessageBox(fmt::format("{} is invalid, date format should be in \'YYYY-MM-DD\'"), mExpiryDate->GetValue().ToStdString(), wxICON_ERROR | wxOK);
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
	SetBackgroundColour(*wxWHITE);
	ClearBackground();
	texts[0] = new wxStaticText;
	texts[1] = new wxStaticText;
	texts[2] = new wxStaticText;
	texts[3] = new wxStaticText;
	texts[4] = new wxStaticText;

	texts[0]->Create(this, wxID_ANY, "ENTER INVENTORY", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	texts[0]->SetFont(wxFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

	texts[1]->Create(this, wxID_ANY, "Invoice way bill number: ", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	texts[2]->Create(this, wxID_ANY, "Quantity in: ", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	texts[3]->Create(this, wxID_ANY, "Expiry date: ", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	texts[4]->Create(this, wxID_ANY, "Please enter inventory for product", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	
	mOkCancel[0] = new wxButton;
	mOkCancel[1] = new wxButton;
	mOkCancel[0]->Create(this, wxID_OK, "OK");
	mOkCancel[1]->Create(this, wxID_CANCEL, "Cancel");
	//look for a calender icon
	mCalenderButton = new wxBitmapButton(this, ID_CALENDAR, wxArtProvider::GetBitmap("application"));
	mCalenderButton->SetBackgroundColour(*wxWHITE);

	mQuantityInControl = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200, -1), wxSP_ARROW_KEYS | wxALIGN_LEFT, 0,
		std::numeric_limits<int>::max());
	mExpiryDate = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200, -1));
	mExpiryDate->SetHint("YYYY-MM-DD");
	mInvoiceWayBill = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200, -1));
	mInvoiceWayBill->SetValidator(wxTextValidator{ wxFILTER_DIGITS });
}

void InventoryDialog::SizeDialog()
{
	wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* okCancleSizer = new wxBoxSizer(wxHORIZONTAL);
	wxFlexGridSizer* flexSizer = new wxFlexGridSizer(3,3, 5,5);

	okCancleSizer->AddStretchSpacer();
	okCancleSizer->Add(mOkCancel[0], wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));
	okCancleSizer->Add(mOkCancel[1], wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));

	flexSizer->Add(texts[1], wxSizerFlags().Align(wxLEFT));
	flexSizer->Add(mInvoiceWayBill, wxSizerFlags().Align(wxLEFT).Border(wxALL, 5));
	flexSizer->AddStretchSpacer();

	flexSizer->Add(texts[2], wxSizerFlags().Align(wxLEFT));
	flexSizer->Add(mQuantityInControl, wxSizerFlags().Align(wxLEFT).Border(wxALL, 5));
	flexSizer->AddStretchSpacer();

	flexSizer->Add(texts[3], wxSizerFlags().Align(wxLEFT));
	flexSizer->Add(mExpiryDate, wxSizerFlags().Align(wxLEFT).Border(wxALL, 5));
	flexSizer->Add(mCalenderButton, wxSizerFlags().Align(wxLEFT).Border(wxALL, 5));

	boxSizer->Add(texts[0], wxSizerFlags().Align(wxLEFT).Border(wxALL, 5));
	boxSizer->Add(texts[4], wxSizerFlags().Align(wxLEFT).Border(wxALL, 5));
	boxSizer->Add(flexSizer, wxSizerFlags().Align(wxLEFT).Border(wxALL, 5));
	boxSizer->Add(okCancleSizer, wxSizerFlags().Expand().Border(wxALL, 5));

	topSizer->Add(boxSizer, wxSizerFlags().Expand().Border(wxALL, 5));
	SetSizer(topSizer);
	topSizer->SetSizeHints(this);
	Center();
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
	CalendarDialog dialog(this);
	if (dialog.ShowModal() == wxID_OK)
	{
		mExpiryDate->Clear();
		auto dateTime = dialog.mCalender->GetDate();
		auto tm = dateTime.GetTm();
		mExpiryDate->SetValue(fmt::format("{:d}-{:d}-{:d}", tm.year, tm.mon, tm.mday));
	}

}

BEGIN_EVENT_TABLE(CalendarDialog, wxDialog)
EVT_CALENDAR(CalendarDialog::ID_CALENDAR, OnCalender)
END_EVENT_TABLE()

CalendarDialog::CalendarDialog(wxWindow* parent)
: wxDialog(parent, wxID_ANY, wxT("Pick an expiry date")){
	mCalender = new wxCalendarCtrl(this, ID_CALENDAR, wxDateTime::Now(), wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
	wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
	boxSizer->Add(mCalender, wxSizerFlags().Align(wxCENTER).Border(wxALL, 2));
	SetSizer(boxSizer);
	boxSizer->SetSizeHints(this);
}

void CalendarDialog::OnCalender(wxCalendarEvent& evnt)
{
	if (IsModal()) EndModal(wxID_OK);
	else {
		SetReturnCode(wxID_OK);
		this->Show(false);
	}
}



