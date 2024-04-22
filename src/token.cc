#include "token.hh"
#include "statement.hh"
#include "environment.hh"

extern std::shared_ptr<Environ> global_env;

std::stringstream
Object::to_stringstream() const{
	if(has_type<CallablePtr>()){
		auto& ptr = std::get<CallablePtr>(data.value());
		return ptr->to_stringstream();
	}
	std::stringstream ss;
	if(data.has_value()){
		auto visitor = [&ss](const auto& t){ss << t; };
		std::visit(visitor, data.value());
	} else ss << "Nil";
	return ss;
}

FuncType::FuncType(FuncDefinition* _def, std::shared_ptr<Environ> _closure): def(_def), closure(_closure){
	_arity = _def->params.size();
}

Object
FuncType::call(std::vector<Object> const& args){
	std::shared_ptr<Environ> previous = global_env;
	global_env = std::make_shared<Environ>(closure);
	for(int i = 0; i < _arity; ++i)
		global_env->define(def->params[i]->lexeme, args[i]);
	try{
		def->body->execute();
	} catch(Object ret){
		global_env = previous;
		return ret;
	}
	global_env = previous;
	return Object();
}

std::stringstream
FuncType::to_stringstream()const{
	std::stringstream ss;
	ss << "<function object " << def->name->lexeme << ">";
	return ss;
}


