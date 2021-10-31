#include"SyntaxAnalyzer.h"
#include<Engine.h>
using namespace BaseEngine;
namespace CompilerPackage
{
	SyntaxAnalyser::SyntaxAnalyser(Compiler* ref, char* ConfigFile)
	{
		m_Comiler = ref;
		m_lexc = m_Comiler->m_lexc;
		m_TokenTable = &m_Comiler->m_TokenTable;
		LoadParseTable(ConfigFile);

		//m_ArrayToken = &m_Comiler->m_ArrayToken;
	}
	SyntaxAnalyser::~SyntaxAnalyser()
	{

	}
	void SyntaxAnalyser::LoadParseTable(char* ConfigFile)
	{
		Map<int, string> m_ReverseColumnMap;//Reverse
		xml_document<> doc;    // character type defaults to char
		rapidxml::file<> xmlFile(ConfigFile); // Default template is char
		doc.parse<0>(xmlFile.data());
		rapidxml::xml_node<>* curNode = doc.first_node("Tables");
		if (curNode == nullptr)
			return;
		GoDownByName(curNode, "m_Symbol");
		if (curNode == nullptr)
			return;

#pragma region Get Symbols of File

		long SymbolCount = toInt(GetValueNode(curNode, "Count"));//<m_Symbol Count="">
		rapidxml::xml_node<>*child = curNode->first_node();
		long index;
		string temp;
		//<Symbol Index="" Name="" Type=""/>
		for (int i = 0; i < SymbolCount; i++)
		{
			index = toInt(GetValueNode(child, "Index"));
			string kk = GetValueNode(child, "Type");
			if ( kk== "0")
			{
				temp = "<";
				temp += GetValueNode(child, "Name");
				temp += ">";
				m_ColumnMap[temp] = index;
				m_ReverseColumnMap[index] = temp;
			}
			else
			{
				temp = GetValueNode(child, "Name");
				m_ColumnMap[temp] = index;
				m_ReverseColumnMap[index] = temp;
			}

			NextSelf(child);
		}

#pragma endregion

		child = NextSelfByName(curNode, "m_Production");//skip of <m_Group Count="">
		if (curNode == nullptr)
			return;

#pragma region Get Prodouctions of file

		long ProductionCount = toInt(GetValueNode(child, "Count"));
		//m_Production = new pair<string, string>[ProductionCount];
		GoDown(child);
		long ProductionSymbol;
		string end = "";
		rapidxml::xml_node<>*child2 = child;
		for (int i = 0; i < ProductionCount; i++)
		{
			//m_Production[i]=
			temp = m_ReverseColumnMap[toInt(GetValueNode(child, "NonTerminalIndex"))];
			ProductionSymbol = toInt(GetValueNode(child, "SymbolCount"));
			GoDown(child2);
			end = "";
			for (int j = 0; j < ProductionSymbol; j++)
			{
				end += m_ReverseColumnMap[toInt(GetValueNode(child2, "SymbolIndex"))];
				NextSelf(child2);
			}
			m_Production.push_back(new pair<string, pair<int, string>>(temp, pair<int, string>(ProductionSymbol, end)));
			child2 = NextSelf(child);
		}

#pragma endregion

		NextSelfByName(curNode, "LALRTable");
		if (curNode == nullptr)
			return;
#pragma region LALR Table that read from file.
		long RowCount = toInt(GetValueNode(curNode, "Count"));//
		m_ParseTable = new string*[RowCount];
		child = curNode->first_node();

		for (int i = 0; i < RowCount; i++)
		{
			m_ParseTable[i] = new string[SymbolCount];
			long CountAction = toInt(GetValueNode(child, "ActionCount"));//
			long index = toInt(GetValueNode(child, "Index"));//
			child2 = child->first_node();
			for (int j = 0; j < CountAction; j++)
			{
				long code = toInt(GetValueNode(child2, "Action"));
				string str = "";
				if (code == 1)
					str = "s";//shift
				else if (code == 2)
					str = "r";//reduce
				else if (code == 3)
					str = "g";//go to
				else if (code == 4)
					str = "a";//accept
				str += GetValueNode(child2, "Value");
				m_ParseTable[index][toInt(GetValueNode(child2, "SymbolIndex"))] = str;
				NextSelf(child2);
			}
			NextSelf(child);
		}
#pragma endregion

	}
	void* SyntaxAnalyser::Parse()
	{
		int RowNumber = 0, column = 0;
		string name = "", instruction;
		stack<int> StackState;
		StackState.push(0);
		stack<node<DataToken>*> symbol;
		bool Exit = false;
		bool ended = false;
		auto m_head = m_ParseTree.CreateTree("Head");
		//
		while (!ended)
		{
			Token* token = m_lexc->GetNextToken();
			Exit = false;
			#pragma region Convert Type To definition string
			switch (token->Type)
			{
				case TokenType::charliteral:
				name = "CharLitreral";
				break;
				case TokenType::strliteral:
				name = "StringLiteral";
				break;
				case TokenType::_char:
				case TokenType::_int:
				case TokenType::_float:
				case TokenType::_double:
				case TokenType::_boolean:
				case TokenType::_void:
				case TokenType::_typeid:
				case TokenType::_main:
				case TokenType::id:
				name = "id";
				break;
				case TokenType::num_float:
				name = "FNumber";
				break;
				case TokenType::num_int:
				name = "Number";
				break;
				case TokenType::eof:
				name = "EOF";//$
				break;
				default:
				name = NameOfToken[uint(token->Type)];
				break;
			}
			#pragma endregion

			while (!Exit)
			{
				column = m_ColumnMap[name];
				instruction = m_ParseTable[RowNumber][column];
				if (instruction == "")
				{
					//do one work;
					String Message = "Syntax error : ...";

					m_Comiler->m_ErrorManager.Add(new LexicalAnalyserError(Message, 0, token->Row, token->Col, token->length,token));
					ended = true;
					Exit = true;
				}
				switch (instruction[0])
				{
				#pragma region Shift State
					case 's':
					{
						auto nd = new node<DataToken>(name);

						switch (token->Type)
						{
							case TokenType::num_int:
							{
								IntToken* Data = (IntToken*) token;
								//fs << Data->Row << "\t" << Data->Col << "\tnum_int\t" << Data->Value << endl;
								nd->AddChild(DataToken( token,to_string(Data->Value)));
							}
							break;
							case TokenType::num_float:
							{
								DoubleToken* Data = (DoubleToken*) token;
								nd->AddChild(DataToken(token, to_string(Data->Value)));
							}
							break;
							case TokenType::_char:
							case TokenType::_int:
							case TokenType::_float:
							case TokenType::_double:
							case TokenType::_boolean:
							case TokenType::_void:
							case TokenType::_typeid:
							case TokenType::_main:
							case TokenType::id:
							{
								IdToken* Data = (IdToken*) token;
								nd->AddChild(DataToken(token, Data->Value));
							}
							break;
							case TokenType::charliteral:
							{
								StringToken* Data = (StringToken*) token;
								nd->AddChild(DataToken(token, "\'" + string(Data->Value) + "\'"));
							}
							break;
							case TokenType::strliteral:
							{
								StringToken* Data = (StringToken*) token;
								nd->AddChild(DataToken(token, "\"" + string(Data->Value) + "\""));
							}
							break;

						}
						symbol.push(nd);
						StackState.push(RowNumber = toInt(instruction.data() + 1));
						Exit = true;
					}
					break;
#pragma endregion

				#pragma region Reduce State
					case 'r':
					{
						auto LeftPro = m_Production[toInt(instruction.data() + 1)];
						auto m_node = new node<DataToken>(LeftPro->first);
						for (int i = LeftPro->second.first - 1; i >= 0; i--)
						{
							StackState.pop();
							m_node->AddChild(symbol.top());
							symbol.pop();
						}
						symbol.push(m_node);
						RowNumber = StackState.top();
						column = m_ColumnMap[LeftPro->first];
						instruction = m_ParseTable[RowNumber][column];
						StackState.push(RowNumber = toInt(instruction.data() + 1));
						break;
					}
				#pragma endregion

					case 'g':
					StackState.push(RowNumber = toInt(instruction.data() + 1));
					Exit = true;
					break;
					case 'e':
					break;
					case 'a':
					Exit = ended = true;
					break;
				}
			}
			//NameOfToken[uint(token->Type)];
		}
		for (; symbol.size() > 0;)
		{
			m_head->AddChild(symbol.top());
			symbol.pop();
		}
		return 0;

	}
}
//every programming language make for special work, and we must dedicate for use one.