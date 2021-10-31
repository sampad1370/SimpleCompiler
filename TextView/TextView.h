#pragma once
#define _CRT_SECURE_NO_WARNINGS 1
#pragma comment(lib,"GameEngine.lib")
#include<Engine.h>
#include"TextDocument.h"
using namespace BaseEngine;



namespace ComponentUI
{
	namespace TextViewComponent
	{
		//Initialize TextView Component
		extern bool InitTextView();
		//Create a TextView Control!
		extern HWND CreateTextView(HWND hwndParent);
#pragma region Setting Definition
		//TextView Window Message defined here.
		#define TXM_Base (WM_USER)
		#define TXM_OpenFile (TXM_Base+0)
		#define TXM_Clear (TXM_Base+1)
		#define TXM_SETLINESPACING	(TXM_Base + 2)
		#define TXM_ADDFONT			(TXM_Base + 3)
		#define TXM_SETCOLOR		(TXM_Base + 5)

		//TextView Message macro defined here
		#define TextView_OpenFile(hwndTextView, szFile)			SendMessage((hwndTextView), TXM_OpenFile, 0, (LPARAM)(szFile))
		#define TextView_Clear(hwndTextView)					SendMessage((hwndTextView), TXM_Clear, 0, 0)
		#define TextView_SetLineSpacing(hwndTV, nAbove, nBelow) SendMessage((hwndTV), TXM_SETLINESPACING, (int) (nAbove), (int) (nBelow))
		#define TextView_AddFont(hwndTV, hFont)					SendMessage((hwndTV), TXM_ADDFONT, (WPARAM) (HFONT) (hFont), 0)
		#define TextView_SetColor(hwndTV, nIdx, rgbColor)		SendMessage((hwndTV), TXM_SETCOLOR, (WPARAM)(nIdx), (LPARAM)(rgbColor))

		#define TextViewClass GL"TextView"

		//TextView Colors
		#define TXC_Background 0 //normal background color
		#define TXC_Foreground 1 //normal foreground color
		#define TXC_HIGHLIGHT		2			// normal background highlight color
		#define TXC_HIGHLIGHTTEXT	3			// normal text highlight color
		#define TXC_HIGHLIGHTTEXT2		4			// inactive text highlight color
		#define TXC_HIGHLIGHT2			5			// inactive background highlight color
		#define TXC_ErrorColor			6			// error color
		#define TXC_ErrorColor2			7			// background error color

		#define TXC_MaxColors 8  //keep this updated!

		#define SYSCOL(COLOR_IDX)   ( 0x80000000 | COLOR_IDX)
		#define SYSCOLIDX(COLREF)   (~0x80000000 & COLREF)

		#define REALIZE_SYSCOL(col) (col & 0x80000000 ? GetSysColor(col & ~0x80000000) : col)

		#define LongestLine 0x100

		// maximum fonts that a TextView can hold
		#define MAX_FONTS 32
		#define TEXTBUFSIZE 32
#pragma endregion


		int		StripCRLF(DChar *szText, int nLength);
		void	PaintRect(HDC hdc, int x, int y, int width, int height, COLORREF fill);

		//	text character attribute
		struct attribute
		{
			COLORREF	fg;			// foreground color
			COLORREF	bg;			// background color
			uint32		style;		// possible font-styling information

		};
		
		struct FONT
		{
			// Windows font information
			HFONT		hFont;
			TEXTMETRIC	tm;

			// dimensions needed for control-character 'bitmaps '
			int			nInternalLeading;
			int			nDescent;

		};

		struct Caret
		{
			int x, y;
			uint PosFile, row, col;
		};

		class TextView
		{
			void PaintLine(HDC hdc, uint32 line);
			//void TabbedExtTextOut(HDC hdc, RECT* rect, DChar* buffer, int length);
			void PaintText(HDC hdc, uint32 nLineNo, RECT *rect);

			int  ApplyTextAttributes(uint32 nLineNo, uint32 offset, DChar *szText, int nTextLen, attribute *attr);
			int  NeatTextOut(HDC hdc, int xpos, int ypos, DChar *szText, int nLen, int nTabOrigin, attribute *attr);

			int  PaintCtrlChar(HDC hdc, int xpos, int ypos, uint32 chValue, FONT *font);
			void InitCtrlCharFontAttr(HDC hdc, FONT *font);

			void RefreshWindow();
			long InvalidateRange(uint32 nStart, uint32 nFinish);

			int  CtrlCharWidth(HDC hdc, uint32 chValue, FONT *fa);
			int  NeatTextWidth(HDC hdc, DChar *buf, int len, int nTabOrigin);
			int	 TabWidth();

			bool  MouseCoordToFilePos(int x, int y, uint32 *pnLineNo, uint32 *pnCharOffset, uint32 *pnFileOffset, int *px);
			uint32  RepositionCaret();

			COLORREF GetColor(uint index);

			void SetupScrollbars();
			void UpdateMetrics();
			void RecalcLineHeight();
			bool PinToBottomCorner();

			void Scroll(int dx, int dy);
			HRGN ScrollRgn(int dx, int dy, bool fReturnUpdateRgn);

			HWND m_hwnd;
			HWND m_Parent;
			
			//font relate data
			FONT m_FontAttr[MAX_FONTS];
			int m_nNumFonts;
			int m_nFontWidth;
			//int m_nFontHeight;
			int m_nMaxAscent;
			int m_nLineHeight;
			int m_nHeightAbove;
			int m_nHeightBelow;

			// Scrollbar related data
			uint32 m_nVScrollPos;
			uint32 m_nVScrollMax;
			int m_nHScrollPos;
			int m_nHScrollMax;

			int m_nLongestLine;
			int m_nWindowLines;
			int m_nMaxWindowLines;
			int m_nWindowColumns;
			int m_nMaxWindowColumns;
			// Display-related data
			int		m_nTabWidthChars;
			uint32	m_nSelectionStart;
			uint32	m_nSelectionEnd;
			uint32	m_nCursorOffset;
			

			COLORREF m_rgbColorList[TXC_MaxColors];

			// Runtime data
			bool	m_fMouseDown;
			uint	m_nScrollTimer;
			int		m_nScrollCounter;

			//File-related data
			uint32 m_nLineCount;
			bool m_ApplyErrorColor = false;
		public:
#define TEXTMATRIX(x, y) *(m_pTextDoc->buffer + (y * m_nWindowColumns) + x)
			TextDocument* m_pTextDoc;
			Caret	m_Caret;
			TextView(HWND hwnd, HWND parent);
			~TextView();

			void SetCaretPosition(uint x, uint y);
			long OnPaint();
			long OnSetFont(HFONT hFont);
			long OnSize(uint nFlags,int width,int height);
			long OnVScroll(uint nSBCode, uint nPos);
			long OnHScroll(uint nSBCode, uint nPos);
			long OnMouseWheel(int nDelta);

			long OnChar(uint nChar, uint nFlags);

			long OnTimer(uint nTimer);

			long OnMouseActivate(HWND hwndTop, uint nHitTest, uint nMessage);
			long OnLButtonDown(uint nFlags, int x, int y);
			long OnLButtonUp(uint nFlags, int x, int y);
			long OnMouseMove(uint nFlags, int x, int y);

			long OnSetFocus(HWND hwndOld);
			long OnKillFocus(HWND hwndNew);
			long OnPast();

			bool OpenFile(DChar* FileName);
			bool ClearFile();

			bool AddFont(HFONT);
			bool SetFont(HFONT, int idx);
			bool SetLineSpacing(int nAbove, int nBelow);
			COLORREF SetColour(uint idx, COLORREF rgbColour);
			bool hasDocument=false;
			void SetErrorArea(int OffsetFile,int length);

			long TextViewWndProc(HWND hwnd, uint msg, WPARAM wparam, LPARAM lparam);
		};
	}
	using namespace ComponentUI::TextViewComponent;
}