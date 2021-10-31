#pragma once
#include "Config.h"
#include <WTypes.h>

namespace BaseEngine
{
	namespace Core
	{
		namespace BaseType
		{
#define ToDChar(p) (const DChar*) DString(p)

#define ToWideChar(p) (const wchar_t*) WString(p)
#define WST (WString)
#define ST (String)

			//For Ansi String with Wide Operator
			class DGE_API String
			{
				std::string Data;
			public:
				size_t length;
				inline String(const char* ch)
				{
					try
					{
						length = strlen(ch);
						if (length < 1)
						{
							Data = "";
							length = 0;
							return;
						}
						Data = ch;
					}
					catch (...)
					{
						length = 0;
						Data = "";
					}
				}
				inline String(char ch)
				{
					length = 1;
					Data = ch;
				}
				inline String()
				{
					Data.clear();
					length = 0;
				}
				inline String(std::string str)
				{
					if (str.size() == 0)
					{
						Data = "";
						length = 0;
						return;
					}
					Data = str;
					length = Data.length();
				}
				inline String(LPSTR str)
				{
					if (strlen(str) == 0)
					{
						Data = "";
						length = 0;
						return;
					}
					Data = str;
					length = Data.length();
				}
				inline String(int number)
				{
					std::stringstream myString;
					myString << number;
					myString >> Data;
					length = Data.length();
				}
				inline String(std::wstring str)
				{
					if (str.size() == 0)
					{
						Data = "";
						length = 0;
						return;
					}
					WCharToMb(str);
					length = Data.length();
				}
				inline const char* ToChar()
				{
					return Data.c_str();
				}
				inline String operator+(String str)
				{
					std::string s1 = this->Data;
					s1 += str.Data;
					String st = s1;
					return (st);
				}
				inline String operator+(int data)
				{
					std::string s1 = this->Data;
					std::stringstream myString;
					myString << data;
					s1 += myString.str();
					String st = s1;
					return (st);
				}
				inline String operator+(unsigned int data)
				{
					std::string s1 = this->Data;
					std::stringstream myString;
					myString << data;
					s1 += myString.str();
					String st = s1;
					return (st);
				}
				inline String operator+(unsigned short data)
				{
					std::string s1 = this->Data;
					std::stringstream myString;
					myString << data;
					s1 += myString.str();
					String st = s1;
					return (st);
				}
				inline String operator+(unsigned long data)
				{
					std::string s1 = this->Data;
					std::stringstream myString;
					myString << data;
					s1 += myString.str();
					String st = s1;
					return (st);
				}
				inline String operator+(double data)
				{
					std::string s1 = this->Data;
					std::stringstream myString;
					myString << data;
					s1 += myString.str();
					String st = s1;
					return (st);
				}
				inline String operator+=(String str)
				{
					this->Data += str.Data;
					//String st=this->Data;
					this->length = this->Data.length();
					return (*this);
				}
				inline String operator+=(int data)
				{
					std::stringstream myString;
					myString << data;
					this->Data += myString.str();
					//String st=this->Data;
					this->length = this->Data.length();
					return (*this);
				}
				inline String operator+=(unsigned int data)
				{
					std::stringstream myString;
					myString << data;
					this->Data += myString.str();
					//String st=this->Data;
					this->length = this->Data.length();
					return (*this);
				}
				inline String operator+=(unsigned short data)
				{
					std::stringstream myString;
					myString << data;
					this->Data += myString.str();
					//String st=this->Data;
					this->length = this->Data.length();
					return (*this);
				}
				inline String operator+=(unsigned long data)
				{
					std::stringstream myString;
					myString << data;
					this->Data += myString.str();
					//String st=this->Data;
					this->length = this->Data.length();
					return (*this);
				}
				inline String operator+=(double data)
				{
					std::stringstream myString;
					myString << data;
					this->Data += myString.str();
					//String st=this->Data;
					this->length = this->Data.length();
					return (*this);
				}
				inline bool operator < (const String &str1)
				{
					return this->Data < str1.Data;
				}
				inline bool operator >(const String &str1)
				{
					return this->Data > str1.Data;
				}
				inline bool operator!=(String str)
				{
					return this->Data != str.Data;
				}
				inline bool operator==(String str)
				{
					return this->Data == str.Data;
				}
				inline void resize(UINT size)
				{
					Data.resize(size);
					//throw Error("Engine","String","resize","Change Size Of String When Need to do this","");
				}
				inline std::string GetWString()
				{
					return Data;
				}
				inline int ToInt()
				{
					std::stringstream myString;
					int data;
					myString << Data;
					myString >> data;
					return data;
				}
				static inline int ToInt(const std::string& dataStr)
				{
					std::stringstream myString;
					int data;
					myString << dataStr;
					myString >> data;
					return data;
				}
				static inline int ToInt(const char* dataStr)
				{
					std::stringstream myString;
					int data;
					myString << dataStr;
					myString >> data;
					return data;
				}

				inline operator std::string()
				{
					return Data;
				}
				inline operator const char*()
				{
					return Data.c_str();
				}
				inline operator char*()
				{
					return (char*) Data.c_str();
				}
				inline bool operator() (String const& n1, String const& n2) const
				{
					return n1.Data < n2.Data;
				}
				inline void WCharToMb(std::wstring wstr)
				{
					int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int) wstr.size(), NULL, 0, NULL, NULL);
					Data.resize(size_needed, 0);
					WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int) wstr.size(), &Data[0], size_needed, NULL, NULL);
				}
				inline char &operator [](UINT index)
				{
					if (index >= Data.size())
					{
						throw "Index Is Out Of Lenght String";
					}
					return Data[index];
				}
				inline String ToLower()
				{
					String temp;
					for (size_t i = 0; i < length; i++)
						if (isalpha(Data[i]) && isupper(Data[i]))
							temp += (char) tolower(Data[i]);
						else
							temp += (char) Data[i];
					return temp;
				}
				inline const char* operator*()
				{
					return Data.data();
				}
				/*const Array<String> Splite(Char* pattern)
				{
				Array<String> arr;
				string temp = Data;
				size_t size = strlen(pattern);
				string::size_type si = 0;
				string subs;
				while ((si = temp.find(pattern)) != string::npos)
				{
				subs = temp.substr(0, si);
				arr.add(subs);
				temp.erase(0, si + size);
				}
				arr.add(temp);
				return arr;
				}*/
				inline static String Convert(void* pdata)
				{
					std::stringstream myString;
					std::string str;
					myString << pdata;
					myString >> str;
					return str;
				}
				inline static void* RetConvert(String pdata)
				{
					std::stringstream myString;
					void* str;
					myString << pdata.ToChar();
					myString >> str;
					return str;
				}
				inline LPSTR ToLPSTR()
				{
					return (char*) Data.data();
				}

				inline bool IsEmpty()
				{
					return Data.empty();
				}
			};

			//For Wide String with Wide Operator
			class DGE_API WString
			{
				std::wstring Data;
			public:
				size_t length;
				inline WString(const char* ch)
				{
					size_t lenght = strlen(ch);
					if (lenght < 1)
					{
						Data = L"";
						length = 0;
						return;
					}
					MbToWChar(ch);
					length = Data.length();
				}
				inline WString(char* ch)
				{
					size_t lenght = strlen(ch);
					if (lenght < 1)
					{
						Data = L"";
						length = 0;
						return;
					}
					MbToWChar(ch);
					length = Data.length();
				}
				inline WString(std::string str)
				{
					if (str.size() == 0)
					{
						Data = L"";
						length = 0;
						return;
					}
					MbToWChar(str);
					length = Data.length();
				}
				inline WString(std::wstring str = L"")
				{
					Data = str;
					length = Data.length();
				}
				inline WString(LPCWSTR str)
				{
					Data = str;
					length = Data.length();
				}
				inline WString(int data)
				{
					std::wstringstream myString;
					myString << data;
					myString >> Data;
				}
				inline WString(float data)
				{
					std::wstringstream myString;
					myString << data;
					myString >> Data;
				}
				inline WString(String str)
				{
					if (str.length == 0)
					{
						Data = L"";
						length = 0;
						return;
					}
					this->MbToWChar(str.ToChar());
					length = Data.length();
				}
				inline void MbToWChar(std::string str)
				{
					/*int len;
					int slength = (int)str.length() + 1;
					len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), slength, 0, 0);
					std::wstring r(len, L'\0');
					MultiByteToWideChar(CP_ACP, 0, str.c_str(), slength, &r[0], len);
					Data=r;*/
					Data.resize(MultiByteToWideChar(CP_UTF8, 0, str.data(), (int) str.length(), NULL, 0));
					MultiByteToWideChar(CP_UTF8, 0, str.data(), (int) str.length(), &Data[0], (int) Data.length());
				}
				inline const wchar_t* ToWChar()
				{
					return Data.c_str();
				}
				inline WString operator+(WString str)
				{
					WString s1 = this->Data + str.Data;
					return std::move(s1);
				}
				inline WString operator+(int data)
				{
					std::wstring s1 = this->Data;
					std::wstringstream myString;
					myString << data;
					s1 += myString.str();
					WString st = s1;
					return (st);
				}
				inline WString operator+(unsigned int data)
				{
					std::wstring s1 = this->Data;
					std::wstringstream myString;
					myString << data;
					s1 += myString.str();
					WString st = s1;
					return (st);
				}
				inline WString operator+(unsigned short data)
				{
					std::wstring s1 = this->Data;
					std::wstringstream myString;
					myString << data;
					s1 += myString.str();
					WString st = s1;
					return (st);
				}
				inline WString operator+(double data)
				{
					std::wstring s1 = this->Data;
					std::wstringstream myString;
					myString << data;
					s1 += myString.str();
					WString st = s1;
					return (st);
				}
				inline WString operator+(unsigned long data)
				{
					std::wstring s1 = this->Data;
					std::wstringstream myString;
					myString << data;
					s1 += myString.str();
					WString st = s1;
					return (st);
				}
				inline WString operator+=(WString str)
				{
					this->Data += str.Data;
					//WString st=this->Data;
					this->length = this->Data.length();
					return (*this);
				}
				inline WString operator+=(int data)
				{
					std::wstringstream myString;
					myString << data;
					this->Data += myString.str();
					//String st=this->Data;
					this->length = this->Data.length();
					return (*this);
				}
				inline WString operator+=(double data)
				{
					std::wstringstream myString;
					myString << data;
					this->Data += myString.str();
					//String st=this->Data;
					this->length = this->Data.length();
					return (*this);
				}
				inline WString operator+=(unsigned int data)
				{
					std::wstringstream myString;
					myString << data;
					this->Data += myString.str();
					//String st=this->Data;
					this->length = this->Data.length();
					return (*this);
				}
				inline WString operator+=(unsigned long data)
				{
					std::wstringstream myString;
					myString << data;
					this->Data += myString.str();
					//String st=this->Data;
					this->length = this->Data.length();
					return (*this);
				}
				inline bool operator!=(WString str)
				{
					return this->Data != str.Data;
				}
				inline bool operator==(WString str)
				{
					return this->Data == str.Data;
				}
				inline static const wchar_t* MbToWChar(char ch)
				{
					std::wstring wch;
					wch.resize(MultiByteToWideChar(CP_UTF8, 0, &ch, 1, NULL, 0));
					MultiByteToWideChar(CP_UTF8, 0, &ch, 1, &wch[0], (int) wch.length());
					return wch.c_str();
				}
				inline void resize(UINT size)
				{
					Data.resize(size);
					//throw Error("Engine","String","resize","Change Size Of String When Need to do this","");
				}
				inline std::wstring GetWString()
				{
					return Data;
				}
				inline operator std::wstring()
				{
					return Data;
				}
				inline operator const wchar_t*()
				{
					return Data.c_str();
				}
				inline operator wchar_t*()
				{
					return (wchar_t*) Data.c_str();
				}
				inline operator const char*()
				{
					return String(Data).ToChar();
				}
				inline operator char*()
				{
					return (char*) String(Data).ToChar();
				}
				inline int ToInt()
				{
					std::wstringstream myString;
					int data;
					myString << Data;
					myString >> data;
					return data;
				}
				inline LPWSTR ToLPWSTR()
				{
					return (wchar_t*) Data.c_str();
				}
				inline LPCWSTR ToLPCWSTR()
				{
					return (wchar_t*) Data.c_str();
				}
				inline String GetString()
				{
					return String(Data);
				}

				inline bool IsEmpty()
				{
					return Data.empty();
				}

				inline void Clear()
				{
					Data.clear();
				}
			};
		}
	}
}