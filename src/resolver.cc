#include "resolver.hh"
#include "statement.hh"
#include "expression.hh"

void
BlockStmt::resolve(){
	Resolver::begin_scope();
	for(auto const& stmt : stmts)
		stmt->resolve();
	Resolver::end_scope();
}

void
Var::resolve(){
	Resolver::declare(name);
	if(expr)
		expr->resolve();
	Resolver::define(name);
}

void
Variable::resolve(){
	if(!Resolver::scopes.empty() && Resolver::scopes.back()[name->lexeme] == false){
		std::stringstream ss;
		ss << name->lexeme << " is declared but undefined.";
		throw ss.str();
	}

	Resolver::resolve_loacl_var(this, name);
}

void
Assign::resolve(){
	value->resolve();
	Resolver::resolve_loacl_var(this, name);
}

void
FuncDefinition::resolve(){
	Resolver::declare(name);
	Resolver::define(name);
	Resolver::begin_scope();
	for(auto const& param : params){
		Resolver::declare(param);
		Resolver::define(param);
	}
	body->resolve();
	Resolver::end_scope();
}

void
ExprStmt::resolve(){
	expr->resolve();
}

void
IfStmt::resolve(){
	condition->resolve();
	then->resolve();
	if(els) els->resolve();
}

void
PrintStmt::resolve(){
	if(expr) expr->resolve();
}

void
WhileStmt::resolve(){
	condition->resolve();
	body->resolve();
}

void
Binary::resolve(){
	left->resolve();
	right->resolve();
}

void
Call::resolve(){
	callee->resolve();
	for(auto const& arg : arguments){
		arg->resolve();
	}
}

void
Grouping::resolve(){
	expression->resolve();
}

void
Literal::resolve(){}

void
Logical::resolve(){
	left->resolve();
	right->resolve();
}

void
Unary::resolve(){
	right->resolve();
}
