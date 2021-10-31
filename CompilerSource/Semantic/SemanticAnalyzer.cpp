#include"SemanticAnalyzer.h"
#include <rapidxml.hpp>
//#include<Engine.h>
using namespace BaseEngine;
namespace CompilerPackage
{
	SemanticAnalyzer::SemanticAnalyzer(Compiler* ref)
	{
		m_Comiler = ref;
		m_treeParse = ref->m_ParseTree;
		/*m_lexc = m_Comiler->m_lexc;
		m_TokenTable = &m_Comiler->m_TokenTable;
		LoadParseTable(ConfigFile);*/

		//m_ArrayToken = &m_Comiler->m_ArrayToken;
	}
	SemanticAnalyzer::~SemanticAnalyzer()
	{

	}

	GlobalSection* SemanticAnalyzer::ProcessTree()
	{
		m_GlobalSection.TypeScope["int"] = nullptr;
		m_GlobalSection.TypeScope["float"] = nullptr;
		m_GlobalSection.TypeScope["char"] = nullptr;
		m_GlobalSection.TypeScope["double"] = nullptr;
		m_GlobalSection.TypeScope["bool"] = nullptr;
		m_GlobalSection.TypeScope["void"] = nullptr;
		//m_GlobalSection = new GlobalSection();
		ProcessNode(m_treeParse->m_head->m_child[0], m_GlobalSection.Types, m_GlobalSection.Functions, m_GlobalSection.Variables, m_GlobalSection);
		/*for each (auto var1 in m_GlobalSection.Types)
		{
			for each (auto var in var1.second->tempScope)
			{
				if (!StlFind(var1.second->TypeScope, var.first))
				{
					if (var.second->token != nullptr)
					{
						String Message = "this type not defined!";
						auto token = var.second->token;
						m_Comiler->m_ErrorManager.Add(new LexicalAnalyserError(Message, 0, token->Row, token->Col, token->length, token));
					}
				}
			}
		}
		for each (auto var1 in m_GlobalSection.Functions)
		{
			for each (auto var in var1.second->tempScope)
			{
				if (!StlFind(var1.second->TypeScope, var.first))
				{
					if (var.second->token != nullptr)
					{
						String Message = "this type not defined!";
						auto token = var.second->token;
						m_Comiler->m_ErrorManager.Add(new LexicalAnalyserError(Message, 0, token->Row, token->Col, token->length, token));
					}
				}
			}
		}*/
		for each (auto var in m_GlobalSection.tempScope)
		{
			if (!StlFind(m_GlobalSection.TypeScope,var.first))
			{
				if (var.second->token != nullptr)
				{
					String Message = "this type not defined! :\"" + var.first + "\"";
					auto token = var.second->token;
					m_Comiler->m_ErrorManager.Add(new LexicalAnalyserError(Message, 0, token->Row, token->Col, token->length, token));
				}
			}
		}
		return &m_GlobalSection;
	}

	bool SemanticAnalyzer::ProcessNode(::BaseEngine::Core::Tree::node<DataToken>*nd, Types& types, Functions& functions, Variables& variables, ScopeData &scope)
	{
		if (nd->data == "<program>"||nd->data == "<Declerations>")
		{
			for (int i = nd->m_child.size() - 1; i >= 0; i--)
			{
				auto var = nd->m_child[i];
				ProcessNode(var, types, functions, variables, scope);
			}
		}
		else if (nd->data == "<VarDecleration>")
		{
			//Def = new VariableDeclertion;
			VarDecleration(nd, variables, &scope);
		}
		else if (nd->data == "<ConstDecleration>")
		{
			//Def = new VariableDeclertion;
			ConstDecleration(nd, variables, &scope);/**/
		}else if (nd->data == "<Function Declertion>")
		{
			//Def = new VariableDeclertion;
			FunctionDecleration(nd, functions, &variables, &scope);/**/
		}else if (nd->data == "<StructDecleration>")
		{
			//Def = new VariableDeclertion;
			TypeDeclerations(nd, types, &functions, &variables, scope);/**/
		}else if (nd->data == "<ClassDecleration>")
		{
			//Def = new VariableDeclertion;
			TypeDeclerations(nd, types, &functions, &variables, scope,true);/**/
		}
		return true;
	}

	bool SemanticAnalyzer::TypeDeclerations(::BaseEngine::Core::Tree::node<DataToken>* ChildNode, Types& types, Functions* functions, Variables* variables, ScopeData& scope, bool typeAccess)
{
		TypeDecleration* TypeDecl=nullptr;
		bool Res = true;
		
		for (int i = ChildNode->m_child.size() - 1; i >= 0; i--)
		{
			auto var = ChildNode->m_child[i];
			if (var->data == "id")
			{
				string name = var->m_child[0]->data;
				if (StlFind(scope.TypeScope, name))
				{
					String Message = "this type has been redefinition!";
					auto token = var->m_child[0]->data.token;
					m_Comiler->m_ErrorManager.Add(new LexicalAnalyserError(Message, 0, token->Row, token->Col, token->length, token));
					return true;
				}
				TypeDecl = new TypeDecleration(name);
				TypeDecl->token = var->m_child[0]->data.token;
				for each (auto var in scope.TypeScope)
				{
					TypeDecl->TypeScope.insert(var);
				}
				TypeDecl->TypeScope[name] = TypeDecl;
			}
			else if (var->data == "<BodyType>")
			{
				BodyType(var, *TypeDecl, typeAccess);
				break;
			}
		}
		types[TypeDecl->name]=TypeDecl;
		scope.TypeScope[TypeDecl->name] = TypeDecl;
		for each (auto var in TypeDecl->tempScope)
		{
			scope.tempScope.insert(var);
		}
		return Res;
	}
	
	bool SemanticAnalyzer::BodyType(::BaseEngine::Core::Tree::node<DataToken>* ChildNode, TypeDecleration &TypeCreated, bool typeAccess)
{
		bool Res = true,used=false;
		MemberDeclertion* mb = new MemberDeclertion();
		mb->AccessType = typeAccess ? AccessTypeMember::Private : AccessTypeMember::Public;

		for (int i = ChildNode->m_child.size() - 1; i >= 0; i--)
		{
			auto var = ChildNode->m_child[i];
			if (var->data == "<AccessType>")
			{
				if (used)
					mb = new MemberDeclertion();
				used = false;
				if (var->m_child[1]->data == "public")
					mb->AccessType= AccessTypeMember::Public;
				else if (var->m_child[1]->data == "private")
				{
					mb->AccessType = AccessTypeMember::Private;
				}
				else if (var->m_child[1]->data == "protected")
				{
					mb->AccessType = AccessTypeMember::Protected;
				}
			}
			else if (var->data == "<Declerations>")
			{
				Res&=ProcessNode(var, mb->TypeMember, mb->Functions, mb->Variables, TypeCreated);
				if (!used)
				TypeCreated.Members.push_back(mb);
				used = true;
			}
			else if (var->data == "<BodyType>")
			{
				if (var->m_child.size() == 0)
					break;
				ChildNode = var;
				i = ChildNode->m_child.size();
			}
		}
		return Res;
	}

	bool SemanticAnalyzer::FunctionDecleration(::BaseEngine::Core::Tree::node<DataToken>*ChildNode, Functions& functions, Variables* variables, ScopeData* scope)
	{
		Variables vb;
		FunctionDeclertion* FunDecl = new FunctionDeclertion();
		for each (auto var in scope->TypeScope)
		{
			FunDecl->TypeScope.insert(var);

		}
		bool Res = true;
		for (int i = ChildNode->m_child.size() - 1; i >= 0; i--)
		{
			auto var = ChildNode->m_child[i];
			if (var->data == "<Type>")
			{
				Res = Type(var->m_child[0], FunDecl, FunDecl->TypeOfReturn);
			}
			else if (var->data == "id")
			{
				FunDecl->name = var->m_child[0]->data;
				if (StlFind(functions, FunDecl->name))
				{
					String Message = "this Function has been redefinition!";
					auto token = var->m_child[0]->data.token;
					m_Comiler->m_ErrorManager.Add(new LexicalAnalyserError(Message, 0, token->Row, token->Col, token->length, token));
					return true;
				}
			}
			else if (var->data == "<Params>")
			{
				Res &= Params(var, FunDecl->param, FunDecl);
			}
			else if (var->data == "<MethodBody>")
			{
				MethodBody(var, &FunDecl->param, FunDecl->Variables, FunDecl);
				break;
			}
		}
		for each (auto var in FunDecl->tempScope)
		{
			scope->tempScope.insert(var);
		}
		functions[FunDecl->name] = FunDecl;
		return Res;
	}

	bool SemanticAnalyzer::Params(::BaseEngine::Core::Tree::node<DataToken>*ChildNode, Parameters& StorePlace, ScopeData* scope, TypeDecleration* DefType)
	{
		VariableDeclertion* VarDef = new VariableDeclertion();
		VarDef->typeDefinition = TypeDefinition::VarDecl;
		//StorePlace->
		bool Res = true;
		for (int i = ChildNode->m_child.size() - 1; i >= 0; i--)
		{
			auto var = ChildNode->m_child[i];
			if (var->data == "<Type>")
			{
				Res &= Type(var->m_child[0], scope, VarDef->TypeVariable);
			}
			else if (var->data == "id")
			{
				VarDef->name = var->m_child[0]->data;
				StorePlace.push_back(VarDef);
			}
			else if (var->data == "<Params>")
			{
				Res &= Params(var, StorePlace, scope, DefType);
			}
		}
		return Res;
	}

	bool SemanticAnalyzer::MethodBody(::BaseEngine::Core::Tree::node<DataToken>*ChildNode, Parameters* params, Variables& variables, ScopeData* scope)
	{
		bool Res = true;
		for (int i = ChildNode->m_child.size() - 1; i >= 0; i--)
		{
			auto var = ChildNode->m_child[i];
			if (var->data == "<MethodBody>")
			{
				Res &= MethodBody(var, params, variables, scope);
			}
			else if (var->data == "<StatementSequence>")
			{
				Res &= StatementSequence(var, params, variables, scope);
			}
		}
		//functions->push_back(FunDecl);
		return Res;
	}

	bool SemanticAnalyzer::StatementSequence(::BaseEngine::Core::Tree::node<DataToken>* ChildNode, Parameters* params, Variables& variables, ScopeData* scope)
	{
		bool Res = true;
		for (int i = ChildNode->m_child.size() - 1; i >= 0; i--)
		{
			auto var = ChildNode->m_child[i];
			if (var->data == "<Assignment>")
			{
				Assignment(var, params, variables, scope);
				//Res &= LValue(var, variables, scope, TODO);
				//do with Rvalue
			}
			else if (var->data == "<RValue>")
			{
				Res &= StatementSequence(var, params, variables, scope);
			}else if (var->data == "<Condition>")
			{
			} 
			else if (var->data == "<Experssion>")
			{
			}
			else if (var->data == "<VarDecleration>")
			{
				VarDecleration(var, variables, scope);
			}
		}
		//functions->push_back(FunDecl);
		return Res;
	}

	bool SemanticAnalyzer::Assignment(::BaseEngine::Core::Tree::node<DataToken>* ChildNode, Parameters* params, Variables& variables, ScopeData* scope)
	{
		bool Res = true;
		TypeDecleration* Ltype = nullptr;
		TypeDecleration* Rtype = nullptr;
		for (int i = ChildNode->m_child.size() - 1; i >= 0; i--)
		{
			auto var = ChildNode->m_child[i];
			if (var->data == "<LValue>")
			{
				LValue(var, params, variables, scope, Ltype);
			}
			else if (var->data == "<RValue>")
			{
				//RValue(ChildNode, params, variables, scope, Rtype);
			}
		}
		//functions->push_back(FunDecl);
		return Res;
	}

	bool SemanticAnalyzer::LValue(::BaseEngine::Core::Tree::node<DataToken>* ChildNode, Parameters* params, Variables& variables, ScopeData* scope, TypeDecleration* &TypeFound)
	{
		bool Res = true;
		VariableDeclertion* variable = nullptr;
		for (int i = ChildNode->m_child.size() - 1; i >= 0; i--)
		{
			auto var = ChildNode->m_child[i];
			if (var->data == "id")
			{
				string name = var->m_child[0]->data;
				if (!StlFind(variables, name))
				{
					for each(auto param in *params)
					{
						if (name == param->name)
						{
							variable = param;
							break;
						}
					}
					if (variable==nullptr)
					{
						//have error that use a variable that don't defined
						String Message = "this variable not defined :\""+name+"\"";
						auto token = var->m_child[0]->data.token;
						m_Comiler->m_ErrorManager.Add(new LexicalAnalyserError(Message, 0, token->Row, token->Col, token->length, token));
						return true;
					}

				}
				variable = variables[name];
				//Res &= LValue(var, variables, scope, TODO);
			}
			else if (var->data == "<LoopLValue>")
			{
				if (var->m_child.size()>0)
				Res &= LoopLValue(var, params, variables, scope, variable->TypeVariable, TypeFound);
			}
		}
		//functions->push_back(FunDecl);
		return Res;
	}

	bool SemanticAnalyzer::RValue(::BaseEngine::Core::Tree::node<DataToken>* ChildNode, Parameters* params, Variables& variables, ScopeData* scope, TypeDecleration* Rtype, bool HasFunctionType/*=false*/)
{
		bool Res = true;
		TypeDecleration* Ltype = nullptr;
		//TypeDecleration* Rtype = nullptr;
		for (int i = ChildNode->m_child.size() - 1; i >= 0; i--)
		{
			auto var = ChildNode->m_child[i];
			if (var->data == "<LValue>")
			{
				LValue(var, params, variables, scope, Ltype);
			}
			else if (var->data == "<RValue>")
			{
				RValue(ChildNode, params, variables, scope, Rtype);
			}
		}
		//functions->push_back(FunDecl);
		return Res;
	}

	bool SemanticAnalyzer::LoopLValue(::BaseEngine::Core::Tree::node<DataToken>* ChildNode, Parameters* params, Variables& variables, ScopeData* scope, TypeDecleration* TypeIn, TypeDecleration* &TypeOut)
	{
		bool Res = true;
		for (int i = ChildNode->m_child.size() - 1; i >= 0; i--)
		{
			auto var = ChildNode->m_child[i];
			if (var->data == "id")
			{
				string name = var->m_child[0]->data;
				bool finded=false;
				for each (auto member in TypeIn->Members)
				{
					if (StlFind(member->Variables, name))
					{
						if (member->AccessType != AccessTypeMember::Public)
						{
							String Message = "Access To Private Member";
							auto token = var->data.token;
							m_Comiler->m_ErrorManager.Add(new LexicalAnalyserError(Message, 0, token->Row, token->Col, token->length, token));
							TypeOut = nullptr;
							return true;
						}
						TypeIn= member->Variables[name]->TypeVariable;
						finded = true;
						break;
					}
				}
				if (!finded)
				{
					//have error that use a variable that don't defined
					String Message = "this type don't have this \""+name+"\" member";
					auto token = var->m_child[0]->data.token;
					m_Comiler->m_ErrorManager.Add(new LexicalAnalyserError(Message, 0, token->Row, token->Col, token->length, token));
					return true;
				}
			}
			else if (var->data == "<LoopLValue>")
			{
				Res &= LoopLValue(var, params, variables, scope, TypeIn, TypeOut);
			}
		}
		//functions->push_back(FunDecl);
		return Res;
	}

	bool SemanticAnalyzer::VarDecleration(::BaseEngine::Core::Tree::node<DataToken>*ChildNode, Variables& StorePlace, ScopeData* scope, TypeDecleration* DefType)
	{
		VariableDeclertion* VarDef = new VariableDeclertion();
		VarDef->typeDefinition = TypeDefinition::VarDecl;
		//StorePlace->
		bool Res = true;
		for (int i = ChildNode->m_child.size() - 1; i >= 0; i--)
		{
			auto var = ChildNode->m_child[i];
			if (var->data == "<Type>")
			{
				Res=Type(var->m_child[0], scope, VarDef->TypeVariable);
			}
			else if (var->data == "id")
			{
				VarDef->name = var->m_child[0]->data;
				if (StlFind(StorePlace, VarDef->name))
				{
					String Message = "this Variable has been redefinition!";
					auto token = var->m_child[0]->data.token;
					m_Comiler->m_ErrorManager.Add(new LexicalAnalyserError(Message, 0, token->Row, token->Col, token->length, token));
					return true;
				}
			}
			else if (var->data == "<opt Value>")
			{
				if (var->m_child.size() > 0)
				{
					//VarDef->name = var->m_child[0]->m_child[0]->m_child[0]->data;
					if (var->m_child[0]->m_child[0]->data == "Number")
					{
						VarDef->data = new IntValue(String::ToInt(var->m_child[0]->m_child[0]->m_child[0]->data.name.data()));
					}
					else if (var->m_child[0]->m_child[0]->data == "FNumber")
					{
						VarDef->data = new FloatValue(strtod(var->m_child[0]->m_child[0]->m_child[0]->data.name.data(), nullptr));
					}
					else if (var->m_child[0]->m_child[0]->data == "CharLitreral")
					{
						VarDef->data = new CharValue(var->m_child[0]->m_child[0]->m_child[0]->data.name[0]);
					}
					else if (var->m_child[0]->m_child[0]->data == "StringLiteral")
					{
						VarDef->data = new StringValue(var->m_child[0]->m_child[0]->m_child[0]->data);
					}
					else if (var->m_child[0]->m_child[0]->data == "true")
					{
						VarDef->data = new BoolValue(true);
					}
					else if (var->m_child[0]->m_child[0]->data == "false")
					{
						VarDef->data = new StringValue(false);
					}
				}
			}
			else if (var->data == "<LoopVarDec>")
			{
				if (DefType != nullptr)
				{
					VarDef->TypeVariable = DefType;
				}
				if (var->m_child.size() > 0)
				{
					StorePlace[VarDef->name] = VarDef;
					VarDecleration(var, StorePlace, scope, VarDef->TypeVariable);
				}
				else
					StorePlace[VarDef->name] = VarDef;
			}
		}
		return Res;
	}

	bool SemanticAnalyzer::ConstDecleration(::BaseEngine::Core::Tree::node<DataToken>*ChildNode, Variables& StorePlace, ScopeData* scope)
	{
		VariableDeclertion* VarDef = new VariableDeclertion();
		VarDef->typeDefinition = TypeDefinition::ConstDecl;
		//StorePlace->
		bool Res = true;
		for (int i = ChildNode->m_child.size() - 2; i >= 0; i--)
		{
			auto var = ChildNode->m_child[i];
			if (var->data == "<Type>")
			{
				Res = Type(var->m_child[0], scope, VarDef->TypeVariable);
			}
			else if (var->data == "id")
			{
				VarDef->name = var->m_child[0]->data;
			}
			else if (var->data == "<Literal>")
			{
				if (var->m_child.size() > 0)
				{
					//VarDef->name = var->m_child[0]->m_child[0]->m_child[0]->data;
					if (var->m_child[0]->data == "Number")
					{
						VarDef->data = new IntValue(String::ToInt(var->m_child[0]->m_child[0]->data.name.data()));
					}
					else if (var->m_child[0]->data == "FNumber")
					{
						VarDef->data = new FloatValue(strtod(var->m_child[0]->m_child[0]->data.name.data(), nullptr));
					}
					else if (var->m_child[0]->data == "CharLitreral")
					{
						VarDef->data = new CharValue(var->m_child[0]->m_child[0]->data.name[0]);
					}
					else if (var->m_child[0]->data == "StringLiteral")
					{
						VarDef->data = new StringValue(var->m_child[0]->m_child[0]->data);
					}
					else if (var->m_child[0]->data == "true")
					{
						VarDef->data = new BoolValue(true);
					}
					else if (var->m_child[0]->data == "false")
					{
						VarDef->data = new StringValue(false);
					}
				}
			}
			else if (var->data == ";")
			{
				StorePlace[VarDef->name] = VarDef;
			}
		}
		return true;
	}

	bool SemanticAnalyzer::Type(::BaseEngine::Core::Tree::node<DataToken>*ChildNode, ScopeData* scope, TypeDecleration* &TypeFound)
	{
		auto TypeName = ChildNode->m_child[0]->data;
		auto field = scope->TypeScope.find(TypeName);
		if (field != scope->TypeScope.end())
			TypeFound=field->second;
		else
		{
			if (StlFind(scope->tempScope,TypeName))
				TypeFound = scope->tempScope[TypeName];
			else
				scope->tempScope[TypeName] = TypeFound = new TypeDecleration(TypeName);
			TypeFound->token = ChildNode->m_child[0]->data.token;
			return false;
		}
		return true;
	}



}
//every programming language make for special work, and we must dedicate for use one.