#pragma once
///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/valnum.h>
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class SalesOutput
///////////////////////////////////////////////////////////////////////////////
class SalesOutput : public wxPanel
{
private:

protected:
	wxStaticText* TotalLabel;
	wxTextCtrl* TotalText;
	wxStaticText* DiscountLabel;
	wxTextCtrl* DiscountText;
	wxStaticText* ChangeLabel;
	wxTextCtrl* ChangeText;

	float Total;
	float Discount;
	float Change;
public:

	SalesOutput(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(550, 107), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString);
	~SalesOutput();

	void SetChangeText(const std::string& text);
	void SetTotalText(const std::string& text);
	void SetDiscountText(const std::string& text);
	void ClearOutput();


	std::string GetTotalTextValue() const {
		return TotalText->GetValue().ToStdString();
	}

	std::string GetDiscountTextValue() const {
		return DiscountText->GetValue().ToStdString();
	}

	std::string GetChangeTextValue() const {
		return ChangeText->GetValue().ToStdString();
	}

	float GetTotal() const { return Total;  }
	float GetDiscount() const { return Discount;  }
	float GetChange() const { return Change;  }
};


