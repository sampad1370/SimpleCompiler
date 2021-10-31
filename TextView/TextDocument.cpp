#include"TextView.h"

namespace ComponentUI
{
	namespace TextViewComponent
	{
		//	TextDocument constructor
		TextDocument::TextDocument()
		{
			str = "";
			buffer = (char*)str.data();
			length = 0;
			linebuffer = 0;
			numlines = 0;
			FileName = "";
		}

		//	TextDocument destructor
		TextDocument::~TextDocument()
		{
			clear();
		}

		//	Initialize the TextDocument with the specified file
		bool TextDocument::init(DChar *filename)
		{
			HANDLE hFile;
			FileName = filename;
			hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

			if (hFile == INVALID_HANDLE_VALUE)
				return false;

			return init(hFile);
		}

		//	Initialize using a file-handle
		bool TextDocument::init(HANDLE hFile)
		{
			uint32 numread;

			if ((length = GetFileSize(hFile, 0)) == 0)
				return false;

			// allocate new file-buffer
			buffer = new DChar[length+1];
			if ((buffer) == nullptr)
				return false;

			// read entire file into memory
			ReadFile(hFile, buffer, length, &numread, 0);

			// work out where each line of text starts
			if (!init_linebuffer())
				clear();
			buffer[length] = 0;
			str = buffer;
			SafeDeleteArray(buffer);
			buffer = (char*) str.data();
			CloseHandle(hFile);
			return true;
		}

		//	Initialize the line-buffer, so we know where every line starts within the file
		bool TextDocument::init_linebuffer()
		{
			uint32 i = 0;
			uint32 linestart = 0;
			if (length == 0)
				return false;
			// allocate the line-buffer
			SafeDeleteArray(linebuffer);
			linebuffer = new uint32[length+1];
			if ((linebuffer) == nullptr)
				return false;

			numlines = 0;

			// loop through every byte in the file
			for (i = 0; i < length;i++)
			{
				if (buffer[i] == '\r')
				{
					if (buffer[i + 1] == '\n')
						i++;
					// carriage-return / line-feed combination

					// record where the line starts
					linebuffer[numlines++] = linestart;
					linestart = i + 1;
				}
				else if (buffer[i] == '\n')
				{

					// record where the line starts
					linebuffer[numlines++] = linestart;
					linestart = i + 1;
				}
			}

			if (length > 0)
				linebuffer[numlines++] = linestart;

			linebuffer[numlines] = length;

			return true;
		}

		//	Empty the data-TextDocument
		bool TextDocument::clear()
		{
			
			//SafeDeleteArray(buffer);
			str.clear();
			length = 0;

			SafeDeleteArray(linebuffer);
			numlines = 0;

			return true;
		}

		//	Retrieve the specified line of text and store it in "buffer"
		uint32 TextDocument::getline(uint32 lineno, uint32 offset, DChar *buf, size_t len, uint32 *fileoff)
		{
			DChar *lineptr;
			uint32 linelen;

			if (lineno >= numlines || buffer == nullptr || length == 0)
				return 0;

			//	if(linebuffer[lineno]

			// find the start of the specified line
			lineptr = buffer + linebuffer[lineno];

			// work out how long it is, by looking at the next line's starting point
			linelen = linebuffer[lineno + 1] - linebuffer[lineno];

			offset = min(offset, linelen);

			// make sure we don't overflow caller's buffer
			linelen = min(len, linelen - offset);

			lineptr += offset;

			memcpy(buf, lineptr, linelen);

			if (fileoff)
				*fileoff = linebuffer[lineno];

			return linelen;
		}
		uint32 TextDocument::getline(uint32 lineno, DChar *buf, size_t len, uint32 *fileoff)
		{
			return getline(lineno, 0, buf, len, fileoff);
		}

		uint32 TextDocument::getdata(uint32 offset, DChar *buf, size_t len)
		{
			memcpy(buf, buffer + offset, len);
			return len;
		}

		//	Return the number of lines
		uint32 TextDocument::linecount()
		{
			return numlines;
		}

		//	Return the length of longest line
		uint32 TextDocument::longestline(int tabwidth)
		{
			uint32 i;
			uint32 longest = 0;
			uint32 xpos = 0;

			// use the line-buffer to work out which is the longest line
			for (i = 0; i < length; i++)
			{
				if (buffer[i] == '\r' || buffer[i] == '\n')
				{
					if (buffer[i + 1] == '\n')
						i++;

					longest = max(longest, xpos);
					xpos = 0;
				}
				else if (buffer[i] == '\t')
				{
					xpos += tabwidth - (xpos % tabwidth);
				}
				else
				{
					xpos++;
				}
			}

			longest = max(longest, xpos);
			return longest;
		}

		bool TextDocument::getlineinfo(uint32 lineno, uint32 *fileoff, uint32 *length)
		{
			if (lineno < numlines)
			{
				if (length)
					*length = linebuffer[lineno + 1] - linebuffer[lineno];

				if (fileoff)
					*fileoff = linebuffer[lineno];

				return true;
			}
			else
			{
				return false;
			}
		}

		//
		//	Perform a reverse lookup - file-offset to line number
		//
		bool TextDocument::offset_to_line(uint32 fileoff, uint32 *lineno, uint32 *offset)
		{
			uint32 low = 0, high = numlines - 1;
			uint32 line = 0;

			if (numlines == 0)
			{
				if (lineno) *lineno = 0;
				if (offset) *offset = 0;
				return false;
			}

			while (low <= high)
			{
				line = (high + low) / 2;

				if (fileoff >= linebuffer[line] && fileoff < linebuffer[line + 1])
				{
					break;
				}
				else if (fileoff < linebuffer[line])
				{
					high = line - 1;
				}
				else
				{
					low = line + 1;
				}
			}

			if (lineno)  *lineno = line;
			if (offset)	*offset = fileoff - linebuffer[line];

			return true;
		}

		bool  TextDocument::offset_to_line2(uint32 fileoffset, uint32 *lineno, uint32 *offset,uint tabwidth)
		{
			string sss = buffer;
			int i = 0, loc = fileoffset;
			(*lineno) = 0;
			(*offset) = 0;
			for (; i <= fileoffset; i++)
			{
				if (buffer[i] == '\n')
				{
					(*lineno)++;
					loc = i;
				}
			}
			if (loc >= fileoffset - 1)
			{
				if ((*lineno) == 0)
				{
					for (i = 0; i < fileoffset; i++)
					{
						if (buffer[i] == '\t')
							(*offset) += tabwidth;
						else if (buffer[i] != '\r')
							(*offset)++;
					}
				}
				return true;
			}
			for (i = loc+1; i < fileoffset; i++)
			{
				if (buffer[i] == '\t')
					(*offset) += tabwidth;
				else if (buffer[i] != '\r')
					(*offset)++;
			}
			return false;
			//str.find_last_of('\n', fileoffset);
		}


		uint32 TextDocument::size()
		{
			return length;
		}
	}
}