#include "common.h"
#include "CreateAccountDialog.h"
BEGIN_EVENT_TABLE(CreateAccountDialog, wxDialog)
EVT_BUTTON(wxID_OK, CreateAccountDialog::OnOK)
EVT_BUTTON(wxID_CANCEL, CreateAccountDialog::OnCancel)
END_EVENT_TABLE()


CreateAccountDialog::CreateAccountDialog(wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize& size, long style)
	: wxDialog(parent, id, wxEmptyString, position, size) {
	CreateDialog();
	SizeDialog();
	SetBackgroundColour(*wxWHITE);
}

void CreateAccountDialog::CreateDialog()
{
	mTitleText = new wxStaticText(this, wxID_ANY, "CREATE ACCOUNT");
	mTitleText->SetFont(wxFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

	mSurnameCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString ,wxDefaultPosition, wxSize(400, -1));
	mSurnameCtrl->SetHint("Surname...");
	mSurnameCtrl->SetToolTip("Enter your surname");
	mSurnameCtrl->SetValidator(wxTextValidator{ wxFILTER_EMPTY });

	mNameCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(400, -1));
	mNameCtrl->SetHint("Name...");
	mNameCtrl->SetToolTip("Enter your other name");
	mNameCtrl->SetValidator(wxTextValidator{ wxFILTER_EMPTY });
	
	mUsernameCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(400, -1));
	mUsernameCtrl->SetHint("username...");
	mUsernameCtrl->SetToolTip("Enter a username that could be used to sign in");
	mUsernameCtrl->SetValidator(wxTextValidator{ wxFILTER_EMPTY });

	mPhoneNumberCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(400, -1));
	mPhoneNumberCtrl->SetHint("Phone number");
	mPhoneNumberCtrl->SetToolTip("Enter your phone number");
	mPhoneNumberCtrl->SetValidator(wxTextValidator{ wxFILTER_EMPTY });

	mPasswordCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(400, -1), wxTE_PASSWORD);
	mPasswordCtrl->SetHint("Password...");
	mPasswordCtrl->SetToolTip("Enter a password");
	mPasswordCtrl->SetValidator(wxTextValidator{ wxFILTER_EMPTY });

	mConfirmPasswordCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(400, -1), wxTE_PASSWORD);
	mConfirmPasswordCtrl->SetHint("Confirm password...");
	mConfirmPasswordCtrl->SetToolTip("Confirm password that was previously enterd");
	mConfirmPasswordCtrl->SetValidator(wxTextValidator{ wxFILTER_EMPTY });

	mOk = new wxButton(this, wxID_OK, "OK");
	mCancel = new wxButton(this, wxID_CANCEL, "Cancel");
}

void CreateAccountDialog::SizeDialog()
{
	wxBoxSizer* outSize = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* okcancelSize = new wxBoxSizer(wxHORIZONTAL);

	sizer->Add(mTitleText, wxSizerFlags().Align(wxLEFT).Border(wxALL,5));
	sizer->Add(mSurnameCtrl, wxSizerFlags().Align(wxLeft).Border(wxALL,5));
	sizer->Add(mNameCtrl, wxSizerFlags().Align(wxLeft).Border(wxALL,5));
	sizer->Add(mUsernameCtrl, wxSizerFlags().Align(wxLeft).Border(wxALL,5));
	sizer->Add(mPhoneNumberCtrl, wxSizerFlags().Align(wxLeft).Border(wxALL,5));
	sizer->Add(mPasswordCtrl, wxSizerFlags().Align(wxLeft).Border(wxALL,5));
	sizer->Add(mConfirmPasswordCtrl, wxSizerFlags().Align(wxLeft).Border(wxALL,5));

	okcancelSize->AddStretchSpacer();
	okcancelSize->Add(mOk, wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL,5));
	okcancelSize->Add(mCancel, wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL,5));

	sizer->Add(okcancelSize, wxSizerFlags().Align(wxLeft).Border(wxALL, 5));
	outSize->Add(sizer, wxSizerFlags().Align(wxCenter).Border(wxALL, 5).Expand());
	SetSizer(outSize);
	outSize->SetSizeHints(this);
}

bool CreateAccountDialog::TransferDataFromWindow()
{
	nl::row_value<Users::id>(mUser) = GenRandomId();
	nl::row_value<Users::name>(mUser) = mNameCtrl->GetValue().ToStdString();
	nl::row_value<Users::surname>(mUser) = mSurnameCtrl->GetValue().ToStdString();
	nl::row_value<Users::username>(mUser) = mUsernameCtrl->GetValue().ToStdString();
	nl::uuid id;
	id.generate();
	nl::row_value<Users::uuid>(mUser) = id;
	if (mPasswordCtrl->GetValue() != mConfirmPasswordCtrl->GetValue()){
		wxMessageBox("Passwords do not match", "CREATE ACCOUNT", wxICON_ERROR | wxOK);
		return false;
	}
	nl::row_value<Users::password_hash>(mUser) = HashUserPassword(mPasswordCtrl->GetValue().ToStdString());
	return true;
}

bool CreateAccountDialog::Validate()
{
	auto iter = UsersInstance::instance().find_on<Users::username>(mUsernameCtrl->GetValue().ToStdString());
	if (iter != UsersInstance::instance().end())
	{
		wxMessageBox("User name already exists, choose another username", "CREATE ACCOUNT", wxICON_INFORMATION | wxOK);
		return false;
	}
	return true;
}

void CreateAccountDialog::OnOK(wxCommandEvent& evt)
{
	if (Validate() && TransferDataFromWindow())
	{
		if (IsModal()) EndModal(wxID_OK);
		else {
			SetReturnCode(wxID_OK);
			this->Show(false);
		}
	}
}

void CreateAccountDialog::OnCancel(wxCommandEvent& evt)
{
	if (wxMessageBox("Are you sure you want to cancel?", "CREATE ACCOUNT", wxICON_INFORMATION | wxYES_NO) != wxYES){
		return;
	}

	if (IsModal()) EndModal(wxID_CANCEL);
	else{
		SetReturnCode(wxID_CANCEL);
		this->Show(false);
	}
}

nl::blob_t CreateAccountDialog::HashUserPassword(const std::string& password)
{
	//change to string type for the hash
	auto hash_string = bcrypt::generateHash(password);
	nl::blob_t hash(hash_string.size());
	std::copy(hash_string.begin(), hash_string.end(), hash.begin());
	return hash;
}


