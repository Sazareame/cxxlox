#pragma once

#include "stdafx.hh"
#include "token.hh"

class Expr;

class Resolver{
public:

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
	auto& scope = scopes.back();
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

};


