#include"Compiler.h"
namespace CompilerPackage
{

	#pragma region NameToken
		string NameToken [] =
		{
			"","_class" , "_struct", "_boolean", "_break", "_char", "_cin", "_cout", "_else", "_false", "_float", "_double", "_for", "_if", "_elseif" , "_int", "_main", "_null",
			"_private", "_public", "_return", "_true", "_void", "_while","_const", "and", "assign", "cbrac", "charliteral", "comma", "div", "dot", "eof", "eq",
			"ge", "gt", "id", "lbracket", "le", "lpran", "lsh", "lt", "minus", "mod", "mul", "neq", "not", "num_float", "num_int", "obrac", "or", "plus", "pow", "plusplus", "minesmines",
			"rbracket", "rpran", "rsh","coloncolon", "semi", "strliteral", "_typeid", "Unknown"
		};
		string NameOfToken[] =
		{
			"","class" , "struct", "boolean", "break", "char", "cin", "cout", "else", "false", "float", "double", "for", "if", "elseif", "int", "main", "null",
			"private", "public", "return", "true", "void", "while","const", "&&", "=", "}", ""/*literal character*/, ",", "/", ".", "" /*EndOfFile*/, "==",
			">=", ">", "id", "[", "<=", "(", "<<", "<", "-", "%", "*", "!=", "!", ""/*float number*/, ""/*integer number*/, "{", "||", "+", "**", "++", "--",
			"]", ")", ">>", ":", ";", ""/*literal string*/, "typeid", "Unknown"
		};
	#pragma endregion

	Compiler::Compiler()
	{
		Reserve(TokenType::_class, "class");
		Reserve(TokenType::_struct, "struct");
		Reserve(TokenType::_boolean, "boolean");
		Reserve(TokenType::_break, "break");
		Reserve(TokenType::_char, "char");
		Reserve(TokenType::_cin, "cin");
		Reserve(TokenType::_cout, "cout");
		Reserve(TokenType::_else, "else");
		Reserve(TokenType::_false, "false");
		Reserve(TokenType::_float, "float");
		Reserve(TokenType::_double, "double");
		Reserve(TokenType::_for, "for");
		Reserve(TokenType::_if, "if");
		Reserve(TokenType::_elseif, "elseif");
		Reserve(TokenType::_int, "int");
		Reserve(TokenType::_main, "main");
		Reserve(TokenType::_null, "null");
		Reserve(TokenType::_private, "private");
		Reserve(TokenType::_public, "public");
		Reserve(TokenType::_return, "return");
		Reserve(TokenType::_true, "true");
		Reserve(TokenType::_void, "void");
		Reserve(TokenType::_while, "while");
		Reserve(TokenType::_const, "const");
		m_ArrayToken = new DArrayPointer<Token>;
		m_LastTypeDef = TokenType::none;

	}
	Compiler::~Compiler()
	{
		if (m_file)
		fclose(m_file);
		SafeDeleteArray(Buffer);
		SafeDelete(m_lexc);
		m_TokenTable.clear();
		SafeDelete(m_ArrayToken);
	}
	bool Compiler::Open(char* file)
	{
		fopen_s(&m_file, file, "a+t");
		fseek(m_file, 0, SEEK_END);
		Length = ftell(m_file);
		Buffer = new char[Length + 2];
		fseek(m_file, 0L, SEEK_SET);
		fread_s(Buffer, sizeof(char) * Length, sizeof(char) * Length, sizeof(char), m_file);
		for (uint k = 0; k < Length+2; k++)
		{
			if (Buffer[k]<0)
			{
				Buffer[k] = 0;
				Buffer[k + 1] = -1;
				break;
			}
		}
		fclose(m_file);
		FileName = file;
		FileName=FileName.substr(0,FileName.find_first_of('.'));
		m_lexc = new LexicalAnalyzer(this);
		return true;
	}
	/*bool Compiler::Compile(char* file)
	{
		Open(file);
		fstream fs;
		
		while (true)
		{
			Token* token=m_lexc->GetNextToken();
			if (token!=nullptr)
				m_ArrayToken->push_back(token);
			else
			{
				EndMessage = L"Compile Complete With Internal Error.";
				m_ErrorManager.Add(new BaseError("error : Internal Error Occurred in Compiler!!"));
				m_ErrorManager.ReportToFile(FileName);
				break;
			}
			if (token->Type==TokenType::eof)
			{
				
				if (!m_ErrorManager.HasError())
				{
					EndMessage = L"Compile Complete Successfully.";
					
					m_ErrorManager.ReportToFile(FileName);
					//cout << EndMessage << "\n";
				}
				else
				{
					EndMessage = L"Compile Complete With Error.";
					m_ErrorManager.ReportToFile(FileName);
					//cout << "Compile Complete But with Error!" << endl;
				}
				break;
			}
		}
		Show();
		cout << "Result Write To File Successfully." << endl;
		return !m_ErrorManager.HasError();
	}*/
	bool Compiler::Compile(char* file)
	{
		Open(file);
		fstream fs;
		m_SyntaxAnlyser = new SyntaxAnalyser(this);
		m_SyntaxAnlyser->Parse();
		return !m_ErrorManager.HasError();
	}

	bool Compiler::Compile(char* buffer, char* file)
	{
		Buffer = buffer;
		FileName = file;
		FileName = FileName.substr(0, FileName.find_first_of('.'));
		m_lexc = new LexicalAnalyzer(this);
		fstream fs;
		m_SyntaxAnlyser = new SyntaxAnalyser(this);
		m_SyntaxAnlyser->Parse();
		m_ParseTree = &m_SyntaxAnlyser->m_ParseTree;
		if (!m_ErrorManager.HasError())
		{
			m_SemanticAnalayzer = new SemanticAnalyzer(this);
			m_SemanticAnalayzer->ProcessTree();
		}
		if (m_ErrorManager.HasError())
		{
			LexicalAnalyserError* ptrError = (LexicalAnalyserError*) m_ErrorManager.GetFirstError();
			if (ptrError->ErrType == ErrorType::LexicalAnalyser || ptrError->ErrType == ErrorType::SyntaxAnalyser)
			{
				x = ptrError->Row; y = ptrError->Col;
				length = ptrError->token->length;
				offset = ptrError->token->Offset;
				Error = ptrError->Message.ToChar();
			}
		}
		m_ErrorManager.ReportToFile("Error");
		return !m_ErrorManager.HasError();
	}

	void Compiler::Show()
	{
		try
		{
			fopen_s(&m_file, "SymTable.txt", "w+");
			fprintf(m_file, "Symbols Table:\n");
			int i = 0;
			for (auto p : m_TokenTable)
			{
				fprintf(m_file, "%s\t%d\n", (char*)p.second.id.c_str(), i);
				i++;
			}
			fclose(m_file);
			fopen_s(&m_file, (char*)(FileName + ".Phase1").c_str(), "w+");
			fprintf(m_file, "Token Streams:\n");
			//fs.open(FileName + ".Phase1", ios::out);
			//fs << "Token Streams:\n";
			for (auto p : *m_ArrayToken)
			{
				switch (p->Type)
				{
					case TokenType::num_int:
					{
											   IntToken* Data = (IntToken*) p;
											   //fs << Data->Row << "\t" << Data->Col << "\tnum_int\t" << Data->Value << endl;
											   fprintf(m_file, "%d\t%d\tnum_int\t%d\n", Data->Row , Data->Col , Data->Value);
					}
					break;
					case TokenType::num_float:
					{
												 DoubleToken* Data = (DoubleToken*) p;
												 //fs << Data->Row << "\t" << Data->Col << "\tnum_float\t" << Data->Value << endl;
												 fprintf(m_file, "%d\t%d\tnum_float\t%f\n", Data->Row, Data->Col, (float)Data->Value);
					}
					break;
					case TokenType::id:
					{
										  IdToken* Data = (IdToken*) p;
										  uint index = uint(p->Type);
										  //fs << Data->Row << "\t" << Data->Col << "\t" << NameToken[index] << "\t" << Data->Value << endl;
										  fprintf(m_file, "%d\t%d\t%s\t%s\n", Data->Row, Data->Col, (char*) NameToken[index].c_str(), Data->Value);
					}
					break;
					case TokenType::charliteral:
					{
												   StringToken* Data = (StringToken*) p;
												   //fs << Data->Row << "\t" << Data->Col << "\tliteral\t\'" << char(*Data->Value) << "\'\n";
												   fprintf(m_file, "%d\t%d\tliteral\t\'%c\'\n", Data->Row, Data->Col, char(*Data->Value));
					}
					break;
					case TokenType::strliteral:
					{
												  StringToken* Data = (StringToken*) p;
												  //fs << Data->Row << "\t" << Data->Col << "\tliteral\t" << string(Data->Value) << endl;
												  fprintf(m_file, "%d\t%d\tliteral\t%s\n", Data->Row, Data->Col, Data->Value);
					}
					break;
					default:
					{
							   uint index = uint(p->Type);
							   //fs << p->Row << "\t" << p->Col << "\t" << NameToken[index] << endl;
							   fprintf(m_file, "%d\t%d\t%s\n", p->Row, p->Col, (char*) NameToken[index].c_str());
					}
					break;
				}
			}
			fclose(m_file);
			m_file = nullptr;
		}
		catch (...)
		{
			cout << "error" << endl;
		}
	}
}