#include"LexicalAnalyzer.h"
#include<Engine.h>
using namespace BaseEngine;
namespace CompilerPackage
{
	LexicalAnalyzer::LexicalAnalyzer(Compiler* ref)
	{
		m_Comiler = ref;
		m_Buffer = m_Comiler->Buffer;
		m_TokenTable = &m_Comiler->m_TokenTable;
		m_LastTypeDef = m_Comiler->m_LastTypeDef;
		//m_ArrayToken = &m_Comiler->m_ArrayToken;
	}
	LexicalAnalyzer::~LexicalAnalyzer()
	{

	}
	Token* LexicalAnalyzer::GetNextToken()
	{
		
		Token* token=nullptr;
		
		char CurrentChar = m_Buffer[EndIndex], LastChar = 0;
		string str = "";
		bool quit = false;
		bool TempError = false;
		int tempbreak = 0;
		while (CurrentChar != -1&&!quit)
		{
			switch (state)
			{
				case 0:
				#pragma region State 0 //can get error
				switch (CurrentChar)
				{
					case '+':
					state = 1;
					break;
					case '-':
					state = 2;
					break;
					case '*':
					state = 3;
					break;
					case '/':
					state = 4;
					break;
					case '%':
					state = 5;
					break;
					case '=':
					state = 6;
					break;
					case '!':
					state = 7;
					break;
					case '|':
					state = 8;
					break;
					case '&':
					state = 9;
					break;
					case '<':
					state = 10;
					break;
					case '>':
					state = 11;
					break;
					case '(':
					state = 12;
					break;
					case ')':
					state = 13;
					break;
					case '[':
					state = 14;
					break;
					case ']':
					state = 15;
					break;
					case '{':
					state = 16;
					break;
					case '}':
					state = 17;
					break;
					case '.':
					state = 18;
					break;
					case ',':
					state = 19;
					break;
					case ':':
					state = 33;
					break;
					case ';':
					state = 20;
					break;
					case '\'':
					state = 23;
					break;
					case '\"':
					state = 24;
					break;
					case '$':
					case '_':
					state = 22;
					break;
					case ' ':
					case '\t':
					state = 25;
					break;
					case '\r':
					break;
					case '\n':
					state = 26;
					break;
					case 0:
					{
						token = new Token(BeginIndex);
						token->Type = TokenType::eof;
						token->Row = Row;
						token->Col = Col;
						quit = true;
					}
					break;
					default:
					if (CurrentChar < 0)
					{
						String Message = "error : undefined this character: \'";
						Message += String(CurrentChar);
						Message += "\'";
						token = new Token(BeginIndex);
						token->Type = TokenType::unknown;
						token->Row = Row;
						token->Col = Col;
						m_Comiler->m_ErrorManager.Add(new LexicalAnalyserError(Message, CurrentChar, Row, Col, 1,token));
						quit = true;
					}else
					if (isdigit(CurrentChar)) state = 21;
					else if(isalpha(CurrentChar)) state = 22;
					else
					{
						String Message = "error : undefined this character: \'";
						Message += String(CurrentChar);
						Message += "\'";
						token = new Token(BeginIndex);
						token->Type = TokenType::unknown;
						token->Row = Row;
						token->Col = Col;
						m_Comiler->m_ErrorManager.Add(new LexicalAnalyserError(Message, CurrentChar, Row, Col, 1, token));
						quit = true;
						//ReturnItem;
					}
					break;
				}
				#pragma endregion
				break;
				case 1:
				#pragma region State 1--> +
				token = new Token(BeginIndex);
				token->Row = Row;
				token->Col = Col;
				if (CurrentChar == '+')
					token->Type = TokenType::plusplus;
				else
				{
					token->Type = TokenType::plus;
					ReturnItem;
				}
				token->length = 2;
				quit = true;
				#pragma endregion
				break;
				case 2:
				#pragma region State 2--> -
				token = new Token(BeginIndex);
				token->Row = Row;
				token->Col = Col;
				if (CurrentChar == '-')
					token->Type = TokenType::minesmines;
				else
				{
					token->Type = TokenType::minus;
					ReturnItem;
				}
				token->length = 2;
				quit = true;
				#pragma endregion
				break;
				case 3:
				#pragma region State 3--> * or **
				token = new Token(BeginIndex);
				token->Row = Row;
				token->Col = Col;
				if (CurrentChar == '*')
					token->Type = TokenType::pow;
				else
				{
					token->Type = TokenType::mul;
					ReturnItem;
				}
				token->length = 2;
				quit = true;
				#pragma endregion
				break;
				case 4:
				#pragma region State 4--> / or /* or //*/
				switch (CurrentChar)
				{
					case '/':
					state = 28;
					break;
					case '*':
					state = 27;
					break;
					default:
					token = new Token(BeginIndex);
					token->Type = TokenType::div;
					token->Row = Row;
					token->Col = Col;
					ReturnItem;
					break;
				}
				#pragma endregion
				break;
				case 5:
				#pragma region State 5--> %
				token = new Token(BeginIndex);
				token->Type = TokenType::mod;
				token->Row = Row;
				token->Col = Col;
				ReturnItem;
				#pragma endregion
				break;
				case 6:
				#pragma region State 6--> = or ==
				token = new Token(BeginIndex);
				token->Row = Row;
				token->Col = Col;
				if (CurrentChar == '=')
					token->Type = TokenType::eq;
				else
				{
					token->Type = TokenType::assign;
					ReturnItem;
				}
				token->length = 2;
				quit = true;
				#pragma endregion
				break;
				case 7:
				#pragma region State 7--> ! or !=
				token = new Token(BeginIndex);
				token->Row = Row;
				token->Col = Col;
				if (CurrentChar == '=')
					token->Type = TokenType::neq;
				else
				{
					token->Type = TokenType::not;
					ReturnItem;
				}
				token->length = 2;
				quit = true;
				#pragma endregion
				break;
				case 8:
				#pragma region State 8--> ||  //can get error
				if (CurrentChar != '|')
				{
					String Message = "Error occurs in undefined character: \'|\'";
					token = new Token(BeginIndex);
					token->Type = TokenType::unknown;
					token->Row = Row;
					token->Col = Col;
					m_Comiler->m_ErrorManager.Add(new LexicalAnalyserError(Message, LastChar, Row, Col, 1, token));
					ReturnItem;
				}
				else
				{
					token = new Token(BeginIndex);
					token->Type = TokenType::or;
					token->Row = Row;
					token->Col = Col;
					token->length = 2;
				}
				quit = true;
				#pragma endregion
				break;
				case 9:
				#pragma region State 9--> &&  //can get error
				if (CurrentChar != '&')
				{
					String Message = "Error occurs in undefined character: \'&\'";
					token = new Token(BeginIndex);
					token->Type = TokenType::unknown;
					token->Row = Row;
					token->Col = Col;
					m_Comiler->m_ErrorManager.Add(new LexicalAnalyserError(Message, LastChar, Row, Col, 1, token));
					ReturnItem;//can get error
				}
				else
				{
					token = new Token(BeginIndex);
					token->Type = TokenType::and;
					token->Row = Row;
					token->Col = Col;
					token->length = 2;
				}
				quit = true;
				#pragma endregion
				break;
				case 10:
				#pragma region State 10--> < or <= or <<
				token = new Token(BeginIndex);
				token->Row = Row;
				token->Col = Col;
				if (CurrentChar == '=')
					token->Type = TokenType::le;
				else if (CurrentChar == '<')
					token->Type = TokenType::lsh;
				else
				{
					token->Type = TokenType::lt;
					ReturnItem;
				}
				token->length = 2;
				quit = true;
				#pragma endregion
				break;
				case 11:
				#pragma region State 11--> > or >= or <<
				token = new Token(BeginIndex);
				token->Row = Row;
				token->Col = Col;
				if (CurrentChar == '=')
					token->Type = TokenType::ge;
				if (CurrentChar == '>')
					token->Type = TokenType::rsh;
				else
				{
					token->Type = TokenType::gt;
					ReturnItem;
				}
				token->length = 2;
				quit = true;
				#pragma endregion
				break;
				case 12:
				#pragma region State 12--> (
				token = new Token(BeginIndex);
				token->Type = TokenType::lpran;
				token->Row = Row;
				token->Col = Col;
				ReturnItem;
				#pragma endregion
				break;
				case 13:
				#pragma region State 13--> )
				token = new Token(BeginIndex);
				token->Type = TokenType::rpran;
				token->Row = Row;
				token->Col = Col;
				ReturnItem;
				#pragma endregion
				break;
				case 14:
				#pragma region State 14--> [
				token = new Token(BeginIndex);
				token->Type = TokenType::lbracket;
				token->Row = Row;
				token->Col = Col;
				ReturnItem;
				#pragma endregion
				break;
				case 15:
				#pragma region State 15--> ]
				token = new Token(BeginIndex);
				token->Type = TokenType::rbracket;
				token->Row = Row;
				token->Col = Col;
				ReturnItem;
				#pragma endregion
				break;
				case 16:
				#pragma region State 16--> {
				token = new Token(BeginIndex);
				token->Type = TokenType::obrac;
				token->Row = Row;
				token->Col = Col;
				ReturnItem;
				#pragma endregion
				break;
				case 17:
				#pragma region State 17--> }
				token = new Token(BeginIndex);
				token->Type = TokenType::cbrac;
				token->Row = Row;
				token->Col = Col;
				ReturnItem;
				#pragma endregion
				break;
				case 18:
				#pragma region State 18--> .
				if (!isdigit(CurrentChar))
				{
					token = new Token(BeginIndex);
					token->Type = TokenType::dot;
					token->Row = Row;
					token->Col = Col;
					ReturnItem;
				}
				else
				{
					state = 29;
					EndIndex--;
				}
					
				#pragma endregion
				break;
				case 19:
				#pragma region State 19--> ,
				token = new Token(BeginIndex);
				token->Type = TokenType::comma;
				token->Row = Row;
				token->Col = Col;
				ReturnItem;
				#pragma endregion
				break;
				case 20:
				#pragma region State 20--> ;
				token = new Token(BeginIndex);
				token->Type = TokenType::semi;
				token->Row = Row;
				token->Col = Col;
				ReturnItem;
				#pragma endregion
				break;
				case 21:
				#pragma region State 21--> Integer number
				if (CurrentChar == '.')
				{
					state = 29;
				}
				else if (!isdigit(CurrentChar))
				{
					IntToken* token_ = new IntToken(BeginIndex);
					token_->Type = TokenType::num_int;
					token_->Row = Row;
					token_->Col = Col;
					char*p = new char[EndIndex - BeginIndex + 1];
					strncpy(p, m_Buffer + BeginIndex, EndIndex - BeginIndex);
					p[EndIndex - BeginIndex] = 0;
					token_->Value = atoi(p);
					token = token_;
					token->length = EndIndex - BeginIndex;
					SafeDeleteArray(p);
					ReturnItem;
				}
				#pragma endregion
				break;
				case 22:
				#pragma region State 22--> id
				if (!isalnum(CurrentChar) && CurrentChar != '_' && CurrentChar != '$')
				{
					temp = new char[EndIndex - BeginIndex+1];
					strncpy(temp, m_Buffer + BeginIndex, EndIndex - BeginIndex);
					temp[EndIndex - BeginIndex] = 0;
					string TempStr;
					TempStr.assign((const char*)temp);
					if (!HasCaseSensitive)
					std::transform(TempStr.begin(), TempStr.end(), TempStr.begin(), ::tolower);

					if (!PStlFind(m_TokenTable, TempStr))//not found so add to symbol table
					{
						IdToken* _token = new IdToken(EndIndex - BeginIndex + 1, BeginIndex);
						if (IsTypeDefinitionState)
						{
							_token->Type = TokenType::_typeid;
							IsTypeDefinitionState = false;
						}
						else
							_token->Type = TokenType::id;
						_token->Row = Row;
						_token->Col = Col;
						strncpy(_token->Value, TempStr.c_str(), EndIndex - BeginIndex);
						_token->Value[EndIndex - BeginIndex] = 0;
						_token->Pointer = _token->Value;
						token = _token;
						token->length = EndIndex - BeginIndex;
						m_TokenTable->operator[](TempStr) = SymbolsTable(TempStr, token);
						SafeDeleteArray(temp);
						ReturnItem;
					}
					else
					{
						IdToken*idp = ((IdToken*) m_TokenTable->operator[](TempStr).Data);
						IdToken *_token = new IdToken(EndIndex - BeginIndex + 1, BeginIndex);
						_token->Type=idp->Type;
						if (_token->Type <= TokenType::_struct)
							IsTypeDefinitionState = true;
						strncpy(_token->Value, TempStr.c_str(), EndIndex - BeginIndex);
						_token->Value[EndIndex - BeginIndex] = 0;
						_token->Pointer = _token->Value;
						_token->Row = Row;
						_token->Col = Col;
						token = _token;
						token->length = EndIndex - BeginIndex;
						SafeDeleteArray(temp);
						ReturnItem;
					}
				}
				#pragma endregion
				break;
				case 23:
				#pragma region State 23--> Character Literal   //can get error
				if (CurrentChar == '\'')
				{
					if ((EndIndex - BeginIndex) == 1)
					{
						String Message = "error : empty character constant.";
						token = new Token(BeginIndex);
						token->Type = TokenType::unknown;
						token->Row = Row;
						token->Col = Col;
						m_Comiler->m_ErrorManager.Add(new LexicalAnalyserError(Message, LastChar, Row, Col, 1, token));
						//ReturnItem;//can get error
					}
					else if( (EndIndex - BeginIndex) == 2)
					{
						StringToken* _token = new StringToken(1, BeginIndex);
						_token->Type = TokenType::charliteral;
						_token->Row = Row;
						_token->Col = Col;
						*_token->Value=LastChar;
						*(_token->Value + 1) = 0;
						token = _token;
						token->length = EndIndex - BeginIndex+1;
					}
					else
					{
						String Message = "error : character literal contain more than one character.";
						token = new Token(BeginIndex);
						token->Type = TokenType::unknown;
						token->Row = Row;
						token->Col = Col;
						token->length = EndIndex - BeginIndex+1;
						m_Comiler->m_ErrorManager.Add(new LexicalAnalyserError(Message, '\'', Row, Col, EndIndex - BeginIndex, token));
					}
					quit = true;
				}
				#pragma endregion
				break;
				case 24:
				#pragma region State 24--> String Literal   //can get error
				if (CurrentChar == '\"')
				{
					if ((EndIndex - BeginIndex) == 1)
					{
						String Message = "error : empty string literal.";
						token = new Token(BeginIndex);
						token->Type = TokenType::unknown;
						token->Row = Row;
						token->Col = Col;
						m_Comiler->m_ErrorManager.Add(new LexicalAnalyserError(Message, LastChar, Row, Col, 2, token));
						//ReturnItem;//can get error
					}
					else if (TempError)
					{
						String Message = "error : string literal must close at this line.";
						token = new Token(BeginIndex);
						token->Type = TokenType::unknown;
						token->Row = Row;
						token->Col = Col;
						token->length = EndIndex - BeginIndex+1;
						TempError = false;
						m_Comiler->m_ErrorManager.Add(new LexicalAnalyserError(Message, '\n', Row, Col, EndIndex - BeginIndex, token));
						Row++;
						Col = EndIndex - tempbreak;
						BeginIndex = EndIndex;
					}
					else
					{
						StringToken* _token = new StringToken(EndIndex - BeginIndex + 2, BeginIndex);
						_token->Type = TokenType::strliteral;
						_token->Row = Row;
						_token->Col = Col;
						strncpy(_token->Value, m_Buffer + BeginIndex, EndIndex - BeginIndex+1);
						_token->Value[EndIndex - BeginIndex+1] = 0;
						token = _token;
						token->length = EndIndex - BeginIndex+1;
					}
					quit = true;
				}
				else if (CurrentChar == 0)
				{
					String Message = "error : string literal must close at this line.";
					token = new Token(BeginIndex);
					token->Type = TokenType::unknown;
					token->Row = Row;
					token->Col = Col;
					m_Comiler->m_ErrorManager.Add(new LexicalAnalyserError(Message, '\n', Row, Col, EndIndex - BeginIndex, token));
					ReturnItem;//can get error
				}
				else if (CurrentChar == '\n')
				{
					TempError = true;
					tempbreak = EndIndex;
				}
				/*else if (CurrentChar == '\t' || CurrentChar == ' ')*/
				#pragma endregion
				break;
				case 25:
				#pragma region State 25--> Space,Tab
				BeginIndex = EndIndex;
				EndIndex--;
				state = 0;
				Col++;
				#pragma endregion
				break;
				case 26:
				#pragma region State 26--> Enter
				BeginIndex = EndIndex;
				EndIndex--;
				state = 0;
				Col=1;
				Row++;
				#pragma endregion
				break;
				case 27:
				#pragma region State 27--> /**/    //can get error
				if (LastChar == '*'&&CurrentChar == '/')
					state = 0;
				else if (CurrentChar == '\n')
				{
					Col = 1;
					Row++;
				}
				else if(CurrentChar == 0)
				{
					String Message = "error : comment must closed before end of file.";
					token = new Token(BeginIndex);
					token->Type = TokenType::unknown;
					token->Row = Row;
					token->Col = Col;
					m_Comiler->m_ErrorManager.Add(new LexicalAnalyserError(Message, LastChar, Row, Col, EndIndex - BeginIndex, token));
					ReturnItem;//can get error
				}
				#pragma endregion
				break;
				case 28:
				#pragma region State 28--> //
				if (CurrentChar == '\n')
				{
					state = 0;
					continue;
				}
				#pragma endregion
				break;
				case 29:
				#pragma region State 29--> Real number  //can get error
				if (!isdigit(CurrentChar))
				{
					String Message = "error : format of real number is incorrect.";
					token = new Token(BeginIndex);
					token->Type = TokenType::unknown;
					token->Row = Row;
					token->Col = Col;
					m_Comiler->m_ErrorManager.Add(new LexicalAnalyserError(Message, LastChar, Row, Col, EndIndex - BeginIndex, token));
					ReturnItem;//can get error
				}
				else
					state = 32;
				#pragma endregion
				break;
				case 30:
				#pragma region State 30--> Real number with exponent1   //can get error
				if (!isdigit(CurrentChar))
				{
					if (CurrentChar != '+'&&CurrentChar != '-')
					{
						if (LastChar == 'E' || LastChar == 'e')
						{
							String Message = "error : after 'e' in real number format must have one digit.";
							token = new Token(BeginIndex);
							token->Type = TokenType::unknown;
							token->Row = Row;
							token->Col = Col;
							m_Comiler->m_ErrorManager.Add(new LexicalAnalyserError(Message, LastChar, Row, Col, EndIndex - BeginIndex, token));
							ReturnItem;//can get error
						}
						else
						{
							DoubleToken* token_ = new DoubleToken(BeginIndex);
							token_->Type = TokenType::num_float;
							token_->Row = Row;
							token_->Col = Col;
							char*p = new char[EndIndex - BeginIndex + 1];
							strncpy(p, m_Buffer + BeginIndex, EndIndex - BeginIndex);
							p[EndIndex - BeginIndex] = 0;
							token_->Value = atof(p);
							token = token_;
							token->length = EndIndex - BeginIndex;
							SafeDeleteArray(p);
							ReturnItem;
						}
					}
					else
						state = 31;
					/*else
					{
						m_Error.Message = "error : after 'e' in real number format must have one digit.";
						m_Error.Character = LastChar;
						m_Error.Row = Row;
						m_Error.Col = Col;
						ReturnItem;
					}*/
				}
				#pragma endregion
				break;
				case 31:
				#pragma region State 31--> Real number with exponent2 that has + or -    //can get error
				if (!isdigit(CurrentChar))
				{
					if (LastChar == '+'||LastChar == '-')
					{
						String Message = "error : after 'e' in real number format must have one number.";
						token = new Token(BeginIndex);
						token->Type = TokenType::unknown;
						token->Row = Row;
						token->Col = Col;
						m_Comiler->m_ErrorManager.Add(new LexicalAnalyserError(Message, LastChar, Row, Col, EndIndex - BeginIndex, token));
						ReturnItem;//can get error
					}
					else
					{
						DoubleToken* token_ = new DoubleToken(BeginIndex);
						token_->Type = TokenType::num_float;
						token_->Row = Row;
						token_->Col = Col;
						char*p = new char[EndIndex - BeginIndex + 1];
						strncpy(p, m_Buffer + BeginIndex, EndIndex - BeginIndex);
						p[EndIndex - BeginIndex] = 0;
						token_->Value = atof(p);
						token = token_;
						token->length = EndIndex - BeginIndex;
						SafeDeleteArray(p);
						ReturnItem;
					}
				}
				#pragma endregion
				break;
				case 32:
				#pragma region State 32--> Real number2
				if (!isdigit(CurrentChar))
				{
					if (CurrentChar != 'E' && CurrentChar != 'e')
					{
						DoubleToken* token_ = new DoubleToken(BeginIndex);
						token_->Type = TokenType::num_float;
						token_->Row = Row;
						token_->Col = Col;
						char*p = new char[EndIndex - BeginIndex + 1];
						strncpy(p, m_Buffer + BeginIndex, EndIndex - BeginIndex);
						p[EndIndex - BeginIndex] = 0;
						token_->Value = atof(p);
						token = token_;
						token->length = EndIndex - BeginIndex;
						SafeDeleteArray(p);
						ReturnItem;
					}
					else
						state = 30;
				}
				#pragma endregion
				break;

				case 33:
				#pragma region State 32 --> :
				token = new Token(BeginIndex);
				token->Type = TokenType::coloncolon;
				token->Row = Row;
				token->Col = Col;
				ReturnItem;
				#pragma endregion

				break;
			}
			EndIndex++;
			LastChar = CurrentChar;
			CurrentChar = m_Buffer[EndIndex];
		}
		state = 0;
		//Col--;
		Col += EndIndex - BeginIndex;
		BeginIndex = EndIndex;
		return token;
	}
}