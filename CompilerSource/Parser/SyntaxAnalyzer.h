#pragma once
#include"../Compiler.h"

namespace CompilerPackage
{
	class Compiler;
	class LexicalAnalyzer;
	struct ActionGoto
	{
		std::string instruction;//! "Si","Ri","Ei","Gi";that "i" is number
	};
	class SyntaxAnalyser
	{
		friend class Compiler;
		Compiler* m_Comiler = nullptr;
		LexicalAnalyzer *m_lexc = nullptr;
		::BaseEngine::Core::Array::HashTable<std::string, SymbolsTable>* m_TokenTable = nullptr;	// a pointer to m_TokenTable in Compiler class
		/* parser variable */
		std::string** m_ParseTable;
		::BaseEngine::Core::Array::Map<std::string, int> m_ColumnMap;
		::BaseEngine::Core::Array::DArrayPointer<std::pair<std::string, std::pair<int,std::string>>> m_Production;
		void LoadParseTable(char* ConfigFile);
		::BaseEngine::Core::Tree::tree<DataToken> m_ParseTree;

	public:
		SyntaxAnalyser(Compiler* ref, char* ConfigFile = "ParserCofig.xml");
		~SyntaxAnalyser();
		void* Parse();

	};
}