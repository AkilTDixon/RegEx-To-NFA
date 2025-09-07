#pragma once
#include <wx/wx.h>
#include <wx/image.h>
#include <wx/bitmap.h>
#include <wx/dcbuffer.h>
#include "Automata.h"

class Automata;

class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title);

	wxStaticText* inputTestCtrl;
	wxTextCtrl* textCtrl;

	wxButton* NFAbutton;
	wxButton* DFAbutton;
	wxButton* minimizeButton;
	wxButton* resetButton;
	wxButton* testInputButton;

	wxPanel* windowPanel;
	wxStaticBitmap* staticBitmap;
	Automata* machine;

private:
	void generateNFA(wxCommandEvent& evt);
	void convertToDFA(wxCommandEvent& evt);
	void minimizeDFA(wxCommandEvent& evt);
	void resetMachine(wxCommandEvent& evt);
	void runInput(wxCommandEvent& evt);
};
