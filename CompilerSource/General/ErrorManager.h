#pragma once
#include<Engine.h>
#include<thread>
#include"..\Lexical\Token.h"
using namespace BaseEngine;

namespace CompilerPackage
{
	struct Token;
	namespace Error
	{
		enum class ErrorType
		{
			Internal, Compiler, LexicalAnalyser,SyntaxAnalyser,SemanticAnalyzer,//...
		};
		struct BaseError
		{
			String Message = "";
			void* ExtraParam = nullptr;
			uint size = 0;
			ErrorType ErrType = ErrorType::Internal;
			BaseError(String message="")
			{
				Message = message;
			}
		};
		struct LexicalAnalyserError :public BaseError
		{
			WChar Character = 0;
			uint Row = 0, Col = 0;
			uint Wide = 0;
			Token* token;
			LexicalAnalyserError(){}
			LexicalAnalyserError(String message, WChar character, uint row, uint col, uint wide,Token* Token)
			{
				Message = message;
				ErrType = ErrorType::LexicalAnalyser;
				Character = character;
				Row = row;
				Col = col;
				Wide = wide;
				token = Token;
			}
		};
		class ErrorManager
		{
			Core::Array::DArrayPointer<BaseError> m_Queue;
		public:
			ErrorManager();
			~ErrorManager();
			void Add(BaseError*);
			void ReportToFile(string filename);
			bool HasError();
			BaseError* GetFirstError();
		};
		//extern ErrorManager g_ErrorManager;
	}
	using namespace CompilerPackage::Error;
}