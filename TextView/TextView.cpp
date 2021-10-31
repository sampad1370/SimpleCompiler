#include"TextView.h"

namespace ComponentUI
{
	namespace TextViewComponent
	{

#pragma region Global Section

		TextView::TextView(HWND hwnd,HWND parent)
		{
			m_hwnd = hwnd;
			m_Parent = parent;
			m_Caret = { 0, 0, 0 };
			// Font-related data
			m_nNumFonts = 1;
			m_nHeightAbove = 0;
			m_nHeightBelow = 0;

			// File-related data
			m_nLineCount = 0;
			m_nLongestLine = 0;

			// Scrollbar related data
			m_nVScrollPos = 0;
			m_nHScrollPos = 0;
			m_nVScrollMax = 0;
			m_nHScrollMax = 0;

			// Display-related data
			m_nTabWidthChars = 4;

			// Default display colors
			m_rgbColorList[TXC_Foreground] = GetSysColor(COLOR_WINDOWTEXT);
			m_rgbColorList[TXC_Background] = GetSysColor(COLOR_WINDOW);
			m_rgbColorList[TXC_HIGHLIGHTTEXT] = GetSysColor(COLOR_HIGHLIGHTTEXT);
			m_rgbColorList[TXC_HIGHLIGHT] = GetSysColor(COLOR_HIGHLIGHT);
			m_rgbColorList[TXC_HIGHLIGHTTEXT2] = 0xfff0f0f0;
			m_rgbColorList[TXC_HIGHLIGHT2] = 0x00f0f0f0;
			m_rgbColorList[TXC_ErrorColor] = 0xff0000ff;
			m_rgbColorList[TXC_ErrorColor2] = 0x000000ff;

			// Runtime data
			m_fMouseDown = false;
			m_nScrollTimer = 0;

			m_nSelectionStart = 0;
			m_nSelectionEnd = 0;
			m_nCursorOffset = 0;

			// Set the default font
			OnSetFont((HFONT) GetStockObject(ANSI_FIXED_FONT));

			m_pTextDoc = new TextDocument();

			UpdateMetrics();
		}
		TextView::~TextView()
		{
			SafeDelete(m_pTextDoc);
		}

		void TextView::UpdateMetrics()
		{
			RECT rect;
			GetClientRect(m_hwnd, &rect);

			OnSize(0, rect.right, rect.bottom);
			RefreshWindow();
		}

		long TextView::OnSetFocus(HWND hwndOld)
		{
			CreateCaret(m_hwnd, nullptr, 1, m_nLineHeight);
			RepositionCaret();

			ShowCaret(m_hwnd);
			RefreshWindow();
			return 0;
		}

		long TextView::OnKillFocus(HWND hwndNew)
		{
			HideCaret(m_hwnd);
			DestroyCaret();
			RefreshWindow();
			return 0;
		}

		//Function for manage message receive to TextView
		LRESULT TextView::TextViewWndProc(HWND hwnd, uint msg, WPARAM wparam, LPARAM lparam)
		{
			

			switch (msg)
			{
				// First message received by any window - make a new TextView object
				// and store pointer to it in our extra-window-bytes
				case WM_KEYDOWN:
				switch (wparam)
				{
					case VK_DELETE:
					OnChar(0x08, 0);
					break;
				}
				// Draw contents of TextView whenever window needs updating
				case WM_PAINT:
				return OnPaint();

				// Set a new font 
				case WM_SETFONT:
				return OnSetFont((HFONT) wparam);

				case WM_SIZE:
				return OnSize(wparam, LOWORD(lparam), HIWORD(lparam));

				case WM_VSCROLL:
				return OnVScroll(LOWORD(wparam), HIWORD(wparam));

				case WM_HSCROLL:
				return OnHScroll(LOWORD(wparam), HIWORD(wparam));

				case WM_MOUSEACTIVATE:
				return OnMouseActivate((HWND) wparam, LOWORD(lparam), HIWORD(lparam));

				case WM_MOUSEWHEEL:
				return OnMouseWheel((short) HIWORD(wparam));

				case WM_SETFOCUS:
				return OnSetFocus((HWND) wparam);

				case WM_KILLFOCUS:
				return OnKillFocus((HWND) wparam);

				case WM_LBUTTONDOWN:
				{
					auto ret = OnLButtonDown(wparam, (short) LOWORD(lparam), (short) HIWORD(lparam));
					SendMessage(m_Parent, WM_LBUTTONDOWN,wparam,lparam);
					return ret;

				}
				case WM_LBUTTONUP:
				return OnLButtonUp(wparam, (short) LOWORD(lparam), (short) HIWORD(lparam));

				case WM_MOUSEMOVE:
				return OnMouseMove(wparam, (short) LOWORD(lparam), (short) HIWORD(lparam));

				case WM_TIMER:
				return OnTimer(wparam);

				case WM_CHAR:
				return OnChar(wparam, lparam);

				//
				case TXM_OpenFile:
				return OpenFile((TCHAR *) lparam);

				case TXM_Clear:
				return ClearFile();

				case TXM_SETLINESPACING:
				return SetLineSpacing(wparam, lparam);

				case TXM_ADDFONT:
				return AddFont((HFONT) wparam);

				case TXM_SETCOLOR:
				//return SetColor(wparam, lparam);

				default:
				break;
			}

			return DefWindowProc(hwnd, msg, wparam, lparam);
		}

		LRESULT WINAPI TextViewWndProc(HWND hwnd, uint msg, WPARAM wparam, LPARAM lparam)
		{
			TextView *ptv = (TextView *) GetWindowLong(hwnd, 0);

			switch (msg)
			{
				// First message received by any window - make a new TextView object
				// and store pointer to it in our extra-window-bytes
				case WM_NCCREATE:

				if ((ptv = new TextView(hwnd, GetParent(hwnd))) == 0)
					return false;

				SetWindowLong(hwnd, 0, (LONG) ptv);
				return true;

				// Last message received by any window - delete the TextView object
				case WM_NCDESTROY:

				delete ptv;
				return 0;
				default:
				return ptv->TextViewWndProc(hwnd,msg, wparam, lparam);
			}
		}

		//Register the TextView window class.
		bool InitTextView()
		{
			WNDCLASSEX wcx;

			//Window class for the main application parent window
			wcx.cbSize = sizeof(wcx);
			wcx.style = 0;
			wcx.lpfnWndProc = TextViewWndProc;
			wcx.cbClsExtra = 0;
			wcx.cbWndExtra = sizeof(TextView *);
			wcx.hInstance = GetModuleHandle(0);
			wcx.hIcon = 0;
			wcx.hCursor = LoadCursor(NULL, IDC_IBEAM);
			wcx.hbrBackground = (HBRUSH) 0;		//NO FLICKERING FOR US!!
			wcx.lpszMenuName = 0;
			wcx.lpszClassName = TextViewClass;
			wcx.hIconSm = 0;

			return RegisterClassEx(&wcx) ? true : false;
		}

		//Create a TextView Control!
		HWND CreateTextView(HWND hwndParent)
		{
			return CreateWindowEx(WS_EX_CLIENTEDGE, TextViewClass, DAT(""),
				WS_VSCROLL | WS_HSCROLL | WS_CHILD | WS_VISIBLE,
				0, 0, 0, 0, hwndParent, 0, GetModuleHandle(0), 0);
		}

#pragma endregion


#pragma region Scroll TextView
		//
		//	Set scrollbar positions and range
		//
		void TextView::SetupScrollbars()
		{
			SCROLLINFO si = { sizeof(si) };

			si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_DISABLENOSCROLL;

			//
			//	Vertical scrollbar
			//
			si.nPos = m_nVScrollPos;		// scrollbar thumb position
			si.nPage = m_nWindowLines;		// number of lines in a page
			si.nMin = 0;
			si.nMax = m_nLineCount - 1;	// total number of lines in file

			SetScrollInfo(m_hwnd, SB_VERT, &si, TRUE);

			//
			//	Horizontal scrollbar
			//
			si.nPos = m_nHScrollPos;		// scrollbar thumb position
			si.nPage = m_nWindowColumns;	// number of lines in a page
			si.nMin = 0;
			si.nMax = m_nLongestLine - 1;	// total number of lines in file

			SetScrollInfo(m_hwnd, SB_HORZ, &si, TRUE);

			// adjust our interpretation of the max scrollbar range to make
			// range-checking easier. The scrollbars don't use these values, they
			// are for our own use.
			m_nVScrollMax = m_nLineCount - m_nWindowLines;
			m_nHScrollMax = m_nLongestLine - m_nWindowColumns;
		}

		//
		//	Ensure that we never scroll off the end of the file
		//
		bool TextView::PinToBottomCorner()
		{
			bool repos = false;

			if (m_nHScrollPos + m_nWindowColumns > m_nLongestLine)
			{
				m_nHScrollPos = m_nLongestLine - m_nWindowColumns;
				repos = true;
			}

			if (m_nVScrollPos + m_nWindowLines > m_nLineCount)
			{
				m_nVScrollPos = m_nLineCount - m_nWindowLines;
				repos = true;
			}

			return repos;
		}

		//
		//	The window has changed size - update the scrollbars
		//
		long TextView::OnSize(uint nFlags, int width, int height)
		{
			m_nWindowLines = min((unsigned) height / m_nLineHeight, m_nLineCount);
			m_nWindowColumns = min(width / m_nFontWidth, m_nLongestLine);
			m_nMaxWindowColumns = width / m_nFontWidth;
			m_nMaxWindowLines = height / m_nLineHeight;
			if (PinToBottomCorner())
			{
				RefreshWindow();
			}

			SetupScrollbars();

			return 0;
		}

		HRGN TextView::ScrollRgn(int dx, int dy, bool fReturnUpdateRgn)
		{
			RECT clip;

			GetClientRect(m_hwnd, &clip);

			//
			// make sure that dx,dy don't scroll us past the edge of the document!
			//

			// scroll up
			if (dy < 0)
			{
				dy = -(int) min((ULONG) -dy, m_nVScrollPos);
				clip.top = -dy * m_nLineHeight;
			}
			// scroll down
			else if (dy > 0)
			{
				dy = min((ULONG) dy, m_nVScrollMax - m_nVScrollPos);
				clip.bottom = (m_nWindowLines - dy) * m_nLineHeight;
			}


			// scroll left
			if (dx < 0)
			{
				dx = -(int) min(-dx, m_nHScrollPos);
				clip.left = -dx * m_nFontWidth * 4;
			}
			// scroll right
			else if (dx > 0)
			{
				dx = min((unsigned) dx, (unsigned) m_nHScrollMax - m_nHScrollPos);
				clip.right = (m_nWindowColumns - dx - 4) * m_nFontWidth;
			}

			// adjust the scrollbar thumb position
			m_nHScrollPos += dx;
			m_nVScrollPos += dy;

			// perform the scroll
			if (dx != 0 || dy != 0)
			{
				// do the scroll!
				ScrollWindowEx(
					m_hwnd,
					-dx * m_nFontWidth,					// scale up to pixel coords
					-dy * m_nLineHeight,
					NULL,								// scroll entire window
					fReturnUpdateRgn ? &clip : NULL,	// clip the non-scrolling part
					0,
					0,
					SW_INVALIDATE
					);

				SetupScrollbars();

				if (fReturnUpdateRgn)
				{
					RECT client;

					GetClientRect(m_hwnd, &client);

					HRGN hrgnClient = CreateRectRgnIndirect(&client);
					HRGN hrgnUpdate = CreateRectRgnIndirect(&clip);

					// create a region that represents the area outside the
					// clipping rectangle (i.e. the part that is never scrolled)
					CombineRgn(hrgnUpdate, hrgnClient, hrgnUpdate, RGN_XOR);

					DeleteObject(hrgnClient);

					return hrgnUpdate;
				}
			}

			return nullptr;
		}

		//
		//	Scroll viewport in specified direction
		//
		void TextView::Scroll(int dx, int dy)
		{

			ScrollRgn(dx, dy, false);
		}

		long GetTrackPos32(HWND hwnd, int nBar)
		{
			SCROLLINFO si = { sizeof(si), SIF_TRACKPOS };
			GetScrollInfo(hwnd, nBar, &si);
			return si.nTrackPos;
		}

		//
		//	Vertical scrollbar support
		//
		long TextView::OnVScroll(uint nSBCode, uint nPos)
		{
			uint32 oldpos = m_nVScrollPos;

			switch (nSBCode)
			{
				case SB_TOP:
				m_nVScrollPos = 0;
				RefreshWindow();
				break;

				case SB_BOTTOM:
				m_nVScrollPos = m_nVScrollMax;
				RefreshWindow();
				break;

				case SB_LINEUP:
				Scroll(0, -1);
				break;

				case SB_LINEDOWN:
				Scroll(0, 1);
				break;

				case SB_PAGEDOWN:
				Scroll(0, m_nWindowLines);
				break;

				case SB_PAGEUP:
				Scroll(0, -m_nWindowLines);
				break;

				case SB_THUMBPOSITION:
				case SB_THUMBTRACK:

				m_nVScrollPos = GetTrackPos32(m_hwnd, SB_VERT);
				RefreshWindow();

				break;
			}

			if (oldpos != m_nVScrollPos)
			{
				SetupScrollbars();
				RepositionCaret();
			}


			return 0;
		}

		//
		//	Horizontal scrollbar support
		//
		long TextView::OnHScroll(uint nSBCode, uint nPos)
		{
			int oldpos = m_nHScrollPos;

			switch (nSBCode)
			{
				case SB_LEFT:
				m_nHScrollPos = 0;
				RefreshWindow();
				break;

				case SB_RIGHT:
				m_nHScrollPos = m_nHScrollMax;
				RefreshWindow();
				break;

				case SB_LINELEFT:
				Scroll(-1, 0);
				break;

				case SB_LINERIGHT:
				Scroll(1, 0);
				break;

				case SB_PAGELEFT:
				Scroll(-m_nWindowColumns, 0);
				break;

				case SB_PAGERIGHT:
				Scroll(m_nWindowColumns, 0);
				break;

				case SB_THUMBPOSITION:
				case SB_THUMBTRACK:

				m_nHScrollPos = GetTrackPos32(m_hwnd, SB_HORZ);
				RefreshWindow();
				break;
			}

			if (oldpos != m_nHScrollPos)
			{
				SetupScrollbars();
				RepositionCaret();
			}

			return 0;
		}

		long TextView::OnMouseWheel(int nDelta)
		{
#ifndef	SPI_GETWHEELSCROLLLINES	
#define SPI_GETWHEELSCROLLLINES   104
#endif

			int nScrollLines;

			SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &nScrollLines, 0);

			if (nScrollLines <= 1)
				nScrollLines = 3;

			Scroll(0, (-nDelta / 120) * nScrollLines);
			RepositionCaret();

			return 0;
		}
#pragma endregion

#pragma region Font Component TextView

		void PaintRect2(HDC hdc, RECT *rect, COLORREF fill)
		{
			fill = SetBkColor(hdc, fill);

			ExtTextOut(hdc, 0, 0, ETO_OPAQUE, rect, 0, 0, 0);

			SetBkColor(hdc, fill);
		}

		//
		//	Return width of specified control-character
		//
		int TextView::CtrlCharWidth(HDC hdc, ULONG chValue, FONT *font)
		{
			SIZE sz;
			const char *str = (char*) &chValue;
			GetTextExtentPoint32(hdc, str, strlen(str), &sz);
			return sz.cx + 4;
		}

		//
		//	Wrapper for GetTextExtentPoint32. Takes into account
		//	control-characters, tabs etc.
		//
		int TextView::NeatTextWidth(HDC hdc, TCHAR *buf, int len, int nTabOrigin)
		{
			SIZE	sz;
			int		width = 0;

			const int TABWIDTHPIXELS = TabWidth();

			for (int i = 0, lasti = 0; i <= len; i++)
			{
				if (i == len || buf[i] == '\t' || buf[i] < 32)
				{
					GetTextExtentPoint32(hdc, buf + lasti, i - lasti, &sz);
					width += sz.cx;

					if (i < len && buf[i] == '\t')
					{
						width += TABWIDTHPIXELS /*- ((width - nTabOrigin) % TABWIDTHPIXELS)*/;
						lasti = i + 1;
					}
					/*else if (i < len && buf[i] < 32)
					{
						width += CtrlCharWidth(hdc, buf[i], &m_FontAttr[0]);
						lasti = i + 1;
					}*/
				}
			}

			return width;
		}

		//	Manually calculate the internal-leading and descent
		//  values for a font by parsing a small bitmap of a single letter "E"
		//	and locating the top and bottom of this letter.
		void TextView::InitCtrlCharFontAttr(HDC hdc, FONT *font)
		{
			// create a temporary off-screen bitmap
			HDC		hdcTemp = CreateCompatibleDC(hdc);
			HBITMAP hbmTemp = CreateBitmap(font->tm.tmAveCharWidth, font->tm.tmHeight, 1, 1, 0);
			HANDLE  hbmOld = SelectObject(hdcTemp, hbmTemp);
			HANDLE  hfnOld = SelectObject(hdcTemp, font->hFont);

			// black-on-white text
			SetTextColor(hdcTemp, RGB(0, 0, 0));
			SetBkColor(hdcTemp, RGB(255, 255, 255));
			SetBkMode(hdcTemp, OPAQUE);

			TextOut(hdcTemp, 0, 0, "E", 1);

			// give default values just in case the scan fails
			font->nInternalLeading = font->tm.tmInternalLeading;
			font->nDescent = font->tm.tmDescent;
			int y;
			// scan downwards looking for the top of the letter 'E'
			for (y = 0; y < font->tm.tmHeight; y++)
			{
				for (int x = 0; x < font->tm.tmAveCharWidth; x++)
				{
					COLORREF col;

					if ((col = GetPixel(hdcTemp, x, y)) == RGB(0, 0, 0))
					{
						font->nInternalLeading = y;
						y = font->tm.tmHeight;
						break;
					}
				}
			}

			// scan upwards looking for the bottom of the letter 'E'
			for (y = font->tm.tmHeight - 1; y >= 0; y--)
			{
				for (int x = 0; x < font->tm.tmAveCharWidth; x++)
				{
					COLORREF col;

					if ((col = GetPixel(hdcTemp, x, y)) == RGB(0, 0, 0))
					{
						font->nDescent = font->tm.tmHeight - y - 1;
						y = 0;
						break;
					}
				}
			}

			// give larger fonts a thicker border
			if (font->nInternalLeading > 1 && font->nDescent > 1 && font->tm.tmHeight > 18)
			{
				font->nInternalLeading--;
				font->nDescent--;
			}

			// cleanup
			SelectObject(hdcTemp, hbmOld);
			SelectObject(hdcTemp, hfnOld);
			DeleteDC(hdcTemp);
			DeleteObject(hbmTemp);
		}

		//	Display an ASCII control character in inverted colors
		//  to what is currently set in the DC
		int TextView::PaintCtrlChar(HDC hdc, int xpos, int ypos, uint32 chValue, FONT *font)
		{
			SIZE  sz;
			RECT  rect;
			const char *str = (char*) &chValue;

			int yoff = m_nMaxAscent + m_nHeightAbove - font->tm.tmAscent;

			COLORREF fg = GetTextColor(hdc);
			COLORREF bg = GetBkColor(hdc);

			// find out how big the text will be
			GetTextExtentPoint32(hdc, str, strlen(str), &sz);
			SetRect(&rect, xpos, ypos, xpos + sz.cx + 4, ypos + m_nLineHeight);

			// paint the background white
			PaintRect2(hdc, &rect, bg);

			// adjust rectangle for first black block
			rect.right -= 1;
			rect.top += font->nInternalLeading + yoff;
			rect.bottom = rect.top + font->tm.tmHeight - font->nDescent - font->nInternalLeading;

			// paint the first black block
			PaintRect2(hdc, &rect, fg);

			// prepare device context
			fg = SetTextColor(hdc, bg);
			bg = SetBkColor(hdc, fg);

			// paint the text and the second "black" block at the same time
			InflateRect(&rect, -1, 1);
			ExtTextOut(hdc, xpos + 1, ypos + yoff, ETO_OPAQUE | ETO_CLIPPED, &rect, str, strlen(str), 0);

			// restore device context
			SetTextColor(hdc, fg);
			SetBkColor(hdc, bg);

			return sz.cx + 4;
		}

		//	Update the line height based on current font settings
		void TextView::RecalcLineHeight()
		{
			m_nLineHeight = 0;
			m_nMaxAscent = 0;

			// find the tallest font in the TextView
			for (int i = 0; i < m_nNumFonts; i++)
			{
				// always include a font's external-leading
				int fontheight = m_FontAttr[i].tm.tmHeight +
					m_FontAttr[i].tm.tmExternalLeading;

				m_nLineHeight = max(m_nLineHeight, fontheight);
				m_nMaxAscent = max(m_nMaxAscent, m_FontAttr[i].tm.tmAscent);
			}

			// add on the above+below spacing
			m_nLineHeight += m_nHeightAbove + m_nHeightBelow;

			// force caret resize if we've got the focus
			if (GetFocus() == m_hwnd)
			{
				OnKillFocus(0);
				OnSetFocus(0);
			}
		}

		//	Set a font for the TextView
		bool TextView::SetFont(HFONT hFont, int idx)
		{
			FONT *font = &m_FontAttr[idx];

			// need a DC to query font data
			HDC    hdc = GetDC(0);
			HANDLE hold = SelectObject(hdc, hFont);

			// get font settings
			font->hFont = hFont;
			GetTextMetrics(hdc, &font->tm);

			// pre-calc the control-characters for this font
			InitCtrlCharFontAttr(hdc, font);

			// cleanup
			SelectObject(hdc, hold);
			ReleaseDC(0, hdc);

			RecalcLineHeight();

			return 0;
		}

		//	Add a secondary font to the TextView
		bool TextView::AddFont(HFONT hFont)
		{
			int idx = m_nNumFonts++;

			SetFont(hFont, idx);
			UpdateMetrics();

			return 0;
		}

		//	WM_SETFONT handler: set a new default font
		long TextView::OnSetFont(HFONT hFont)
		{
			// default font is always #0
			SetFont(hFont, 0);

			m_nFontWidth = m_FontAttr[0].tm.tmAveCharWidth;
			UpdateMetrics();

			return 0;
		}

		//	Set spacing (in pixels) above and below each line - 
		//  this is in addition to the external-leading of a font
		bool TextView::SetLineSpacing(int nAbove, int nBelow)
		{
			m_nHeightAbove = nAbove;
			m_nHeightBelow = nBelow;
			RecalcLineHeight();
			return true;
		}

		int TextView::TabWidth()
		{
			return m_nTabWidthChars * m_nFontWidth;
		}

#pragma endregion


#pragma region Paint Component TextView

		void TextView::RefreshWindow()
		{
			InvalidateRect(m_hwnd, nullptr, false);
		}

		//Painting function for TextView Objects
		long TextView::OnPaint()
		{
			PAINTSTRUCT ps;

			ULONG i, first, last;

			BeginPaint(m_hwnd, &ps);

			// figure out which lines to redraw
			first = m_nVScrollPos + ps.rcPaint.top / m_nLineHeight;
			last = m_nVScrollPos + ps.rcPaint.bottom / m_nLineHeight;

			// make sure we never wrap around the 4gb boundary
			if (last < first) last = -1;

			// draw the display line-by-line
			for (i = first; i <= last; i++)
			{
				PaintLine(ps.hdc, i);
			}

			EndPaint(m_hwnd, &ps);

			return 0;
		}

		//	Draw the specified line
		void TextView::PaintLine(HDC hdc, uint32 nLineNo)
		{
			RECT  rect;

			// Get the area we want to update
			GetClientRect(m_hwnd, &rect);

			// calculate rectangle for entire length of line in window
			rect.left = (long) (-m_nHScrollPos * m_nFontWidth);
			rect.top = (long) ((nLineNo - m_nVScrollPos) * m_nLineHeight);
			rect.right = (long) (rect.right);
			rect.bottom = (long) (rect.top + m_nLineHeight);

			//
			//	do things like margins, line numbers etc. here
			//

			//
			//	check we have data to draw on this line
			//
			if (nLineNo >= m_nLineCount)
			{
				SetBkColor(hdc, GetColor(TXC_Background));
				ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rect, 0, 0, 0);
				return;
			}

			//
			//	paint the line's text
			//
			PaintText(hdc, nLineNo, &rect);
		}

		//	Draw a line of text into the TextView window
		void TextView::PaintText(HDC hdc, uint32 nLineNo, RECT *rect)
		{
			TCHAR		buff[TEXTBUFSIZE];
			attribute		attr[TEXTBUFSIZE];

			ULONG		charoff = 0;
			int			len;

			int			xpos = rect->left;
			int			ypos = rect->top;
			int			xtab = rect->left;

			//
			//	TODO: Clip text to left side of window
			//


			//
			//	Keep drawing until we reach the edge of the window
			//
			while (xpos < rect->right)
			{
				ULONG fileoff;
				int	  lasti = 0;
				int   i;

				//
				//	Get a block of text for drawing
				//
				if ((len = m_pTextDoc->getline(nLineNo, charoff, buff, TEXTBUFSIZE, &fileoff)) == 0)
					break;

				// ready for the next block of characters (do this before stripping CR/LF)
				fileoff += charoff;
				charoff += len;


				//
				//	Apply text attributes - 
				//	i.e. syntax highlighting, mouse selection colours etc.
				//
				//len = ApplyTextAttributes(nLineNo, fileoff+charoff, buff, len, attr);
				len = ApplyTextAttributes(nLineNo, fileoff, buff, len, attr);

				//
				//	Display the text by breaking it into spans of colour/style
				//
				for (i = 0; i <= len; i++)
				{
					// if the colour or font changes, then need to output 
					if (i == len ||
						attr[i].fg != attr[lasti].fg ||
						attr[i].bg != attr[lasti].bg ||
						attr[i].style != attr[lasti].style)
					{
						xpos += NeatTextOut(hdc, xpos, ypos, buff + lasti, i - lasti, xtab, &attr[lasti]);

						lasti = i;
					}
				}
			}

			//
			// Erase to the end of the line
			//
			rect->left = xpos;
			SetBkColor(hdc, GetColor(TXC_Background));
			ExtTextOut(hdc, 0, 0, ETO_OPAQUE, rect, 0, 0, 0);
		}

		//	Apply visual-styles to the text by returning color and font
		//	information into the supplied TEXT_ATTR structure
		//
		//	nLineNo	- line number
		//	nOffset	- actual offset of line within file
		//
		//	Returns new length of buffer if text has been modified
		int TextView::ApplyTextAttributes(uint32 nLineNo, uint32 nOffset, DChar *szText, int nTextLen, attribute *attr)
		{
			int	font = nLineNo % m_nNumFonts;
			int i;

			ULONG selstart = min(m_nSelectionStart, m_nSelectionEnd);
			ULONG selend = max(m_nSelectionStart, m_nSelectionEnd);

			//
			//	TODO: 1. Apply syntax colouring first of all
			//

			//
			//	TODO: 2. Apply bookmarks, line highlighting etc (override syntax colouring)
			//

			//
			//	STEP 3:  Now apply text-selection (overrides everything else)
			//
			for (i = 0; i < nTextLen; i++)
			{
				// apply highlight colors 
				if (nOffset + i >= selstart && nOffset + i < selend)
				{
					if (m_ApplyErrorColor)
					{
						if (GetFocus() == m_hwnd)
						{
							attr[i].fg = GetColor(TXC_ErrorColor);
							attr[i].bg = GetColor(TXC_ErrorColor2);
						}
						else
						{
							attr[i].fg = GetColor(TXC_HIGHLIGHTTEXT2);
							attr[i].bg = GetColor(TXC_HIGHLIGHT2);
						}
					}
					else if (GetFocus() == m_hwnd)
					{
						attr[i].fg = GetColor(TXC_HIGHLIGHTTEXT);
						attr[i].bg = GetColor(TXC_HIGHLIGHT);
					}
					else
					{
						attr[i].fg = GetColor(TXC_HIGHLIGHTTEXT2);
						attr[i].bg = GetColor(TXC_HIGHLIGHT2);
					}
				}
				// normal text colours
				else
				{
					attr[i].fg = GetColor(TXC_Foreground);
					attr[i].bg = GetColor(TXC_Background);
				}

				if (szText[i] == ' ')
					font = (font + 1) % m_nNumFonts;

				attr[i].style = font;
			}

			//
			//	Turn any CR/LF at the end of a line into a single 'space' character
			//
			return StripCRLF(szText, nTextLen);
		}

		//	Draw tabbed text using specified colors and font, return width of output text
		//
		//	We could just use TabbedTextOut - however, we need to parse the text because
		//  it might contain ASCII-control characters which must be output separately.
		//  because of this we'll just handle the tabs at the same time.
		int TextView::NeatTextOut(HDC hdc, int xpos, int ypos, DChar *szText, int nLen, int nTabOrigin, attribute *attr)
		{
			int   i;
			int   xold = xpos;
			int   lasti = 0;
			SIZE  sz;

			const int TABWIDTHPIXELS = m_nTabWidthChars * m_nFontWidth;

			FONT *font = &m_FontAttr[attr->style];

			// configure the device context
			SetTextColor(hdc, attr->fg);
			SetBkColor(hdc, attr->bg);
			SelectObject(hdc, font->hFont);

			// loop over each character
			for (i = 0; i <= nLen; i++)
			{
				int  yoff = m_nMaxAscent + m_nHeightAbove - font->tm.tmAscent;

				// output any "deferred" text before handling tab/control chars
				if (i == nLen || szText[i] == '\t' || szText[i] < 32)
				{
					RECT rect;

					// get size of text
					GetTextExtentPoint32(hdc, szText + lasti, i - lasti, &sz);
					SetRect(&rect, xpos, ypos, xpos + sz.cx, ypos + m_nLineHeight);

					// draw the text and erase it's background at the same time
					ExtTextOut(hdc, xpos, ypos + yoff, ETO_CLIPPED | ETO_OPAQUE, &rect, szText + lasti, i - lasti, 0);

					xpos += sz.cx;
				}

				// special case for TAB and Control characters
				if (i < nLen)
				{
					// TAB characters
					if (szText[i] == '\t')
					{
						// calculate distance in pixels to the next tab-stop
						int width = TABWIDTHPIXELS /*- ((xpos - nTabOrigin) % TABWIDTHPIXELS)*/;

						// draw a blank space 
						PaintRect(hdc, xpos, ypos, width, m_nLineHeight, attr->bg);

						xpos += width;
						lasti = i + 1;
					}
					// ASCII-CONTROL characters
					else if (szText[i] < 32)
					{
						xpos += PaintCtrlChar(hdc, xpos, ypos, szText[i], font);
						lasti = i + 1;
					}
				}
			}

			// return the width of text drawn
			return xpos - xold;
		}

		void PaintRect(HDC hdc, int x, int y, int width, int height, COLORREF fill)
		{
			RECT rect = { x, y, x + width, y + height };

			fill = SetBkColor(hdc, fill);
			ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rect, 0, 0, 0);
			SetBkColor(hdc, fill);
		}

		//	Strip CR/LF combinations from the end of a line and
		//  replace with a single space character (for drawing purposes)
		int StripCRLF(DChar *szText, int nLength)
		{
			if (nLength >= 2)
			{
				if (szText[nLength - 2] == '\r' && szText[nLength - 1] == '\n')
				{
					szText[nLength - 2] = ' ';
					return --nLength;
				}
			}

			if (nLength >= 1)
			{
				if (szText[nLength - 1] == '\r' || szText[nLength - 1] == '\n')
				{
					szText[nLength - 1] = ' ';
					nLength--;
				}
			}

			return nLength;
		}

		//Emulates ExtTextOut, but draws text using tabs using TabbedTextOut
		/*void TextView::TabbedExtTextOut(HDC hdc, RECT *rect, DChar *buf, int len)
		//{
		//	int  tab = 4 * m_nFontWidth;
		//	int  width;
		//	RECT fill = *rect;

		//	// Draw line and expand tabs
		//	width = TabbedTextOut(hdc, rect->left, rect->top, buf, len, 1, &tab, rect->left);

		//	// Erase the rest of the line with the background color
		//	fill.left += LOWORD(width);
		//	ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &fill, 0, 0, 0);
		//}*/

		//	Return an RGB value corresponding to the specified HVC_xxx index
		COLORREF TextView::GetColor(uint index)
		{
			return m_rgbColorList[index];
			/*switch (index)
			{
				case TXC_Background: return GetSysColor(COLOR_WINDOW);
				case TXC_Foreground: return GetSysColor(COLOR_WINDOWTEXT);
				case TXC_HIGHLIGHT:		return GetSysColor(COLOR_HIGHLIGHT);
				case TXC_HIGHLIGHTTEXT:	return GetSysColor(COLOR_HIGHLIGHTTEXT);
				default:			 return 0;
			}*/
		}

		void TextView::SetErrorArea(int OffsetFile,int length)
		{
			ULONG	nLineNo, nLineOff;
			m_pTextDoc->offset_to_line(OffsetFile, &nLineNo, &nLineOff);
			SetCaretPosition(nLineNo, nLineOff);
			InvalidateRange(m_nSelectionStart, m_nSelectionEnd);
			m_Caret.PosFile = OffsetFile;
			m_Caret.row = nLineNo;
			m_Caret.col = nLineOff;
			m_nSelectionStart = OffsetFile;
			m_nSelectionEnd = OffsetFile + length;
			m_nCursorOffset = OffsetFile;
			RefreshWindow();
			m_ApplyErrorColor = true;
			// adjust the cursor + selection to the new offset
			//SetCaretPosition(nLineNo, nLineOff + length);
		}

#pragma endregion


#pragma region File Component TextView

		//
		bool TextView::OpenFile(DChar *FileName)
		{
			ClearFile();

			if (m_pTextDoc->init(FileName))
			{
				m_nLineCount = m_pTextDoc->linecount();
				m_nLongestLine = m_pTextDoc->longestline(4);

				m_nVScrollPos = 0;
				m_nHScrollPos = 0;

				m_nSelectionStart = 0;
				m_nSelectionEnd = 0;
				m_nCursorOffset = 0;

				UpdateMetrics();
				hasDocument = true;
				return true;
			}
			return false;
		}

		//
		bool TextView::ClearFile()
		{
			if (m_pTextDoc)
				m_pTextDoc->clear();

			m_nLineCount = m_pTextDoc->linecount();
			m_nLongestLine = m_pTextDoc->longestline(4);

			m_nVScrollPos = 0;
			m_nHScrollPos = 0;

			UpdateMetrics();

			return true;
		}

#pragma endregion

#pragma region Mouse Control
		void TextView::SetCaretPosition(uint x, uint y)
		{
			m_Caret.x = x;
			m_Caret.y = y;
			SetCaretPos(x, y);
		}
		int ScrollDir(int counter, int distance)
		{
			int amt;

			// amount to scroll based on distance of mouse from window
			if (abs(distance) < 16)			amt = 8;
			else if (abs(distance) < 48)		amt = 3;
			else							amt = 1;

			if (counter % amt == 0)
				return distance < 0 ? -1 : 1;
			else
				return 0;
		}

		//
		//	WM_MOUSEACTIVATE
		//
		//	Grab the keyboard input focus 
		//	
		long TextView::OnMouseActivate(HWND hwndTop, UINT nHitTest, UINT nMessage)
		{
			SetFocus(m_hwnd);
			return MA_ACTIVATE;
		}

		//
		//	WM_LBUTTONDOWN
		//
		//  Position caret to nearest text character under mouse
		//
		long TextView::OnLButtonDown(UINT nFlags, int mx, int my)
		{
			ULONG nLineNo;
			ULONG nCharOff;
			ULONG nFileOff;
			int   px;

			// map the mouse-coordinates to a real file-offset-coordinate
			MouseCoordToFilePos(mx, my, &nLineNo, &nCharOff, &nFileOff, &px);

			m_Caret.row = nLineNo;
			m_Caret.col = nCharOff;
			SetCaretPosition(px, (nLineNo - m_nVScrollPos) * m_nLineHeight);

			// remove any existing selection
			InvalidateRange(m_nSelectionStart, m_nSelectionEnd);

			// reset cursor and selection offsets to the same location
			m_nSelectionStart = nFileOff;
			m_nSelectionEnd = nFileOff;
			m_nCursorOffset = nFileOff;
			m_ApplyErrorColor = false;
			// set capture for mouse-move selections
			m_fMouseDown = true;
			SetCapture(m_hwnd);
			//RefreshWindow();
			m_Caret.PosFile = nFileOff;
			return 0;
		}

		//
		//	WM_LBUTTONUP 
		//
		//	Release capture and cancel any mouse-scrolling
		//
		long TextView::OnLButtonUp(UINT nFlags, int mx, int my)
		{
			if (m_fMouseDown)
			{
				// cancel the scroll-timer if it is still running
				if (m_nScrollTimer != 0)
				{
					KillTimer(m_hwnd, m_nScrollTimer);
					m_nScrollTimer = 0;
				}

				m_fMouseDown = false;
				ReleaseCapture();
			}

			return 0;
		}

		//
		//	WM_MOUSEMOVE
		//
		//	Set the selection end-point if we are dragging the mouse
		//
		long TextView::OnMouseMove(UINT nFlags, int mx, int my)
		{
			if (m_fMouseDown)
			{
				ULONG	nLineNo, nCharOff, nFileOff;

				RECT	rect;
				POINT	pt = { mx, my };
				int		cx;					// caret coordinates

				// get the non-scrolling area (an even no. of lines)
				GetClientRect(m_hwnd, &rect);
				rect.bottom -= rect.bottom % m_nLineHeight;

				// If mouse is within this area, we don't need to scroll
				if (PtInRect(&rect, pt))
				{
					if (m_nScrollTimer != 0)
					{
						KillTimer(m_hwnd, m_nScrollTimer);
						m_nScrollTimer = 0;
					}
				}
				// If mouse is outside window, start a timer in
				// order to generate regular scrolling intervals
				else
				{
					if (m_nScrollTimer == 0)
					{
						m_nScrollCounter = 0;
						m_nScrollTimer = SetTimer(m_hwnd, 1, 10, 0);
					}
				}

				// get new cursor offset+coordinates
				MouseCoordToFilePos(mx, my, &nLineNo, &nCharOff, &nFileOff, &cx);

				// update the region of text that has changed selection state
				if (m_nSelectionEnd != nFileOff)
				{
					// redraw from old selection-pos to new position
					InvalidateRange(m_nSelectionEnd, nFileOff);

					// adjust the cursor + selection to the new offset
					m_nSelectionEnd = nFileOff;
					m_nCursorOffset = nFileOff;
					//m_ApplyErrorColor = false;
				}
				m_Caret.PosFile = nFileOff;
				m_Caret.row = nLineNo;
				m_Caret.col = nCharOff;
				// always set the caret position because we might be scrolling
				SetCaretPosition(cx, (nLineNo - m_nVScrollPos) * m_nLineHeight);
			}

			return 0;
		}

		long TextView::OnTimer(uint nTimerId)
		{
			int	  dx = 0, dy = 0;	// scrolling vectors
			RECT  rect;
			POINT pt;

			// find client area, but make it an even no. of lines
			GetClientRect(m_hwnd, &rect);
			rect.bottom -= rect.bottom % m_nLineHeight;

			// get the mouse's client-coordinates
			GetCursorPos(&pt);
			ScreenToClient(m_hwnd, &pt);

			//
			// scrolling up / down??
			//
			if (pt.y < 0)
				dy = ScrollDir(m_nScrollCounter, pt.y);

			else if (pt.y >= rect.bottom)
				dy = ScrollDir(m_nScrollCounter, pt.y - rect.bottom);

			//
			// scrolling left / right?
			//
			if (pt.x < 0)
				dx = ScrollDir(m_nScrollCounter, pt.x);

			else if (pt.x > rect.right)
				dx = ScrollDir(m_nScrollCounter, pt.x - rect.right);

			//
			// Scroll the window but don't update any invalid
			// areas - we will do this manually after we have 
			// repositioned the caret
			//
			HRGN hrgnUpdate = ScrollRgn(dx, dy, true);

			//
			// do the redraw now that the selection offsets are all 
			// pointing to the right places and the scroll positions are valid.
			//
			if (hrgnUpdate != NULL)
			{
				// We perform a "fake" WM_MOUSEMOVE for two reasons:
				//
				// 1. To get the cursor/caret/selection offsets set to the correct place
				//    *before* we redraw (so everything is synchronized correctly)
				//
				// 2. To invalidate any areas due to mouse-movement which won't
				//    get done until the next WM_MOUSEMOVE - and then it would
				//    be too late because we need to redraw *now*
				//
				OnMouseMove(0, pt.x, pt.y);

				// invalidate the area returned by ScrollRegion
				InvalidateRgn(m_hwnd, hrgnUpdate, FALSE);
				DeleteObject(hrgnUpdate);

				// the next time we process WM_PAINT everything 
				// should get drawn correctly!!
				UpdateWindow(m_hwnd);
			}

			// keep track of how many WM_TIMERs we process because
			// we might want to skip the next one
			m_nScrollCounter++;

			return 0;
		}

		//
		//	Convert mouse(client) coordinates to a file-relative offset
		//
		//	Currently only uses the main font so will not support other
		//	fonts introduced by syntax highlighting
		//
		bool TextView::MouseCoordToFilePos( int		 mx,			// [in]  mouse x-coord
											int		 my,			// [in]  mouse x-coord
											ULONG	*pnLineNo,		// [out] line number
											ULONG	*pnCharOffset,	// [out] char-offset from start of line
											ULONG	*pfnFileOffset, // [out] zero-based file-offset
											int		*px				// [out] adjusted x coord of caret
											)
		{
			ULONG nLineNo;

			ULONG charoff = 0;
			ULONG fileoff = 0;

			TCHAR buf[TEXTBUFSIZE];
			int   len;
			int	  curx = 0;
			RECT  rect;

			// get scrollable area
			GetClientRect(m_hwnd, &rect);
			rect.bottom -= rect.bottom % m_nLineHeight;

			// clip mouse to edge of window
			if (mx < 0)				mx = 0;
			if (my < 0)				my = 0;
			if (my >= rect.bottom)	my = rect.bottom - 1;
			if (mx >= rect.right)	mx = rect.right - 1;

			// It's easy to find the line-number: just divide 'y' by the line-height
			nLineNo = (my / m_nLineHeight) + m_nVScrollPos;

			// make sure we don't go outside of the document
			if (nLineNo >= m_nLineCount)
			{
				nLineNo = m_nLineCount ? m_nLineCount - 1 : 0;
				fileoff = m_pTextDoc->size();
			}

			HDC    hdc = GetDC(m_hwnd);
			HANDLE hOldFont = SelectObject(hdc, m_FontAttr[0].hFont);

			*pnCharOffset = 0;

			mx += m_nHScrollPos * m_nFontWidth;

			// character offset within the line is more complicated. We have to 
			// parse the text.
			for (;;)
			{
				// grab some text
				if ((len = m_pTextDoc->getline(nLineNo, charoff, buf, TEXTBUFSIZE, &fileoff)) == 0)
					break;

				int tlen = len;

				if (len > 1 && buf[len - 2] == '\r')
				{
					buf[len - 2] = 0;
					len -= 2;
				}

				// find it's width
				int width = NeatTextWidth(hdc, buf, len, -(curx % TabWidth()));

				// does cursor fall within this segment?
				if (mx >= curx && mx < curx + width)
				{
					//
					//	We have a range of text, with the mouse 
					//  somewhere in the middle. Perform a "binary chop" to
					//  locate the exact character that the mouse is positioned over
					//
					int low = 0;
					int high = len;
					int lowx = 0;
					int highx = width;

					while (low < high - 1)
					{
						int newlen = (high - low) / 2;

						width = NeatTextWidth(hdc, buf + low, newlen, -lowx - curx);

						if (mx - curx < width + lowx)
						{
							high = low + newlen;
							highx = lowx + width;
						}
						else
						{
							low = low + newlen;
							lowx = lowx + width;
						}
					}

					// base coordinates on centre of characters, not the edges
					if (mx - curx > highx - m_nFontWidth / 2)
					{
						curx += highx;
						charoff += high;
					}
					else
					{
						curx += lowx;
						charoff += low;
					}

					*pnCharOffset = charoff;
					break;
				}

				curx += width;
				charoff += tlen;
				*pnCharOffset += len;
			}

			SelectObject(hdc, hOldFont);
			ReleaseDC(m_hwnd, hdc);


			*pnLineNo = nLineNo;
			//*pnCharOffset=charoff;
			*pfnFileOffset = fileoff + *pnCharOffset;
			*px = curx - m_nHScrollPos *m_nFontWidth;

			return 0;
		}

		//
		//	Redraw the specified range of text/data in the control
		//
		long TextView::InvalidateRange(ULONG nStart, ULONG nFinish)
		{
			ULONG start = min(nStart, nFinish);
			ULONG finish = max(nStart, nFinish);

			ULONG lineno, charoff, linelen, lineoff;
			int   xpos1 = 0, xpos2 = 0;
			int  ypos;

			HDC hdc = GetDC(m_hwnd);
			SelectObject(hdc, m_FontAttr[0].hFont);

			RECT rect, client;
			GetClientRect(m_hwnd, &client);

			// nothing to do?
			if (start == finish)
				return 0;

			//
			//	Find the line number and character offset of specified file-offset
			//
			if (!m_pTextDoc->offset_to_line(start, &lineno, &charoff))
				return 0;

			// clip to top of window
			if (lineno < m_nVScrollPos)
			{
				lineno = m_nVScrollPos;
				charoff = 0;
				xpos1 = 0;
			}

			if (!m_pTextDoc->getlineinfo(lineno, &lineoff, &linelen))
				return 0;

			ypos = (lineno - m_nVScrollPos) * m_nLineHeight;

			// selection starts midline...
			if (charoff != 0)
			{
				ULONG off = 0;
				TCHAR buf[TEXTBUFSIZE];
				int   len = charoff;
				int   width = 0;

				// loop until we get on-screen
				while (off < charoff)
				{
					int tlen = min(len, TEXTBUFSIZE);
					tlen = m_pTextDoc->getline(lineno, off, buf, tlen, 0);

					len -= tlen;
					off += tlen;

					width = NeatTextWidth(hdc, buf, tlen, -(xpos1 % TabWidth()));
					xpos1 += width;

					if (tlen == 0)
						break;
				}

				// xpos now equals the start of range
			}

			// Invalidate any lines that aren't part of the last line
			while (finish >= lineoff + linelen)
			{
				SetRect(&rect, xpos1, ypos, client.right, ypos + m_nLineHeight);
				rect.left -= m_nHScrollPos * m_nFontWidth;

				//InvertRect(hdc, &rect);
				InvalidateRect(m_hwnd, &rect, FALSE);

				xpos1 = 0;
				charoff = 0;
				ypos += m_nLineHeight;

				// get next line 
				if (!m_pTextDoc->getlineinfo(++lineno, &lineoff, &linelen))
					break;
			}

			// erase up to the end of selection
			ULONG offset = lineoff + charoff;
			ULONG len = finish - charoff;

			xpos2 = xpos1;

			TCHAR buf[TEXTBUFSIZE];
			int width;

			while (offset < finish)
			{
				int tlen = min((finish - offset), TEXTBUFSIZE);
				tlen = m_pTextDoc->getdata(offset, buf, tlen);

				offset += tlen;

				width = NeatTextWidth(hdc, buf, tlen, -(xpos2 % TabWidth()));
				xpos2 += width;
			}

			SetRect(&rect, xpos1, ypos, xpos2, ypos + m_nLineHeight);
			OffsetRect(&rect, -m_nHScrollPos * m_nFontWidth, 0);

			//InvertRect(hdc, &rect);
			InvalidateRect(m_hwnd, &rect, FALSE);

			ReleaseDC(m_hwnd, hdc);

			return 0;
		}

		//
		//	Set the caret position based on m_nCursorOffset,
		//	typically used whilst scrolling 
		//	(i.e. not due to mouse clicks/keyboard input)
		//
		uint32 TextView::RepositionCaret()
		{
			ULONG lineno, charoff;
			ULONG offset = 0;
			int   xpos = 0;
			int   ypos = 0;
			TCHAR buf[TEXTBUFSIZE];

			ULONG nOffset = m_nCursorOffset;

			// make sure we are using the right font
			HDC hdc = GetDC(m_hwnd);
			SelectObject(hdc, m_FontAttr[0].hFont);

			// get line number from cursor-offset
			if (!m_pTextDoc->offset_to_line(nOffset, &lineno, &charoff))
				return 0;

			// y-coordinate from line-number
			ypos = (lineno - m_nVScrollPos) * m_nLineHeight;

			// now find the x-coordinate on the specified line
			while (offset < charoff)
			{
				int tlen = min((charoff - offset), TEXTBUFSIZE);
				tlen = m_pTextDoc->getdata(nOffset - charoff + offset, buf, tlen);

				offset += tlen;
				xpos += NeatTextWidth(hdc, buf, tlen, -xpos);
			}

			ReleaseDC(m_hwnd, hdc);

			// take horizontal scrollbar into account
			xpos -= m_nHScrollPos * m_nFontWidth;
			m_Caret.PosFile = nOffset;
			SetCaretPosition(xpos, ypos);
			return 0;
		}
#pragma endregion

#pragma region Keyboard Control
		long TextView::OnPast()
		{
			if (!OpenClipboard(nullptr))
				return false; // error

				// Get handle of clipboard object for ANSI text
				HANDLE hData = GetClipboardData(CF_TEXT);
			if (hData == nullptr)
				return false; // error

				// Lock the handle to get the actual text pointer
				char * pszText = static_cast<char*>(GlobalLock(hData));
			if (pszText == nullptr)
				return false; // error

				// Save text in a string class instance
				std::string text(pszText);

			// Release the lock
			GlobalUnlock(hData);

			// Release the clipboard
			CloseClipboard();

			string &str = m_pTextDoc->str; 
			str = m_pTextDoc->buffer;
			if (str.size() != 0)
				if (m_nSelectionEnd - m_nSelectionStart != 0)
				{
					int size = (m_nSelectionEnd - m_nSelectionStart);
					if (size > 0)
					{
						size = abs((int) size);
						for (int i = 0; i < size; i++)
						{
							str.erase(str.begin() + (m_Caret.PosFile - size));
						}
						m_Caret.PosFile -= size;
					}
					else
					{
						size = abs((int) size);
						for (int i = 0; i < size; i++)
						{
							str.erase(str.begin() + (m_Caret.PosFile));
						}
					}
					m_pTextDoc->buffer = (char*) str.data();

					m_pTextDoc->length -= size;

				}
			str.reserve(str.size() + 3);
			for (auto var : text)
			{
				str.insert(str.begin() + m_Caret.PosFile, var);
				m_Caret.PosFile++;
				m_pTextDoc->length++;
			}
			m_pTextDoc->buffer = (char*) str.data();
			m_pTextDoc->init_linebuffer();
			m_nLineCount = m_pTextDoc->linecount();
			m_nLongestLine = m_pTextDoc->longestline(4);
			UpdateMetrics();
			RefreshWindow();
			ULONG	nLineNo, nLineOff;
			m_pTextDoc->offset_to_line2(m_Caret.PosFile, &nLineNo, &nLineOff);
			SetCaretPosition(nLineOff *m_nFontWidth, nLineNo* m_nLineHeight);
			m_nSelectionStart = 0;
			m_nSelectionEnd = 0;
			return true;
		}
		long TextView::OnChar(uint nChar, uint nFlags)
		{
			DChar ch = (DChar) nChar;
			string &str = m_pTextDoc->str;

			switch (nChar)
			{
				case 0x08:          // Backspace 
				{
					str = m_pTextDoc->buffer;
					if (str.size() == 0)
						return 0;
					// Move the caret back one space, and then 
					// process this like the DEL key. 
					if (m_nSelectionEnd - m_nSelectionStart != 0)
					{
						int size=(m_nSelectionEnd - m_nSelectionStart);
						if (size > 0)
						{
							size = abs((int) size);
							for (int i = 0; i < size;i++)
							{
								str.erase(str.begin() + (m_Caret.PosFile -size));
							}
							m_Caret.PosFile -= size;
						}
						else
						{
							size = abs((int) size);
							for (int i = 0; i < size;i++)
							{
								str.erase(str.begin() + (m_Caret.PosFile));
							}
						}
						m_pTextDoc->buffer = (char*) str.data();
						
						m_pTextDoc->length -= size;
						
					}
					else
					{
						if (nFlags != 0)
						{
							if (m_Caret.PosFile <= 0)
								return true;
							str.erase(str.begin() + (m_Caret.PosFile - 1));
							m_Caret.PosFile--;
							if (m_Caret.PosFile>=1)
							if (str[m_Caret.PosFile - 1] == '\r')
							{
								str.erase(str.begin() + (m_Caret.PosFile - 1));
								m_Caret.PosFile--;
								m_pTextDoc->length--;
							}
							m_pTextDoc->buffer = (char*) str.data();
							m_pTextDoc->length--;

						}
						else
						{
							if (str.size() > m_Caret.PosFile)
							{

								str.erase(str.begin() + (m_Caret.PosFile));
								if (str.size() > m_Caret.PosFile)
									if (str[m_Caret.PosFile] == '\n')
									{
										str.erase(str.begin() + (m_Caret.PosFile));
										m_pTextDoc->length--;
									}
								m_pTextDoc->buffer = (char*) str.data();
								m_pTextDoc->length--;
							}
						}
					}
					m_pTextDoc->init_linebuffer();
					m_nLineCount = m_pTextDoc->linecount();
					m_nLongestLine = m_pTextDoc->longestline(4);
					UpdateMetrics();
					RefreshWindow();
					ULONG	nLineNo, nLineOff;
					m_pTextDoc->offset_to_line2(m_Caret.PosFile, &nLineNo, &nLineOff);
					SetCaretPosition((nLineOff - m_nHScrollPos) *m_nFontWidth, (nLineNo - m_nVScrollPos)* m_nLineHeight);
					m_nSelectionStart = 0;
					m_nSelectionEnd = 0;
					m_nCursorOffset = 0;
				}
				break;

				case 0x0D:          // Carriage return 
				{
					str = m_pTextDoc->buffer;
					str.reserve(str.size() + 3);
					str.insert(str.begin() + m_Caret.PosFile, (char) '\r');
					str.insert(str.begin() + m_Caret.PosFile + 1, (char) '\n');
					m_pTextDoc->buffer = (char*) str.data();
					m_Caret.PosFile += 2;
					m_pTextDoc->length += 2;
					m_pTextDoc->init_linebuffer();
					m_nLineCount = m_pTextDoc->linecount();
					m_nLongestLine = m_pTextDoc->longestline(4);
					UpdateMetrics();
					RefreshWindow();
					ULONG	nLineNo, nLineOff;
					m_pTextDoc->offset_to_line2(m_Caret.PosFile, &nLineNo, &nLineOff);
					if (nLineNo - m_nVScrollPos >= m_nMaxWindowLines)
					m_nVScrollPos += nLineNo - m_nVScrollPos - m_nMaxWindowLines+1;
					if (nLineOff - m_nHScrollPos >= m_nMaxWindowColumns)
					m_nHScrollPos += nLineOff - m_nHScrollPos - m_nMaxWindowColumns;
					SetCaretPosition((nLineOff - m_nHScrollPos) *m_nFontWidth, (nLineNo - m_nVScrollPos)* m_nLineHeight);
					m_nSelectionStart = 0;
					m_nSelectionEnd = 0;
					m_nCursorOffset = 1;
					UpdateMetrics();
					RefreshWindow();
				}
				break;

				default:
				// Add the character to the text buffer. 
				{
					str = m_pTextDoc->buffer;
					if (str.size() != 0)
					if (m_nSelectionEnd - m_nSelectionStart != 0)
					{
						int size = (m_nSelectionEnd - m_nSelectionStart);
						if (size > 0)
						{
							size = abs((int) size);
							for (int i = 0; i < size; i++)
							{
								str.erase(str.begin() + (m_Caret.PosFile - size));
							}
							m_Caret.PosFile -= size;
						}
						else
						{
							size = abs((int) size);
							for (int i = 0; i < size; i++)
							{
								str.erase(str.begin() + (m_Caret.PosFile));
							}
						}
						m_pTextDoc->buffer = (char*) str.data();

						m_pTextDoc->length -= size;

					}
					str.reserve(str.size() + 3);
					str.insert(str.begin() + m_Caret.PosFile, (char) nChar);
					m_pTextDoc->buffer = (char*) str.data();
					m_Caret.PosFile++;
					m_pTextDoc->length++;
					m_pTextDoc->init_linebuffer();
					m_nLineCount = m_pTextDoc->linecount();
					m_nLongestLine = m_pTextDoc->longestline(4);
					UpdateMetrics();
					RefreshWindow();
					ULONG	nLineNo, nLineOff;
					m_pTextDoc->offset_to_line2(m_Caret.PosFile, &nLineNo, &nLineOff);
					if (nLineNo - m_nVScrollPos >= m_nMaxWindowLines)
						m_nVScrollPos += nLineNo - m_nVScrollPos - m_nMaxWindowLines + 1;
					if (nLineOff - m_nHScrollPos >= m_nMaxWindowColumns)
						m_nHScrollPos += nLineOff - m_nHScrollPos - m_nMaxWindowColumns;
					SetCaretPosition((nLineOff - m_nHScrollPos) *m_nFontWidth, (nLineNo - m_nVScrollPos)* m_nLineHeight);
					m_nSelectionStart = 0;
					m_nSelectionEnd = 0;
					UpdateMetrics();
					RefreshWindow();

				}
				break;
			}
			m_nCursorOffset = m_Caret.PosFile;
			return 0;

		}

#pragma endregion

	}
}