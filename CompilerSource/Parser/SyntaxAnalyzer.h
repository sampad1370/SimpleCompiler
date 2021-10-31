#pragma once
#include"../Compiler.h"

namespace CompilerPackage
{
	class Compiler;
	class LexicalAnalyzer;
	struct ActionGoto
	{
		string instruction;//! "Si","Ri","Ei","Gi";that "i" is number
	};
	class SyntaxAnalyser
	{
		friend class Compiler;
		Compiler* m_Comiler = nullptr;
		LexicalAnalyzer *m_lexc = nullptr;
		HashTable<string, SymbolsTable>* m_TokenTable = nullptr;	// a pointer to m_TokenTable in Compiler class
		/* parser variable */
		string** m_ParseTable;
		Map<string, int> m_ColumnMap;
		DArrayPointer<pair<string, pair<int,string>>> m_Production;
		void LoadParseTable(char* ConfigFile);
		tree<DataToken> m_ParseTree;

	public:
		SyntaxAnalyser(Compiler* ref, char* ConfigFile = "ParserCofig.xml");
		~SyntaxAnalyser();
		void* Parse();

	};
}