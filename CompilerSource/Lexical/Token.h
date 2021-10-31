#pragma once

#include"..\General\ErrorManager.h"

namespace CompilerPackage
{

	enum class TokenType
	{
		none,_class, _struct, _boolean, _break, _char, _cin, _cout, _else, _false, _float,_double, _for, _if,_elseif, _int, _main, _null,
		_private, _public, _return, _true, _void, _while, _const, and, assign, cbrac, charliteral, comma, div, dot, eof, eq,
		ge, gt, id, lbracket, le, lpran, lsh, lt, minus, mod, mul, neq, not, num_float, num_int, obrac, or, plus, pow,plusplus,minesmines,
		rbracket, rpran, rsh, coloncolon, semi, strliteral, _typeid, unknown
	};

	//Base Token structure
	struct Token
	{
		TokenType Type = TokenType::id;
		uint Row = 0, Col = 0;
		ushort length = 1;
		uint Offset;
		Token(uint offset):Offset(offset){}
		Token(TokenType type, uint row, uint column,ushort length=1)
		{
			Type = type;
			Row = row;
			Col = column;
			this->length = length;
		}
	};

	struct DataToken
	{
		std::string name;
		Token* token;
		DataToken(Token* token, std::string Name = "")
		{
			name = Name;
			this->token = token;
		}
		DataToken(std::string Name = "")
		{
			name = Name;
		}
		DataToken(char* Name)
		{
			name = Name;
		}
		inline operator std::string()
		{
			return name;
		}
		inline bool operator==(std::string Name)
		{
			return name == Name;
		}
	};

	struct StringToken :public Token
	{
		char* Value = nullptr;
		int Length = 0;
		StringToken(int length, uint offset) :Token(offset)
		{
			Value = new char[length];
			Length=length;
		}
		~StringToken()
		{
			SafeDeleteArray(Value);
		}
	};
	struct IdToken :public StringToken
	{
		char* Pointer = nullptr;
		IdToken(int length, uint offset) :StringToken(length, offset){}
		~IdToken()
		{
			SafeDeleteArray(Value);
		}
	};
	struct IntToken :public Token
	{
		int Value = 0;
		IntToken(uint offset) :Token(offset){}
	};
	
	struct DoubleToken :public Token
	{
		double Value = 0;
		DoubleToken(uint offset) :Token(offset){}
	};

	//#include<string>
	//#include<vector>
	//using namespace std;
	//template<class T>
	//struct node
	//{
	//	vector<node<T>> m_child;
	//	node<T>*parent=nullptr;
	//	int indexCurrent = 0;
	//	T data;
	//	node()
	//	{
	//	}
	//	node(T Data, node<T>* Parent=nullptr)
	//	{
	//		data = Data;
	//		parent = Parent;
	//	}
	//	node<T>* AddChild(T data)
	//	{
	//		m_child.push_back(node<T>(data,this));
	//		return &m_child[m_child.size() - 1];
	//	}
	//	void AddChild(node<T>* data)
	//	{
	//		m_child.push_back(data);
	//		data->parent = this;
	//	}
	//	node<T>* NextSibling()
	//	{
	//		indexCurrent++;
	//		if (indexCurrent > (m_child.size() - 1))
	//			return nullptr;
	//		return &m_child[indexCurrent];
	//	}
	//	node<T>* CurrentSibling()
	//	{
	//		if (indexCurrent > (m_child.size() - 1))
	//			return nullptr;
	//		return &m_child[indexCurrent];
	//	}
	//	node<T>* PrevSibling()
	//	{
	//		indexCurrent--;
	//		if (indexCurrent <0)
	//			return nullptr;
	//		return &m_child[indexCurrent];
	//	}
	//	node<T>* GetParent()
	//	{
	//		return parent;
	//	}
	//};
	//template<class T>
	//class tree
	//{
	//public:
	//	node<T>* m_head=nullptr;
	//	tree()
	//	{
	//	}
	//	node<T>* CreateTree(T DefaultTreeData)
	//	{
	//		m_head = new node<T>(DefaultTreeData,nullptr);
	//		return m_head;
	//	}
	//	
	//};
	//int main()
	//{
	//	tree<string> tr;
	//	auto pHnode=tr.CreateTree("Head");
	//	auto p2=pHnode->AddChild("1");
	//	p2->AddChild("2");
	//	return 0;
	//}

	enum class TypeValue
	{
		Int,Float,Char,String,Boolean,
	};

	struct Value
	{
		TypeValue type;
		Value(TypeValue t = TypeValue::Int) :type(t){}
	};

	struct IntValue :Value
	{
		int data=0;
		IntValue(int number) :Value(TypeValue::Int),data(number){};
	};

	struct FloatValue :Value
	{
		double data=0;
		FloatValue(double number) :Value(TypeValue::Float), data(number){};
	};

	struct CharValue :Value
	{
		char data=0;
		CharValue(char ch) :Value(TypeValue::Char), data(ch){};
	};

	struct StringValue :Value
	{
		std::string data="";
		StringValue(std::string str) :Value(TypeValue::String), data(str){};
	};

	struct BoolValue :Value
	{
		bool data = false;
		BoolValue(bool number) :Value(TypeValue::Boolean), data(number){};
	};

	enum class TypeDefinition
	{
		VarDecl, FunctionDecl, TypeDecl, ConstDecl,GlobalSection
	};

	struct Definition
	{
		std::string name;
		TypeDefinition typeDefinition;
		uint LineDefinition;
	};

	struct TypeDecleration;

	struct VariableDeclertion :public Definition
	{
		TypeDecleration* TypeVariable=nullptr;
		Value* data=nullptr;
		VariableDeclertion(){}
	};

	struct ScopeData :public Definition
	{
		std::map<std::string, TypeDecleration*> TypeScope;//type is define in this scope;
		std::map<std::string, TypeDecleration*> tempScope;//temp place for store types,variables,functions,... that must check for adding to upper scopes!
	};

	struct FunctionDeclertion :public ScopeData
	{
		TypeDecleration* TypeOfReturn;//type of value that this function return!
		::BaseEngine::Core::Array::DArrayPointer<VariableDeclertion> param;//parameter of this function
		std::map<std::string, VariableDeclertion*> Variables;//variables is define in this function!
	};

	enum class AccessTypeMember
	{
		Public,Protected,Private,
	};

	struct MemberDeclertion :public Definition
	{
		AccessTypeMember AccessType;
		std::map<std::string, VariableDeclertion*> Variables;//variables is define in this Class or struct!
		std::map<std::string, FunctionDeclertion*> Functions;//Functions is define in this Class or struct!
		std::map<std::string, TypeDecleration*> TypeMember;//Classes or struct is define in this Class or struct!
	};

	struct TypeDecleration :public ScopeData
	{
		::BaseEngine::Core::Array::DArrayPointer<MemberDeclertion> Members;//Members is define in this Class or struct!
		Token* token=nullptr;
		TypeDecleration(std::string Name)
		{
			name = Name;
			typeDefinition = TypeDefinition::TypeDecl;
		}
	};

	struct GlobalSection :public ScopeData
	{
		std::map<std::string, VariableDeclertion*> Variables;//variables is define in Global Section
		std::map<std::string, FunctionDeclertion*> Functions;//Functions is define in Global Section
		std::map<std::string, TypeDecleration*>	  Types;//Classes or struct is define in Global Section
	};

	struct SymbolsTable
	{
		std::string id;
		Token* Data = nullptr;//!? a Pointer to Token that is store in a token stream!
		//...
		SymbolsTable(std::string ID, Token* data)
		{
			id = ID;
			Data = data;
		}
		SymbolsTable(){}
	};
}