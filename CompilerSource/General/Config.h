
//! -----------------------------------------------------------------
	//! C++ Header File.
	//! -----------------------------------------------------------------
#include<vector>
#include<set>
#include<queue>
#include<map>
#include<list>
#include<stack>
#include<unordered_map>
#include<unordered_set>
#include<tuple>
#include<valarray>
#include<utility>
#include<chrono>
#include<array>
#include<algorithm>
#include<typeinfo> //for bad_cast and bad_typeid
#include<exception> //for classes exception and bad_exception
#include<stdexcept> //for most logic and runtime error classes
#include<system_error> //for system errors (since C++11)
#include<ios> //for I/O exceptions
#include<fstream>
#include<iostream>
#include<sstream>
#include<string>
#ifndef _M_CEE
#include <atomic>
#include<mutex>
#include<condition_variable>
#include<future>//for errors with a sync() and futures (since C++11)
#endif

	//! -----------------------------------------------------------------
	//! C Header File.
	//! -----------------------------------------------------------------
#include<cassert>
#include<cctype>
#include<cerrno>
#include<cfenv>
#include<cfloat>
#include<cinttypes>
#include<climits>
#include<clocale>
#include<cmath>
#include<csetjmp>
#include<csignal>
#include<cstdarg>
#include<cstdbool>
#include<cstddef>
#include<cstdint>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<ctgmath>
#include<ctime>
#include<cwchar>
#include<cwctype>
#include <process.h>

#include "String.h"
#include "DArray.h"
#include "DTree.h"


typedef char Char;
typedef wchar_t WChar;

//unsigned long
typedef unsigned long uint32;

//unsigned long long
typedef unsigned long long uint64;
//unsigned char
typedef unsigned char uchar;
//unsigned int
typedef unsigned int uint;
//unsigned short
typedef unsigned short ushort;

#define DGE_API 

#define SafeDelete(x) if(x) {delete x; x=nullptr;}
#define SafeDeleteArray(x) if(x) {delete[] x; x=nullptr;}
using DChar = char;
#define GL 

#define GetValueNode(node,attributeName) node->first_attribute(attributeName)->value()
#define NextSelf(node) (node=(node->next_sibling()))
#define NextSelfByName(node,name) (node=(node->next_sibling(name)))
#define GoDown(node) node=node->first_node()
#define GoDownByName(node,name) node=node->first_node(name)