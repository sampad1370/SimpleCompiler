#pragma once
#include"../Compiler.h"

namespace CompilerPackage
{
	using Types = map<string, TypeDecleration*>;
	using Functions = map<string, FunctionDeclertion*>;
	using Variables = map<string, VariableDeclertion*>;
	using Parameters = DArrayPointer<VariableDeclertion>;
	class Compiler;
	class LexicalAnalyzer;
	class SyntaxAnalyser;
	class SemanticAnalyzer
	{
		friend class Compiler;
		Compiler* m_Comiler = nullptr;
		tree<DataToken>* m_treeParse;
		GlobalSection m_GlobalSection;
	public:
		SemanticAnalyzer(Compiler* ref);
		~SemanticAnalyzer();
		GlobalSection* ProcessTree();
		bool ProcessNode(node<DataToken>*node, Types& types, Functions& functions, Variables& variables, ScopeData &scope);
		/*typeAccess=true	->	class
		typeAccess=false	->	struct*/
		bool TypeDeclerations(node<DataToken>* ChildNode, Types& types, Functions* functions, Variables* variables, ScopeData& scope, bool typeAccess = false);
		/*typeAccess=true	->	class
		typeAccess=false	->	struct*/
		bool BodyType(node<DataToken>* ChildNode, TypeDecleration &TypeCreated, bool typeAccess = false);
		bool FunctionDecleration(node<DataToken>*ChildNode, Functions& functions, Variables* variables, ScopeData* scope);
		bool Params(node<DataToken>*ChildNode, Parameters& StorePlace, ScopeData* scope, TypeDecleration* DefType = nullptr);
		bool MethodBody(node<DataToken>*ChildNode, Parameters* params, Variables& variables, ScopeData* scope);
		bool StatementSequence(node<DataToken>* ChildNode, Parameters* params, Variables& variables, ScopeData* scope);
		bool Assignment(node<DataToken>* ChildNode, Parameters* params, Variables& variables, ScopeData* scope);

		bool LoopLValue(node<DataToken>* ChildNode, Parameters* params, Variables& variables, ScopeData* scope, TypeDecleration* TypeIn, TypeDecleration* &TypeOut);
		bool LValue(node<DataToken>* ChildNode, Parameters* params, Variables& variables, ScopeData* scope, TypeDecleration* &TypeFound);
		bool VarDecleration(node<DataToken>*ChildNode, Variables& StorePlace, ScopeData* scope, TypeDecleration* DefType = nullptr);
		bool ConstDecleration(node<DataToken>*ChildNode, Variables& StorePlace, ScopeData* scope);
		
		//! if find this type in parent scope,return true.
		//! in other case,return false and add this type to temp scope of parent!
		bool Type(node<DataToken>*ChildNode, ScopeData* scope, TypeDecleration*& TypeFound);
		bool RValue(node<DataToken>* ChildNode, Parameters* params, Variables& variables, ScopeData* scope, TypeDecleration* Rtype, bool HasFunctionType=false);
	};
}