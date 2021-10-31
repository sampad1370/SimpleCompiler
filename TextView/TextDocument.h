#pragma once
#define _CRT_SECURE_NO_WARNINGS 1
//#pragma comment(lib,"GameEngine.lib")
//#include<Engine.h>
#include"TextView.h"
#include <General/Config.h>

//using namespace BaseEngine;

namespace ComponentUI
{

	namespace TextViewComponent
	{
		class TextView;
		class TextDocument
		{
			bool init_linebuffer();


			uint32 *linebuffer;
			uint32  numlines;
			std::string str;
			friend class TextView;
		public:
			uint32 length;
			DChar *buffer;
			std::string FileName;
			TextDocument();
			~TextDocument();

			bool init(HANDLE hFile);
			bool init(DChar *filename);

			bool clear();

			bool  offset_to_line(uint32 fileoffset, uint32 *lineno, uint32 *offset);
			bool  offset_to_line2(uint32 fileoffset, uint32 *lineno, uint32 *offset, uint tabwidth=4);
			bool  getlineinfo(uint32 lineno, uint32 *fileoff, uint32 *length);

			uint32 getline(uint32 lineno, DChar *buf, size_t len, uint32 *fileoff = 0);
			uint32 getline(uint32 lineno, uint32 offset, DChar *buf, size_t len, uint32 *fileoff = 0);
			uint32 getdata(uint32 offset, DChar *buf, size_t len);

			uint32 linecount();
			uint32 longestline(int tabwidth);
			uint32 size();

			std::string GetString()
			{
				return str;
			}
		};
	}
}