#pragma once
#include <wx/wx.h>


class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title);

	wxTextCtrl* textCtrl;
	wxButton* button;
	wxPanel* windowPanel;
	wxStaticBitmap* staticBitmap;

private:
	void OnButtonClick(wxCommandEvent& evt);
};
