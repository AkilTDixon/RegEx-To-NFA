#include "MainFrame.h"
#include <wx/wx.h>
#include <wx/image.h>
#include <wx/bitmap.h>
#include <wx/dcbuffer.h>
#include "Automata.h"
using namespace std;

/*
a(a+(cb)*)

a((a+b)c)*

a(a+c(dc)*)

a(a+(cb)*)*

a(a+c(dc)*b)

((a+b)(a+b))*

a(a+(cb)*c)d

(0(0(0(0(0+1)(0+1))*+1(0(1(0+1)(0+1)*)*0)*)*)*)

(1(0(0(1(0+1)(0+1)*)*0+1(0+1)(0+1)*))*)

((0(0(0(0(0+1)(0+1))*+1(0(1(0+1)(0+1)*)*0)*)*)*)+(1(0(0(1(0+1)(0+1)*)*0+1(0+1)(0+1)*))*))

((a+b)*(aa+bb)(a+b)*(ab+ba)(a+b)*+(ab+ba)(a+b)*(aa+bb)(a+b)*)*

((ab+ba)(a+b)*(aa+bb)(a+b)*)*

(a+b)*(aa+bb)(a+b)*(ab+ba)(a+b)*
*/

bool sanityCheck(string regex);

MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title){



	windowPanel = new wxPanel(this);
	NFAbutton = new wxButton(windowPanel, wxID_ANY, "Generate NFA", wxPoint(150,400), wxSize(100, 35));
	DFAbutton = new wxButton(windowPanel, wxID_ANY, "Convert to DFA", wxPoint(150, 450), wxSize(100, 35));
	minimizeButton = new wxButton(windowPanel, wxID_ANY, "Minimize DFA", wxPoint(150, 500), wxSize(100, 35));
	testInputButton = new wxButton(windowPanel, wxID_ANY, "Test Input", wxPoint(350, 500), wxSize(100, 35));
	textCtrl = new wxTextCtrl(windowPanel, wxID_ANY, "(a+b)*(aa+bb)(a+b)*(ab+ba)(a+b)*", wxPoint(300, 470), wxSize(200, -1));
	resetButton = new wxButton(windowPanel, wxID_ANY, "Reset", wxPoint(350, 550), wxSize(100, 35));
	inputTestCtrl = new wxStaticText(windowPanel, wxID_ANY, "", wxPoint(375, 450), wxSize(200, 10));

	if (!machine)
		resetButton->Enable(false);

	testInputButton->Enable(false);
	DFAbutton->Enable(false);
	minimizeButton->Enable(false);

	testInputButton->Bind(wxEVT_BUTTON, &MainFrame::runInput, this);
	NFAbutton->Bind(wxEVT_BUTTON, &MainFrame::generateNFA, this);
	DFAbutton->Bind(wxEVT_BUTTON, &MainFrame::convertToDFA, this);
	minimizeButton->Bind(wxEVT_BUTTON, &MainFrame::minimizeDFA, this);
	resetButton->Bind(wxEVT_BUTTON, &MainFrame::resetMachine, this);


}

void MainFrame::generateNFA(wxCommandEvent& evt) {

	string regEx = textCtrl->GetValue().ToStdString();
	if (!sanityCheck(regEx))
	{
		inputTestCtrl->SetLabel("ERROR: invalid input text");
		return;
	}
	inputTestCtrl->SetLabel("");

	
	
	if (staticBitmap) {
		staticBitmap->Destroy();
		staticBitmap = nullptr;
	}
	
	textCtrl->ChangeValue("");
	if (machine) {
		machine->reset();
		if (!machine->createNFA(regEx))
		{
			inputTestCtrl->SetLabel("ERROR: invalid input text");
			return;
		}
	}
	else {
		machine = new Automata();
		if (!machine->createNFA(regEx))
		{
			inputTestCtrl->SetLabel("ERROR: invalid input text");
			return;
		}	
	}
	machine->print(this);

	NFAbutton->Enable(false);
	testInputButton->Enable(true);
	DFAbutton->Enable(true);
	resetButton->Enable(true);
	
}

void MainFrame::convertToDFA(wxCommandEvent& evt)
{
	DFAbutton->Enable(false);

	if (staticBitmap) {
		staticBitmap->Destroy();
		staticBitmap = nullptr;
	}

	machine->convertToDFA();
	machine->print(this);

	minimizeButton->Enable(true);
}

void MainFrame::minimizeDFA(wxCommandEvent& evt)
{
	minimizeButton->Enable(false);

	if (staticBitmap) {
		staticBitmap->Destroy();
		staticBitmap = nullptr;
	}

	machine->minimizeDFA();
	machine->print(this);

	
}
void MainFrame::resetMachine(wxCommandEvent& evt)
{

	if (machine)
		machine->reset();
	else
		return;

	if (staticBitmap) {
		staticBitmap->Destroy();
		staticBitmap = nullptr;
	}
	textCtrl->ChangeValue("");
	inputTestCtrl->SetLabel("");
	testInputButton->Enable(false);
	NFAbutton->Enable(true);
	DFAbutton->Enable(false);
	minimizeButton->Enable(false);
	resetButton->Enable(false);
	//textCtrl->Enable(true);
}

void MainFrame::runInput(wxCommandEvent& evt)
{
	string input = textCtrl->GetValue().ToStdString();

	if (machine->input(input)) {
		inputTestCtrl->SetForegroundColour(*wxBLUE);
		inputTestCtrl->SetLabel("Accepted");
		
	}
	else {
		
		inputTestCtrl->SetForegroundColour(*wxRED);
		inputTestCtrl->SetLabel("Rejected");
		
		
	}
}

bool sanityCheck(string regex)
{
	stack<char> bracketCount;
	int bCharCount = 0;

	//check if there's a closing bracket for every opening bracket
	for (char c : regex)
	{
		switch (c)
		{
		case '(':
			bracketCount.push(c);
			break;
		case ')':
			if (bracketCount.empty())
				return false;
			bracketCount.pop();
			break;
		case '~':
			return false;
		}
	}

	if (bracketCount.empty())
		return true;
	else
		return false;

	
	


}