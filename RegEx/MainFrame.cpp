#include "MainFrame.h"
#include <wx/wx.h>
#include <wx/image.h>
#include <wx/bitmap.h>
#include <wx/dcbuffer.h>
#include "Automata.h"
using namespace std;



MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title){



	windowPanel = new wxPanel(this);
	button = new wxButton(windowPanel, wxID_ANY, "Generate NFA", wxPoint(350,500), wxSize(100, 35));
	textCtrl = new wxTextCtrl(windowPanel, wxID_ANY, "Enter RegEx", wxPoint(300, 470), wxSize(200, -1));

	button->Bind(wxEVT_BUTTON, &MainFrame::OnButtonClick, this);

}

void MainFrame::OnButtonClick(wxCommandEvent& evt) {

	if (staticBitmap)
		staticBitmap->Destroy();

	string regEx = textCtrl->GetValue().ToStdString();

	Automata d(regEx);

	d.convertToDFA();
	d.minimizeDFA();

	d.print();

	wxImage image;
	if (image.LoadFile("output.png"))
	{
		int maxWidth = 800, maxHeight = 450;
		double scaleX = (double)maxWidth / image.GetWidth(), scaleY = (double)maxHeight / image.GetHeight();
		double scale = min(scaleX, scaleY);
		if (scale < 1.0)
			image = image.Scale(image.GetWidth() * scale, image.GetHeight() * scale, wxIMAGE_QUALITY_HIGH);

		wxBitmap bitmap(image);
		int x = (windowPanel->GetSize().GetWidth() - bitmap.GetWidth()) / 2;
		//int y = (windowPanel->GetSize().GetHeight() - bitmap.GetHeight()) / 2;
		staticBitmap = new wxStaticBitmap(windowPanel, wxID_ANY, bitmap, wxPoint(x, 10));

	}
}