#include "common.h"
#include "SalesOutput.h"

#include <fmt/format.h>

///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////

SalesOutput::SalesOutput(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name) : wxPanel(parent, id, pos, size, style, name)
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer(wxHORIZONTAL);


	bSizer3->Add(0, 0, 1, wxEXPAND, 5);

	TotalLabel = new wxStaticText(this, wxID_ANY, wxT("Total(N)"), wxDefaultPosition, wxDefaultSize, 0);
	TotalLabel->Wrap(-1);
	bSizer3->Add(TotalLabel, 0, wxALL | wxEXPAND, 5);

	TotalText = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxBORDER_RAISED);
	TotalText->SetValidator(wxFloatingPointValidator<float>(2, &Total, wxNUM_VAL_ZERO_AS_BLANK));
	bSizer3->Add(TotalText, 0, wxALL, 5);


	bSizer2->Add(bSizer3, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer(wxHORIZONTAL);


	bSizer4->Add(0, 0, 1, wxEXPAND, 5);

	DiscountLabel = new wxStaticText(this, wxID_ANY, wxT("Discount(%)"), wxDefaultPosition, wxDefaultSize, 0);
	DiscountLabel->Wrap(-1);
	bSizer4->Add(DiscountLabel, 0, wxALL | wxEXPAND, 5);

	DiscountText = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxBORDER_RAISED);

	bSizer4->Add(DiscountText, 0, wxALL, 5);


	bSizer2->Add(bSizer4, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer(wxHORIZONTAL);


	bSizer5->Add(0, 0, 1, wxEXPAND, 5);

	ChangeLabel = new wxStaticText(this, wxID_ANY, wxT("Change(N)"), wxDefaultPosition, wxDefaultSize, 0);
	ChangeLabel->Wrap(-1);
	bSizer5->Add(ChangeLabel, 0, wxALL | wxEXPAND, 5);

	ChangeText = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxBORDER_RAISED);
	ChangeText->SetValidator(wxFloatingPointValidator<float>(2, &Change, wxNUM_VAL_ZERO_AS_BLANK));
	bSizer5->Add(ChangeText, 0, wxALL, 5);


	bSizer2->Add(bSizer5, 1, wxEXPAND, 5);


	bSizer1->Add(bSizer2, 1, wxEXPAND, 5);


	this->SetSizer(bSizer1);
	this->Layout();
}

SalesOutput::~SalesOutput()
{
}

void SalesOutput::SetChangeText(const std::string& text)
{
	//validate that the text is correct and within a certin limit
	ChangeText->SetValue(text);
	ChangeText->Update();
	
}

void SalesOutput::SetTotalText(const std::string& text)
{
	//validate that the text is correct and within a certin limit
	TotalText->SetValue(text);
	TotalText->Update();
}

void SalesOutput::SetDiscountText(const std::string& text)
{
	//validate that the text is correct and within a certin limit
	DiscountText->SetValue(text);
	DiscountText->Update();
}

void SalesOutput::ClearOutput()
{
	TotalText->Clear();
	DiscountText->Clear();
	ChangeText->Clear();
}
