#pragma once
#include"Token.h"
#include"..\Compiler.h"
namespace CompilerPackage
{
	class Compiler;
	class LexicalAnalyzer
	{
		Compiler* m_Comiler = nullptr;
		uint BeginIndex = 0, EndIndex = 0, Row = 1, Col = 1;
		HashTable<string, SymbolsTable>* m_TokenTable = nullptr;	// a pointer to m_TokenTable in Compiler class
		//DArray<Token*>* m_ArrayToken = nullptr;						// a pointer to m_ArrayToken in Compiler class
		TokenType m_LastTypeDef;//Top index of Last instruction of Type definition that get from Compiler class.
		char* m_Buffer = nullptr;										// a pointer to Buffer in Compiler class
		uint state = 0;
		char* temp = nullptr;
		uint m_Length = 0;
		friend class Compiler;
		bool HasCaseSensitive = false;
		bool IsTypeDefinitionState = false;

#define ReturnItem quit=true;continue
	public:
		LexicalAnalyzer(Compiler* ref);
		~LexicalAnalyzer();
		Token* GetNextToken();
	};
}