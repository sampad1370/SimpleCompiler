#pragma once
#include"../Compiler.h"

namespace CompilerPackage
{
	using Types = std::map<std::string, TypeDecleration*>;
	using Functions = std::map<std::string, FunctionDeclertion*>;
	using Variables = std::map<std::string, VariableDeclertion*>;
	using Parameters = ::BaseEngine::Core::Array::DArrayPointer<VariableDeclertion>;
	class Compiler;
	class LexicalAnalyzer;
	class SyntaxAnalyser;
	class SemanticAnalyzer
	{
		friend class Compiler;
		Compiler* m_Comiler = nullptr;
		::BaseEngine::Core::Tree::tree<DataToken>* m_treeParse;
		GlobalSection m_GlobalSection;
	public:
		SemanticAnalyzer(Compiler* ref);
		~SemanticAnalyzer();
		GlobalSection* ProcessTree();
		bool ProcessNode(::BaseEngine::Core::Tree::node<DataToken>*node, Types& types, Functions& functions, Variables& variables, ScopeData &scope);
		/*typeAccess=true	->	class
		typeAccess=false	->	struct*/
		bool TypeDeclerations(::BaseEngine::Core::Tree::node<DataToken>* ChildNode, Types& types, Functions* functions, Variables* variables, ScopeData& scope, bool typeAccess = false);
		/*typeAccess=true	->	class
		typeAccess=false	->	struct*/
		bool BodyType(::BaseEngine::Core::Tree::node<DataToken>* ChildNode, TypeDecleration &TypeCreated, bool typeAccess = false);
		bool FunctionDecleration(::BaseEngine::Core::Tree::node<DataToken>*ChildNode, Functions& functions, Variables* variables, ScopeData* scope);
		bool Params(::BaseEngine::Core::Tree::node<DataToken>*ChildNode, Parameters& StorePlace, ScopeData* scope, TypeDecleration* DefType = nullptr);
		bool MethodBody(::BaseEngine::Core::Tree::node<DataToken>*ChildNode, Parameters* params, Variables& variables, ScopeData* scope);
		bool StatementSequence(::BaseEngine::Core::Tree::node<DataToken>* ChildNode, Parameters* params, Variables& variables, ScopeData* scope);
		bool Assignment(::BaseEngine::Core::Tree::node<DataToken>* ChildNode, Parameters* params, Variables& variables, ScopeData* scope);

		bool LoopLValue(::BaseEngine::Core::Tree::node<DataToken>* ChildNode, Parameters* params, Variables& variables, ScopeData* scope, TypeDecleration* TypeIn, TypeDecleration* &TypeOut);
		bool LValue(::BaseEngine::Core::Tree::node<DataToken>* ChildNode, Parameters* params, Variables& variables, ScopeData* scope, TypeDecleration* &TypeFound);
		bool VarDecleration(::BaseEngine::Core::Tree::node<DataToken>*ChildNode, Variables& StorePlace, ScopeData* scope, TypeDecleration* DefType = nullptr);
		bool ConstDecleration(::BaseEngine::Core::Tree::node<DataToken>*ChildNode, Variables& StorePlace, ScopeData* scope);
		
		//! if find this type in parent scope,return true.
		//! in other case,return false and add this type to temp scope of parent!
		bool Type(::BaseEngine::Core::Tree::node<DataToken>*ChildNode, ScopeData* scope, TypeDecleration*& TypeFound);
		bool RValue(::BaseEngine::Core::Tree::node<DataToken>* ChildNode, Parameters* params, Variables& variables, ScopeData* scope, TypeDecleration* Rtype, bool HasFunctionType=false);
	};
}