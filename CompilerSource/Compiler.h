#pragma once
//#pragma comment(lib,"GameEngine.lib")

#include "General\Config.h"
#include"Lexical\Token.h"
#include"Lexical\LexicalAnalyzer.h"
#include"Parser\SyntaxAnalyzer.h"
#include"Semantic\SemanticAnalyzer.h"

using namespace std;

namespace CompilerPackage
{
	//? this type use for use in column of table
	class LexicalAnalyzer;
	class SyntaxAnalyser;
	class SemanticAnalyzer;
	extern std::string NameToken [];
	extern std::string NameOfToken[];
	
	enum class CompilerState
	{
		NotStart,Started,LexicalAnalyzer,//...
		Ended
	};

	class Compiler
	{
		
		FILE* m_file = nullptr;
		std::fstream fs;
		std::string strp;
		char *Buffer;
		uint Length = 0;
		LexicalAnalyzer *m_lexc = nullptr;
		SyntaxAnalyser *m_SyntaxAnlyser = nullptr;
		SemanticAnalyzer* m_SemanticAnalayzer = nullptr;
		::BaseEngine::Core::Array::HashTable<string, SymbolsTable> m_TokenTable;
		::BaseEngine::Core::Array::DArrayPointer<Token>* m_ArrayToken;
		::BaseEngine::Core::Tree::tree<DataToken>* m_ParseTree;
		TokenType m_LastTypeDef;//Top index of Last instruction of Type definition
		std::string FileName;
		friend class LexicalAnalyzer;
		friend class SyntaxAnalyser;
		friend class SemanticAnalyzer;
		ErrorManager m_ErrorManager;
		bool Open(char* file);
		void Show();
	public:
		int x, y, length,offset;//for location of error!
		std::string Error;
		WString EndMessage;
		Compiler();
		~Compiler();
		#define Reserve(type,key) m_TokenTable[key] = SymbolsTable(key, new Token(type, 0, 0))
		
		bool Compile(char* file);
		bool Compile(char* buffer, char* file);
		::BaseEngine::Core::Tree::tree<DataToken>* GetParseTree(){ return m_ParseTree; }
	};
}