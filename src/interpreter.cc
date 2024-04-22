#include "statement.hh"
#include "environment.hh"
#include "resolver.hh"

extern std::shared_ptr<Environ> global_env;

static std::string
op_error(TokenPtr op, std::string_view msg){
	std::stringstream ss;
	ss << "line " << op->line << ": Operands of " << op->lexeme << " " << msg;
	return ss.str();
}

static bool
check_addtion(TokenPtr op, Object const& lhs, Object const& rhs){
	if(lhs.has_type<double>() && rhs.has_type<double>()) return true;
	if(lhs.has_type<std::string>() && rhs.has_type<std::string>()) return false;
	throw op_error(op, "must be number or string.");
}

static void
check_operands(TokenPtr op, Object const& lhs, Object const& rhs){
	if(lhs.has_type<double>() && rhs.has_type<double>()) return;
	throw op_error(op, "must be number.");
}

static void
check_operand(TokenPtr op, Object const& hs){
	if(hs.has_type<double>()) return;
	throw op_error(op, "must be number.");
}

Object
Binary::evaluate(){
	auto lhs = left->evaluate();
	auto rhs = right->evaluate();
	if(oper->type == TokenType::PLUS){
		if(check_addtion(oper, lhs, rhs))
			return Object(lhs.number() + rhs.number());
		else
			return Object(lhs.string() + rhs.string());
	}
	check_operands(oper, lhs, rhs);
	switch(oper->type){
		case TokenType::MINUS: return Object(lhs.number() - rhs.number()); break;
		case TokenType::SLASH: return Object(lhs.number() / rhs.number()); break;
		case TokenType::STAR: return Object(lhs.number() * rhs.number()); break;
		case TokenType::GREATER: return Object(lhs.number() > rhs.number()); break;
		case TokenType::GREATER_EQUAL: return Object(lhs.number() >= rhs.number()); break;
		case TokenType::LESS: return Object(lhs.number() < rhs.number()); break;
		case TokenType::LESS_EQUAL: return Object(lhs.number() <= rhs.number()); break;
		case TokenType::EQUAL_EQUAL: return Object(lhs.number() == rhs.number()); break;
		case TokenType::BANG_EQUAL: return Object(lhs.number() != rhs.number()); break;
	}
}

Object
Literal::evaluate(){
	return lexeme;
}

Object
Grouping::evaluate(){
	return expression->evaluate();
}

Object
Unary::evaluate(){
	auto rhs = right->evaluate();
	if(oper->type == TokenType::MINUS){
		check_operand(oper, rhs);
		return Object(-rhs.number());
	}
	if(oper->type == TokenType::BANG){
		return Object(rhs.logic_not());
	}
}

Object
Variable::evaluate(){
	if(auto dist_p = Resolver::locals.find(this); dist_p != Resolver::locals.end()){
		return global_env->get_at(*dist_p, name->lexeme);
	} else{
		return global_env->get(name);
	}
}

Object
Assign::evaluate(){
	auto res = value->evaluate();
	global_env->assign(name, res);
	return res;
}

Object
Logical::evaluate(){
	auto left_value = left->evaluate();
	if(oper->type == TokenType::OR){
		if(left_value.is_true())
			return left_value;
	} else{
		if(!left_value.is_true())
			return left_value;
	}
	return right->evaluate();
}

Object
Call::evaluate(){
	auto _caller = callee->evaluate();
	std::vector<Object> args{};
	for(auto const& arg : arguments)
		args.push_back(arg->evaluate());
	CallablePtr caller = 0;
	if(_caller.has_type<CallablePtr>())
		caller = _caller.callable();
	else{
		std::stringstream ss;
		ss << "line " << paren->line << ": " << _caller.to_stringstream().str() << " is not callable type.";
		throw ss.str();
	}
	if(caller->arity() != arguments.size()){
		std::stringstream ss;
		ss << "line " << paren->line << ": " << "Expect " << caller->arity() << " arguments, but got " << arguments.size() << ".";
		throw ss.str();
	}
	return caller->call(args);
}

void
Var::execute(){
	Object value = Object();
	if(expr) value = expr->evaluate();
	global_env->define(name->lexeme, value);
}

void
BlockStmt::execute(){
	std::shared_ptr<Environ> previous = global_env;
	try{
		global_env = std::make_shared<Environ>(global_env);
		for(auto& stmt : stmts){
			stmt->execute();
		}
	} catch(std::string e){
		// var table should be recovered into its origin.
		global_env = previous;
		throw e;
	}
	global_env = previous;
}

void
IfStmt::execute(){
	if(condition->evaluate().is_true())
		then->execute();
	else if(els)
		els->execute();
	return;
}

void
WhileStmt::execute(){
	while(condition->evaluate().is_true())
		body->execute();
}

void
FuncDefinition::execute(){
	Object value = Object(std::shared_ptr<Callable>(new FuncType(this, global_env)));
	global_env->define(name->lexeme, value);
}

void
RetStmt::execute(){
	Object ret = ret_value ? ret_value->evaluate() : Object();
	throw ret;
}
