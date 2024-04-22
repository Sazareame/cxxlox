#pragma once

#include "stdafx.hh"
#include "expression.hh"

class Stmt{
public:
	virtual void execute() = 0;
	virtual ~Stmt() = default;
};

typedef std::shared_ptr<Stmt> StmtPtr;

class PrintStmt: public Stmt{
	ExprPtr expr;

public:
	PrintStmt(ExprPtr _expr): expr(_expr){}
	void execute()override{
		auto value = expr->evaluate();
		std::cout << value.to_stringstream().str() << std::endl;
	}
};

class ExprStmt: public Stmt{
	ExprPtr expr;

public:
	ExprStmt(ExprPtr _expr): expr(_expr){}
	void execute()override{
		expr->evaluate();
	}
};

class Var: public Stmt{
	TokenPtr name;
	ExprPtr expr;

public:
	Var(TokenPtr _name, ExprPtr _expr): name(_name), expr(_expr){}
	void execute()override;
};

class BlockStmt: public Stmt{
	std::vector<StmtPtr> stmts{};

public:
	BlockStmt(std::vector<StmtPtr> _stmts){
		stmts.swap(_stmts);
	}
	void execute()override;
};

class IfStmt: public Stmt{
	ExprPtr condition;
	StmtPtr then;
	StmtPtr els;

public:
	IfStmt(ExprPtr _condition, StmtPtr _then, StmtPtr _els): condition(_condition), then(_then), els(_els){}
	void execute()override;
};

class WhileStmt: public Stmt{
	ExprPtr condition;
	StmtPtr body;

public:
	WhileStmt(ExprPtr _condition, StmtPtr _body): condition(_condition), body(_body){}
	void execute()override;
};

class FuncDefinition: public Stmt{
	TokenPtr name;
	std::vector<TokenPtr> params;
	StmtPtr body;

public:
	friend class FuncType;
	FuncDefinition(TokenPtr _name, std::vector<TokenPtr> _params, StmtPtr _body): name(_name), body(_body){
		params.swap(_params);
	}
	void execute()override;
};

class RetStmt: public Stmt{
	TokenPtr keyword;
	ExprPtr ret_value;

public:
	RetStmt(TokenPtr _keyword, ExprPtr _value): keyword(_keyword), ret_value(_value){}
	void execute()override;
};

inline void interpreter(std::vector<StmtPtr> const& stmts){
	for(auto const& stmt : stmts)
		stmt->execute();
}
