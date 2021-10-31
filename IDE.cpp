#pragma comment(linker, "/STACK:2000000")

#include"TextView\TextView.h"
#include "resource\resource.h"
#include"CompilerSource\Compiler.h"

using namespace CompilerPackage;
using namespace ComponentUI;

#define AppTitle GL"Compiler C++"
#define MinWidth 800
#define MinHeight 600

#define TreeWidth 400

DChar AppName [] = AppTitle;
HWND hwndMain, hwndTextView, hWndToolbar, hStatus,hwndTV,hwndTV2_Prog;

DChar FileName[MAX_PATH];
DChar FileTitle[MAX_PATH];

HIMAGELIST g_hImageList = nullptr;
HINSTANCE g_hInst;
HFONT	  g_hFont = nullptr;

Compiler* g_Compier;
TextView *ptv;

bool ShowOpenFileDlg(HWND hwnd, DChar* pstrFileName, DChar* pstrTitleName)
{
	DChar *szFilter = DAT("CMan Source Files (*.cman)\0*.cman\0C++ Source Files (*.h;*.hpp;*.hxx;*.cpp;*.cxx;*.cc)\0*.h;*.hpp;*.hxx;*.cpp;*.cxx;*.cc\0Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0\0");

	OPENFILENAME ofn = { sizeof(ofn) };

	ofn.hwndOwner = hwnd;
	ofn.hInstance = GetModuleHandle(0);
	ofn.lpstrFilter = szFilter;
	ofn.lpstrFile = pstrFileName;
	ofn.lpstrFileTitle = pstrTitleName;

	ofn.nFilterIndex = 1;
	ofn.nMaxFile = _MAX_PATH;
	ofn.nMaxFileTitle = _MAX_FNAME + _MAX_EXT;

	// flags to control appearance of open-file dialog
	ofn.Flags = OFN_EXPLORER |
		OFN_ENABLESIZING |
		OFN_ALLOWMULTISELECT |
		OFN_FILEMUSTEXIST;

	return GetOpenFileName(&ofn);
}

void ShowAboutDlg(HWND hwndParent)
{
	MessageBox(hwndParent,
		AppTitle DAT("\r\n\r\n"),
		AppTitle,
		MB_OK | MB_ICONINFORMATION
		);
}

void SetWindowFileName(HWND hwnd, DChar *szFileName)
{
	DChar ach[MAX_PATH + sizeof(AppName) +4];

	wsprintf(ach, DAT("%s - %s"), szFileName, AppName);
	SetWindowText(hwnd, ach);
}

bool DoOpenFile(HWND hwnd, DChar *szFileName, DChar *szFileTitle)
{
	if (TextView_OpenFile(hwndTextView, szFileName))
	{
		SetWindowFileName(hwnd, szFileTitle);
		return true;
	}
	else
	{
		MessageBox(hwnd, _T("Error opening file"), AppTitle, MB_ICONEXCLAMATION);
		return false;
	}
}

int PointsToLogical(int nPointSize)
{
	HDC hdc = GetDC(0);
	int nLogSize = -MulDiv(nPointSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	ReleaseDC(0, hdc);

	return nLogSize;
}

HFONT EasyCreateFont(int nPointSize, BOOL fBold, DChar *szFace)
{
	return CreateFont(PointsToLogical(nPointSize),
		0, 0, 0,
		fBold ? FW_BOLD : 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		szFace);
}

//
//	How to process WM_DROPFILES
//
void HandleDropFiles(HWND hwnd, HDROP hDrop)
{
	TCHAR buf[MAX_PATH];
	TCHAR *name;

	if (DragQueryFile(hDrop, 0, buf, MAX_PATH))
	{
		strcpy(FileName, buf);

		name = strrchr(FileName, '\\');
		strcpy(FileTitle, name ? name + 1 : buf);

		DoOpenFile(hwnd, FileName, FileTitle);
	}

	DragFinish(hDrop);
}

HWND CreateToolBars(HWND hw)
{
	
	TBADDBITMAP tbab;
	const int ImageListID = 0;
	const int numButtons = 10;
	const int bitmapSize = 16;

	const DWORD buttonStyles = BTNS_AUTOSIZE;
	hWndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0,
		hw, (HMENU) IDC_MAIN_TOOL, GetModuleHandle(NULL), NULL);
	if (hWndToolbar == NULL)
		MessageBox(hw, "Could not create tool bar.", "Error", MB_OK | MB_ICONERROR);

	// Create the image list.
	HIMAGELIST g_hImageList = ImageList_Create(bitmapSize, bitmapSize, // Dimensions of individual bitmaps.
		ILC_COLOR16 | ILC_MASK, // Ensures transparent background.
		numButtons, 0);
	HBITMAP hbmp = (HBITMAP) LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP2));
	int index = ImageList_Add(g_hImageList, hbmp, (HBITMAP) hbmp);
	DeleteObject(hbmp);
	// Set the image list.
	SendMessage(hWndToolbar, TB_SETIMAGELIST,(WPARAM) ImageListID,(LPARAM) g_hImageList);

	// Load the button images.
	SendMessage(hWndToolbar, TB_LOADIMAGES,	(WPARAM) IDB_STD_SMALL_COLOR,(LPARAM) HINST_COMMCTRL);


	// Send the TB_BUTTONSTRUCTSIZE message, which is required for
	// backward compatibility.
	SendMessage(hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

	tbab.hInst = HINST_COMMCTRL;
	tbab.nID = IDB_STD_SMALL_COLOR;
	SendMessage(hWndToolbar, TB_ADDBITMAP, 0, (LPARAM) &tbab);

	TBBUTTON tbb[]=
	{
		{ 55,ID_FILE_NEW,TBSTATE_ENABLED,TBSTYLE_BUTTON  },
		{ 1, ID_FileOpen, TBSTATE_ENABLED, TBSTYLE_BUTTON },
		{ 42, ID_ReOpen, TBSTATE_ENABLED, TBSTYLE_BUTTON },
		{ 2, ID_FILE_SAVE, TBSTATE_ENABLED, TBSTYLE_BUTTON },
		{ 3, ID_FILE_SAVEAS, TBSTATE_ENABLED, TBSTYLE_BUTTON },
		{ -1, TBSTYLE_FLAT, TBSTYLE_BUTTON, 0 },
		{ 20, ID_Compile, TBSTATE_ENABLED, TBSTYLE_BUTTON },
		{ 22, ID_Build, TBSTATE_ENABLED, TBSTYLE_BUTTON },
		{ 23, ID_Start, TBSTATE_ENABLED, TBSTYLE_BUTTON },

	};

	SendMessage(hWndToolbar, TB_ADDBUTTONS, sizeof(tbb) / sizeof(TBBUTTON), (LPARAM) &tbb);
	SendMessage(hWndToolbar, TB_AUTOSIZE, 0, 0);
	return hWndToolbar;
}

HWND CreateStatus(HWND hw)
{
	int statwidths[] = { 100, 200, 350, MinWidth, -1 };
	hStatus = CreateWindowEx(0, STATUSCLASSNAME, NULL,
		WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0,
		hw, (HMENU) IDC_MAIN_STATUS, GetModuleHandle(NULL), NULL);

	SendMessage(hStatus, SB_SETPARTS, sizeof(statwidths) / sizeof(int), (LPARAM) statwidths);
	SendMessage(hStatus, SB_SETTEXT, 0, (LPARAM)"row:0");
	SendMessage(hStatus, SB_SETTEXT, 1, (LPARAM)"col:0");
	SendMessage(hStatus, SB_SETTEXT, 2, (LPARAM)"...");
	SendMessage(hStatus, SB_SETTEXT, 3, (LPARAM)"...");
	return hStatus;
}

HWND CreateTreeView(HWND hw, HWND &hwndTV = ::hwndTV)
{
	
		RECT rcClient;  // dimensions of client area 
		  // handle to tree-view control 
		// Ensure that the common control DLL is loaded. 
		InitCommonControls();
		// Get the dimensions of the parent window's client area, and create 
		// the tree-view control. 
		GetClientRect(hw, &rcClient);
		hwndTV = CreateWindowEx(TVS_EX_AUTOHSCROLL | TVS_EX_MULTISELECT,
			WC_TREEVIEW,
			TEXT("Tree View"),
			WS_VSCROLL | WS_HSCROLL | WS_CHILD | WS_VISIBLE |WS_BORDER| TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_INFOTIP|TVS_TRACKSELECT|TVS_FULLROWSELECT,
			200,0,0,0,
			hw,
			(HMENU) IDC_TreeView,
			g_hInst,
			NULL);
		// creating image list and put it into the tree control
		//====================================================//
		HIMAGELIST hImageList = ImageList_Create(16, 16, ILC_COLOR16, 2, 10);					  // Macro: 16x16:16bit with 2 pics [array]
		HBITMAP hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_TREE));					  // load the picture from the resource
		ImageList_Add(hImageList, hBitMap, NULL);								      // Macro: Attach the image, to the image list
		DeleteObject(hBitMap);													  // no need it after loading the bitmap
		SendMessage(hwndTV, TVM_SETIMAGELIST, 0, (LPARAM) hImageList); // put it onto the tree control
		// Add the open file, closed file, and document bitmaps. 

		//tvinsert.hParent = NULL;			// top most level no need handle
		//tvinsert.hInsertAfter = TVI_ROOT; // work as root level
		//tvinsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		//tvinsert.item.pszText = "Parent";
		//tvinsert.item.iImage = 0;
		//tvinsert.item.iSelectedImage = 1;
		////	        [+]
		////           | 
		///*			 |--*/Parent = (HTREEITEM) SendMessage(hwndTV, TVM_INSERTITEM, 0, (LPARAM) &tvinsert);
		///*			 |  */Root = Parent;
		///*			 |  */Before = Parent;                   // handle of the before root
		///*			 |  */tvinsert.hParent = Parent;         // handle of the above data
		///*			 |  */tvinsert.hInsertAfter = TVI_LAST;  // below parent
		///*			 |  */tvinsert.item.pszText = "Child 1";
		///*           |    |--[+] */
		///*			 |    |   |  */
		///*			 |    |   |*/ Parent = (HTREEITEM) SendMessage(hwndTV, TVM_INSERTITEM, 0, (LPARAM) &tvinsert);
		///*			 |    |   |*/ tvinsert.hParent = Parent;
		///*			 |    |   |*/ tvinsert.item.pszText = "Child of Child1";
		///*			 |    |   |*/ Parent = (HTREEITEM) SendMessage(hwndTV, TVM_INSERTITEM, 0, (LPARAM) &tvinsert);
		///*           |    |   |-[+]*/
		///*			 |    |	     | */
		///*			 |    |	     | */
		///*			 |    |	  */ tvinsert.hParent = Parent;
		///*			 |    |	  */ tvinsert.hInsertAfter = TVI_LAST;
		///*			 |    |	  */ tvinsert.item.pszText = "Double Click Me!";
		///*			 |    |	  */ tvinsert.item.mask = TVIF_TEXT;
		///*			 |    |	  */ SendMessage(hwndTV, TVM_INSERTITEM, 0, (LPARAM) &tvinsert);
		///*			 |	*/tvinsert.hParent = Before;         // handle of the above data
		///*			 |	*/tvinsert.hInsertAfter = TVI_LAST;  // below parent
		///*			 |	*/tvinsert.item.pszText = "Child 2";
		///*			 |	*/Parent = (HTREEITEM) SendMessage(hwndTV, TVM_INSERTITEM, 0, (LPARAM) &tvinsert);
		///*			 |	*/
		//tvinsert.hParent = NULL;			// top most level no need handle
		//tvinsert.hInsertAfter = TVI_LAST; // work as root level
		//tvinsert.item.pszText = "Parent2";
		//Parent = (HTREEITEM) SendMessage(hwndTV, TVM_INSERTITEM, 0, (LPARAM) &tvinsert);
		return hwndTV;
}

HTREEITEM AddItemToTree(DChar* lpszItem, HTREEITEM Parent, DChar* InfoTip = "", HWND hwndTV=::hwndTV)
{
	TVITEM tvi;
	TVINSERTSTRUCT tvins;
	tvi.mask = TVIF_TEXT | TVIF_IMAGE| TVIF_SELECTEDIMAGE | TVIF_PARAM;

	// Set the text of the item. 
	tvi.pszText = lpszItem;
	tvi.cchTextMax = sizeof(tvi.pszText) / sizeof(tvi.pszText[0]);

	// Assume the item is not a parent item, so give it a 
	// document image. 
	tvi.iImage = 0;
	tvi.iSelectedImage = 1;
	tvi.lParam = (LPARAM)InfoTip;

	tvins.item = tvi;
	tvins.hInsertAfter = Parent;

	// Set the parent item based on the specified level. 
	tvins.hParent = Parent;

	// Add the item to the tree-view control. 
	HTREEITEM hPrev = (HTREEITEM) SendMessage(hwndTV, TVM_INSERTITEM,
		0, (LPARAM) (LPTVINSERTSTRUCT) &tvins);

	return hPrev;
}

void UpdateTreeView(node<DataToken>* node, HTREEITEM Parent)
{
	
	for (int i = node->m_child.size()-1; i >=0; i--)
	{
		auto var = node->m_child[i];
		UpdateTreeView(var, AddItemToTree((char*) var->data.name.c_str(), Parent));
	}
}

//	Main window procedure
LRESULT CALLBACK WndProc(HWND hwnd, uint msg, WPARAM wParam, LPARAM lParam)
{
	static int width, height;
	

	switch (msg)
	{
		case WM_CREATE:
		{
			hwndTextView = CreateTextView(hwnd);
			CreateToolBars(hwnd);
			CreateStatus(hwnd);
			CreateTreeView(hwnd);
			CreateTreeView(hwnd, hwndTV2_Prog);
			// change the font to look like visual-studio 2003
			g_hFont = EasyCreateFont(10, false, "Courier New");

			// change the font
			SendMessage(hwndTextView, WM_SETFONT, (WPARAM) g_hFont, 0);
			TextView_SetLineSpacing(hwndTextView, 0, 1);

			// automatically create new document when we start
			PostMessage(hwnd, WM_COMMAND, ID_FILE_NEW, 0);

			// tell windows that we can handle drag+drop's files
			DragAcceptFiles(hwnd, TRUE);
			ptv = (TextView *) GetWindowLong(hwndTextView, 0);
			//AddItemToTree("test1", AddItemToTree("test2", AddItemToTree("test2", AddItemToTree("test3", AddItemToTree("test3", nullptr)))));
		}
			return 0;

		case WM_DROPFILES:
			HandleDropFiles(hwnd, (HDROP) wParam);
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			DeleteObject(g_hFont);
			return 0;

		case WM_NOTIFY:
		switch (((LPNMHDR) lParam)->code)
		{
			case TVN_GETINFOTIP:
			{
				LPNMTVGETINFOTIP pTip = (LPNMTVGETINFOTIP) lParam;
				HTREEITEM item = pTip->hItem;

				// Get the text for the item.
				TVITEM tvitem;
				tvitem.mask = TVIF_TEXT;
				tvitem.hItem = item;
				tvitem.pszText = new DChar[MAXCHAR];
				tvitem.cchTextMax = 1024;
				TreeView_GetItem(hwndTV, &tvitem);

				// Copy the text to the infotip.
				strcpy(pTip->pszText, (DChar*) tvitem.pszText);
				TreeView_ShowInfoTip(hwndTV, &tvitem);
				break;
			}
		}
		return 0;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case ID_FILE_NEW:
					SetWindowFileName(hwnd, DAT("Untitled"));
					TextView_Clear(hwndTextView);
					return 0;

				case ID_FileOpen://File Open
					// get a filename to open
					if (ShowOpenFileDlg(hwnd, FileName, FileTitle))
					{
						DoOpenFile(hwnd, FileName, FileTitle);
					}

					return 0;
				case ID_ReOpen:
				DoOpenFile(hwnd, FileName, FileTitle);
				return 0;
				case ID_Compile:
				{
					
					if (ptv->m_pTextDoc->GetString()!="")
					{
						g_Compier = new Compiler();
						if (!g_Compier->Compile(ptv->m_pTextDoc->buffer,(char*) ptv->m_pTextDoc->FileName.c_str()))
						{
							SendMessage(hStatus, SB_SETTEXT, 2, (LPARAM)"Compile Failed! :(");
							SendMessage(hStatus, SB_SETTEXT, 3, (LPARAM) (g_Compier->Error.c_str()));
							ptv->SetErrorArea(g_Compier->offset, g_Compier->length);
						}
						else
						{
							SendMessage(hStatus, SB_SETTEXT, 2, (LPARAM)"Compile Successfully! :D");
							SendMessage(hStatus, SB_SETTEXT, 3, (LPARAM) "");
							ptv->OnLButtonDown(0, 0, 0);
							ptv->OnLButtonUp(0, 0, 0);
						}
						int TreeCount = TreeView_GetCount(hwndTV);
						for (int i = 0; i<TreeCount; i++)
							TreeView_DeleteAllItems(hwndTV);
						UpdateTreeView(g_Compier->GetParseTree()->m_head,nullptr);
						//SafeDelete(g_Compier);
					}
				}
				break;

				case ID_FILE_EXIT:
					DestroyWindow(hwnd);
					return 0;

				case ID_EDIT_PAST:
				ptv->OnPast();
				return 0;

				case ID_HELP_ABOUT:
					ShowAboutDlg(hwnd);
					return 0;
			}
			return 0;
		case WM_LBUTTONDOWN:
		{
			
			SendMessage(hStatus, SB_SETTEXT, 0, (LPARAM) ("Row: "+to_string(ptv->m_Caret.row)).c_str());
			SendMessage(hStatus, SB_SETTEXT, 1, (LPARAM) ("Column: " + to_string(ptv->m_Caret.col)).c_str());
			return 0;
		}
		case WM_SETFOCUS:
			SetFocus(hwndTextView);
			return 0;

		case WM_CLOSE:
			DestroyWindow(hwnd);
			return 0;

		case WM_SIZE:		
		{   
			/*width = (short) LOWORD(lParam);
			height = (short) HIWORD(lParam);

			MoveWindow(hwndTextView, 0, 0, width, height, TRUE);*/
			RECT rcTool;
			int iToolHeight;

			HWND hStatus;
			RECT rcStatus;
			int iStatusHeight;

			int iEditHeight;
			RECT rcClient;

			// Size toolbar and get height

			SendMessage(hWndToolbar, TB_AUTOSIZE, 0, 0);

			GetWindowRect(hWndToolbar, &rcTool);
			iToolHeight = rcTool.bottom - rcTool.top;

			// Size status bar and get height

			hStatus = GetDlgItem(hwnd, IDC_MAIN_STATUS);
			SendMessage(hStatus, WM_SIZE, 0, 0);

			GetWindowRect(hStatus, &rcStatus);
			iStatusHeight = rcStatus.bottom - rcStatus.top;


			// Calculate remaining height and size edit

			GetClientRect(hwnd, &rcClient);

			iEditHeight = rcClient.bottom - iToolHeight - iStatusHeight;

			MoveWindow(hwndTV, rcClient.right - TreeWidth, iToolHeight, TreeWidth+2, iEditHeight/2, true);

			MoveWindow(hwndTV2_Prog, rcClient.right - TreeWidth, iToolHeight+iEditHeight / 2+2, TreeWidth + 2, iEditHeight / 2-2, true);
			
			MoveWindow(hwndTextView, 0, iToolHeight, rcClient.right - TreeWidth, iEditHeight, true);
		}
		return 0;
		case WM_SIZING:
		{
			RECT r = *((RECT *) lParam);
			RECT rcClient;
			GetClientRect(hwndTextView, &rcClient);
			if (rcClient.bottom - rcClient.top <= TreeWidth)
			{
				if (r.bottom - r.top <= MinHeight)
					((RECT *) lParam)->bottom = r.top + MinHeight;
			}
			if (r.right - r.left <MinWidth)
				((RECT *) lParam)->right = r.left + MinWidth;
			break;
		}
		/*case WM_PAINT:
		RedrawWindow(hwnd, nullptr, nullptr, RDW_INTERNALPAINT);
		return 0;*/

	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

//	Register main window class
void InitMainWnd()
{
	WNDCLASSEX wcx;
	HINSTANCE hInst = g_hInst;

	// Window class for the main application parent window
	wcx.cbSize = sizeof(wcx);
	wcx.style = 0;
	wcx.lpfnWndProc = WndProc;
	wcx.cbClsExtra = 0;
	wcx.cbWndExtra = 0;
	wcx.hInstance = hInst;
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcx.hbrBackground = (HBRUSH) 0;
	wcx.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wcx.lpszClassName = AppName;
	wcx.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
	wcx.hIconSm = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));

	


	RegisterClassEx(&wcx);
}

//	Create a top-level window
HWND CreateMainWnd()
{
	return CreateWindowEx(0,
		AppName,				// window class name
		AppName,				// window caption
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT,			// initial x position
		CW_USEDEFAULT,			// initial y position
		800,					// initial x size
		600,					// initial y size
		NULL,					// parent window handle
		NULL,					// use window class menu
		GetModuleHandle(0),		// program instance handle
		NULL);					// creation parameters
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, Char* cmdl, int iShowCmd)
{
	MSG msg;
	HACCEL hAccel;
	g_hInst = hInst;
	InitMainWnd();
	InitTextView();

	hwndMain = CreateMainWnd();

	ShowWindow(hwndMain, SW_MAXIMIZE);

	hAccel = LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(hwndMain, hAccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	//EnableMemoryLeakDetector();
	//_CrtDumpMemoryLeaks();
	return 0;
}