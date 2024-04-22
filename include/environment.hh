#pragma once

#include "stdafx.hh"
#include "statement.hh"

class Environ{
public:
	std::unordered_map<std::string, Object> values{};
	std::shared_ptr<Environ> enclosing = nullptr;

	Environ() = default;
	Environ(std::shared_ptr<Environ> _enclosing): enclosing(_enclosing){}

	void
	define(std::string const& name, Object const& value){
		values.insert({name, value});
	}

	Object const& 
	get(TokenPtr name){
		if(auto res = values.find(name->lexeme); res != values.end())
			return res->second;
		if(enclosing) return enclosing->get(name);
		std::stringstream ss;
		ss << "Undefined variable " << name->lexeme << ".";
		throw ss.str();
	}

	void 
	assign(TokenPtr name, Object const& value){
		if(auto res = values.find(name->lexeme); res != values.end()){
			values[name->lexeme] = value;
			return;
		}
		if(enclosing){
			enclosing->assign(name, value);
			return;
		}
		std::stringstream ss;
		ss << "Undefined variable " << name->lexeme << ".";
		throw ss.str();
	}

	Object const&
	get_at(int dist, std::string const& name){
		return ancestor(dist)->values[name];
	}

private:

	Environ*
	ancestor(int dist){
		auto res = this;
		for(int i = 0; i < dist; ++i){
			res = res->enclosing.get();
		}
		return res;
	}
};