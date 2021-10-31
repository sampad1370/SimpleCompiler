#pragma once
#include<Engine.h>
using namespace BaseEngine;
namespace CompilerGUI
{
	

	POINT point{ -1 };

	//mouse button status
	bool MBS[3];

	void HandleFiles(WPARAM wParam)
	{
		// DragQueryFile() takes a LPWSTR for the name so we need a TCHAR string
		TCHAR szName[MAX_PATH];

		// Here we cast the wParam as a HDROP handle to pass into the next functions
		HDROP hDrop = (HDROP) wParam;

		// This functions has a couple functionalities.  If you pass in 0xFFFFFFFF in
		// the second parameter then it returns the count of how many filers were drag
		// and dropped.  Otherwise, the function fills in the szName string array with
		// the current file being queried.
		int count = DragQueryFile(hDrop, 0xFFFFFFFF, szName, MAX_PATH);

		// Here we go through all the files that were drag and dropped then display them
		for (int i = 0; i < count; i++)
		{
			// Grab the name of the file associated with index "i" in the list of files dropped.
			// Be sure you know that the name is attached to the FULL path of the file.
			DragQueryFile(hDrop, i, szName, MAX_PATH);
			dropped.push_back(szName);
			// Bring up a message box that displays the current file being processed
			//MessageBox(GetForegroundWindow(), szName, L"Current file received", MB_OK);
		}

		// Finally, we destroy the HDROP handle so the extra memory
		// allocated by the application is released.
		DragFinish(hDrop);
	}

	LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
	{
		//g_Disable = hwnd != GetForegroundWindow();

		/*if (!g_Disable)
		g_CondVar->notify_all();*/

		switch (msg)
		{
			case WM_ACTIVATEAPP:

			if (wp)
			{
				GlobalObjects::g_Engine->Enable();
				//SetCapture(hwnd);
			}
			else
				GlobalObjects::g_Engine->Disable();
			return 0;
			case WM_LBUTTONDOWN:
			SetCapture(hwnd);
			MBS[0] = true;
			GetCursorPos(&point);
			break;
			case WM_LBUTTONUP:
			ReleaseCapture();
			MBS[0] = false;
			break;
			case WM_MOUSEMOVE:
			if (MBS[0])
			{
				POINT pt;
				GetCursorPos(&pt);
				Bound<int> bn = GlobalObjects::g_MainWindow->GetBoundOfWindow();
				bn.x += pt.x - point.x;
				bn.y += pt.y - point.y;
				MoveWindow(hwnd, GlobalObjects::g_MainWindow->GetBoundOfWindow().x, bn.y,
					GlobalObjects::g_MainWindow->GetBoundOfWindow().width, bn.height, true);
				point = pt;

			}
			break;
			case WM_DROPFILES:
			HandleFiles(wp);
			DroppedFile = true;
			break;
			case WM_DESTROY:
			case WM_CLOSE:
			PostQuitMessage(0);
			return 0;
			default:// Any other messages send to the default message handler as our application won't make use of them.
			return DefWindowProc(hwnd, msg, wp, lp);
		}
		return DefWindowProc(hwnd, msg, wp, lp);
	}
}