#pragma once

#include "stdafx.hh"
#include "token.hh"

class Expr{
public:
	// virtual std::string ast_print() = 0;
	virtual Object evaluate() = 0;
	virtual void resolve() = 0;
	virtual ~Expr() = default;
};

typedef std::shared_ptr<Expr> ExprPtr;

// std::string parenthesis(std::string const& name, std::vector<ExprPtr> const& exprs);

class Binary: public Expr{
	ExprPtr left;
	TokenPtr oper;
	ExprPtr right;

public:
	Binary(ExprPtr _left, TokenPtr _oper, ExprPtr _right):
		left(_left), oper(_oper), right(_right){}

	// std::string ast_print() override{
	// 	return parenthesis(oper.lexeme, std::vector<ExprPtr>{left, right});
	// }

	Object evaluate()override;
	void resolve()override;
};

class Grouping: public Expr{
	ExprPtr expression;

public:
	Grouping(ExprPtr _expression): expression(_expression){}

	// std::string ast_print() override{
	// 	return parenthesis("group", std::vector<ExprPtr>{expression});
	// }
	Object evaluate()override;
	void resolve()override;
};

class Literal: public Expr{
	Object lexeme;

public:
	Literal(){
		lexeme = Object();
	}

	template<class T>
	Literal(T _lexeme_type){
		lexeme = Object(_lexeme_type);
	}

	// std::string ast_print() override{
	// 	return lexeme.to_stringstream().str();
	// }
	Object evaluate()override;
	void resolve()override;
};

class Unary: public Expr{
	TokenPtr oper;
	ExprPtr right;

public:
	Unary(TokenPtr _oper, ExprPtr _right):
		oper(_oper), right(_right){}

	// std::string ast_print() override{
	// 	return parenthesis(oper.lexeme, std::vector<ExprPtr>{right});
	// }
	Object evaluate()override;
	void resolve()override;
};

class Variable: public Expr{
	TokenPtr name;

public:
	Variable(TokenPtr _name): name(_name){};
	// std::string ast_print() override;

	Object evaluate()override;
	void resolve()override;

	TokenPtr get_name()const{
		return name;
	}
};

class Assign: public Expr{
	TokenPtr name;
	ExprPtr value;

public:
	Assign(TokenPtr _name, ExprPtr _value): name(_name), value(_value){}
	Object evaluate()override;
	void resolve()override;
};

class Logical: public Expr{
	ExprPtr left;
	TokenPtr oper;
	ExprPtr right;

public:
	Logical(ExprPtr _left, TokenPtr _oper, ExprPtr _right): left(_left), oper(_oper), right(_right){}
	Object evaluate()override;
	void resolve()override;
};

class Call: public Expr{
	ExprPtr callee;
	TokenPtr paren;
	std::vector<ExprPtr> arguments;

public:
	Call(ExprPtr _callee, TokenPtr _paren, std::vector<ExprPtr> _arguments): callee(_callee), paren(_paren){
		arguments.swap(_arguments);
	}
	Object evaluate()override;
	void resolve()override;
};

// inline std::string parenthesis(std::string const& name, std::vector<ExprPtr> const& exprs){
// 	std::string s = "(";
// 	s.append(name);
// 	for(const auto& expr : exprs){
// 		s.push_back(' ');
// 		s.append(expr->ast_print());
// 	}
// 	s.push_back(')');
// 	return s;
// }

