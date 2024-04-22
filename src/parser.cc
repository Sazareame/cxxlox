#include "parser.hh"
#include "lox.hh"

StmtPtr Parser::declaration(){
	StmtPtr res = nullptr;
	try{
		if(is_match(TokenType::VAR))
			res = var_decl();
		else if(is_match(TokenType::FUN))
			res = func_def();
		else
			res = statement();
	} catch(std::string e){
		std::cout << e << std::endl;
		synchronize();
		Lox::had_error = true;
		return nullptr;
	}
	return res;
}
 StmtPtr Parser::func_def(){
	auto name = consume(TokenType::IDENTIFIER, "Expect function name.");
	consume(TokenType::LEFT_PAREN, "Expect `(` before function name.");
	std::vector<TokenPtr> parameters{};
	if(!check(TokenType::RIGHT_PAREN)){
		do{
			if(parameters.size() >= 255)
				error(peek(), "Maximum argument number is limited to 255.");
			parameters.push_back(consume(TokenType::IDENTIFIER, "Expect parameter name."));
		} while(is_match(TokenType::COMMA));
	}
	consume(TokenType::RIGHT_PAREN, "Expect `)` after parameter list.");
	consume(TokenType::LEFT_BRACE, "Expect `{` before function body.");
	auto body = block();
	return std::shared_ptr<Stmt>(new FuncDefinition(name, parameters, body));
}

ExprPtr Parser::assignment(){
	auto expr = orr();
	if(is_match(TokenType::EQUAL)){
		auto equals = previous();
		auto value = assignment();
		if(auto var = std::dynamic_pointer_cast<Variable>(expr)){
			auto name = var->get_name();
			return std::shared_ptr<Expr>(new Assign(name, value));
		}
		throw error(equals, "Invalid assignment target.");
	}
	return expr;
}

ExprPtr Parser::orr(){
	auto expr = andd();
	while(is_match(TokenType::OR)){
		auto oper = previous();
		auto right = andd();
		expr = std::shared_ptr<Expr>(new Logical(expr, oper, right));
	}
	return expr;
}

ExprPtr Parser::andd(){
	auto expr = equlity();
	while(is_match(TokenType::AND)){
		auto oper = previous();
		auto right = equlity();
		expr = std::shared_ptr<Expr>(new Logical(expr, oper, right));
	}
	return expr;
}

StmtPtr Parser::for_stmt(){
	consume(TokenType::LEFT_PAREN, "Expect `(` after keyword for.");
	StmtPtr initializer = nullptr;
	if(is_match(TokenType::SEMICOLON));
	else if(is_match(TokenType::VAR))
		initializer = var_decl();
	else
		initializer = expr_stmt();

	ExprPtr condition = nullptr;
	if(!check(TokenType::SEMICOLON))
		condition = expression();
	consume(TokenType::SEMICOLON, "Expect `;` after condition expr.");

	ExprPtr action = nullptr;
	if(!check(TokenType::RIGHT_PAREN))
		action = expression();
	consume(TokenType::RIGHT_PAREN, "Expect `)` after clause.");

	StmtPtr body = statement();

	if(action){
		std::vector<StmtPtr> block{body, std::shared_ptr<Stmt>(new ExprStmt(action))};
		body = std::shared_ptr<Stmt>(new BlockStmt(block));
	}

	if(!condition)
		condition = std::shared_ptr<Expr>(new Literal(true));
	body = std::shared_ptr<Stmt>(new WhileStmt(condition, body));

	if(initializer)
		body = std::shared_ptr<Stmt>(new BlockStmt(std::vector<StmtPtr>{initializer, body}));

	return body;
}

ExprPtr Parser::equlity(){
	auto expr = comparision();
	ExprPtr right = nullptr;
	while(is_match(TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL)){
		auto operat = previous();
		right = comparision();
		expr = std::shared_ptr<Expr>(new Binary(expr, operat, right));
	}
	return expr;
}

ExprPtr Parser::comparision(){
	auto expr = term();
	ExprPtr right = nullptr;
	while(is_match(TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL)){
		auto operat = previous();
		right = term();
		expr = std::shared_ptr<Expr>(new Binary(expr, operat, right));
	}
	return expr;
}

ExprPtr Parser::term(){
	auto expr = factor();
	ExprPtr right = nullptr;
	while(is_match(TokenType::MINUS, TokenType::PLUS)){
		auto operat = previous();
		right = factor();
		expr = std::shared_ptr<Expr>(new Binary(expr, operat, right));
	}
	return expr;
}

ExprPtr Parser::factor(){
	auto expr = unary();
	ExprPtr right = nullptr;
	while(is_match(TokenType::SLASH, TokenType::STAR)){
		auto operat = previous();
		right = unary();
		expr = std::shared_ptr<Expr>(new Binary(expr, operat, right));
	}
	return expr;
}

ExprPtr Parser::unary(){
	if(is_match(TokenType::BANG, TokenType::MINUS)){
		auto operat = previous();
		auto right = unary();
		return std::shared_ptr<Expr>(new Unary(operat, right));
	}
	return call();
}

ExprPtr Parser::call(){
	auto expr = primary();
	for(;;){
		if(is_match(TokenType::LEFT_PAREN))
			expr = to_call(expr);
		else
			break;
	}
	return expr;
}

ExprPtr Parser::to_call(ExprPtr callee){
	std::vector<ExprPtr> args{};
	if(!check(TokenType::RIGHT_PAREN)){
		do{
			if(args.size() >= 255)
				error(peek(), "Maximum argument number is limited to 255");
			args.push_back(expression());
		} while(is_match(TokenType::COMMA));
	}
	
	auto paren = consume(TokenType::RIGHT_PAREN, "Expect `)` after arguments.");
	return std::shared_ptr<Expr>(new Call(callee, paren, args));
}

ExprPtr Parser::primary(){
	if(is_match(TokenType::FALSE))
		return std::shared_ptr<Expr>(new Literal(false));
	if(is_match(TokenType::TRUE))
		return std::shared_ptr<Expr>(new Literal(true));
	if(is_match(TokenType::NIL))
		return std::shared_ptr<Expr>(new Literal());
	if(is_match(TokenType::STRING, TokenType::NUMBER))
		return std::shared_ptr<Expr>(new Literal(previous()->literal));
	if(is_match(TokenType::LEFT_PAREN)){
		auto expr = expression();
		consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
		return std::shared_ptr<Expr>(new Grouping(expr));
	}
	if(is_match(TokenType::IDENTIFIER))
		return std::shared_ptr<Expr>(new Variable(previous()));
	throw error(peek(), "Expect expression.");
}

TokenPtr Parser::consume(TokenType type, std::string_view msg){
	if(check(type))
		return advance();
	else
		throw error(peek(), msg);
}

std::string Parser::error(TokenPtr token, std::string_view msg)const{
	std::stringstream ss;
	if(token->type == TokenType::EEOF)
		ss << "line " << token->line << " at the end: " << msg;
	else
		ss << "line " << token->line << " at " << token->lexeme << ": " << msg;
	return ss.str();
}