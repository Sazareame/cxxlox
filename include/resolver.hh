#pragma once

#include "stdafx.hh"
#include "token.hh"
#include <memory>
#include <sstream>

class Expr;
class Stmt;

class Resolver{
public:

enum FunctionType{
	NONE, FUNCTION
};

static FunctionType current_func;
static std::deque<std::unordered_map<std::string, bool> > scopes;
static std::unordered_map<Expr*, unsigned> locals;

// push an empty frame into the stack
static void
begin_scope(){
	scopes.emplace_back(std::unordered_map<std::string, bool>());
}

// pop the frame stack
static void
end_scope(){
	scopes.pop_back();
}

// to declare an identifier in current frame.
// label the identifier false, which means it is undefined
static void
declare(TokenPtr name){
	if(scopes.empty()) return;
	auto& scope = scopes.back();
	if(scope.count(name->lexeme) == 1){
		error(name->line, name, "variable has already been declared.");
	}
	scope.insert({name->lexeme, false});
}

// change the label of identifier to true, if the stack is empty, do nothing.
// it should always be called after declared() is called
static void
define(TokenPtr name){
	if(scopes.empty()) return;
	// no safty check...
	scopes.back()[name->lexeme] = true;
}

static void
resolve_loacl_var(Expr* expr, TokenPtr name){
	for(int i = scopes.size() - 1; i >= 0; --i){
		if(scopes[i].count(name->lexeme) == 1){
			locals.insert({expr, scopes.size() - 1 - i});
		}
	}
}

static void
error(size_t line, TokenPtr token, std::string const& msg){
	std::stringstream ss;
	ss << "line " << line << " at " << token->lexeme << ": " << msg;
	throw ss.str();
}

static void
resolve(std::vector<std::shared_ptr<Stmt> > const& stmts);

};