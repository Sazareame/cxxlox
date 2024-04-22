#include "scanner.hh"
#include "lox.hh"

void Scanner::string(void){
	while(peek() != '"' && !is_at_end()){
		if(peek() == '\n') ++line;
		advance();
	}
	if(is_at_end()){
		std::stringstream msg;
		msg << "line " << line << ": unterminated string";
		Lox::report(msg.str());
		return;
	}
	advance();
	auto s = source.substr(start + 1, current - start - 2);
	add_token(TokenType::STRING, Object(s));
}

void Scanner::number(void){
	while(is_digit(peek())) advance();
	if(peek() == '.' && is_digit(peek_next())){
		advance();
		while(is_digit(peek())) advance();
	}
	double num = std::stod(std::string(source.substr(start, current - start)));
	add_token(TokenType::NUMBER, Object(num));
}

void Scanner::identifier(void){
	while(is_alphanumeric(peek())) advance();
	auto text = source.substr(start, current - start);
	auto p = key_words.find(std::string(text));
	if(p != key_words.end()) add_token(p->second);
	else add_token(TokenType::IDENTIFIER);
}

void Scanner::scan_token(void){
	char c = advance();
	switch(c){
		case '(': add_token(TokenType::LEFT_PAREN); break;
		case ')': add_token(TokenType::RIGHT_PAREN); break;
		case '{': add_token(TokenType::LEFT_BRACE); break;
		case '}': add_token(TokenType::RIGHT_BRACE); break;
		case ',': add_token(TokenType::COMMA); break;
		case '.': add_token(TokenType::DOT); break;
		case '-': add_token(TokenType::MINUS); break;
		case '+': add_token(TokenType::PLUS); break;
		case ';': add_token(TokenType::SEMICOLON); break;
		case '*': add_token(TokenType::STAR); break;
		case '!': add_token(is_match('=') ? TokenType::BANG_EQUAL : TokenType::BANG); break;
		case '=': add_token(is_match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); break;
		case '<': add_token(is_match('=') ? TokenType::LESS_EQUAL : TokenType::LESS); break;
		case '>': add_token(is_match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER); break;
		case '/':  // slash for single / and comment for double /
			if(is_match('/')) while(peek() != '\n' && !is_at_end()) advance();
			else add_token(TokenType::SLASH);
			break;
		case ' ':
		case '\r':
		case '\t':
			break;
		case '\n': ++line; break;
		case '"': string(); break;
	  default: 
			if(is_alphanumeric(c)){
				if(is_digit(c)) number();
				else identifier();
			}
			else{
				std::stringstream msg;
				msg << "line " << line << ": unexpected error.";
				Lox::report(msg.str());
			}
			break;
	}
}

std::vector<TokenPtr> Scanner::scan_tokens(){
	while(!is_at_end()){
		start = current;
		scan_token();
		if(Lox::had_error) return std::vector<TokenPtr>();
	}
	tokens.emplace_back(new Token(TokenType::EEOF, " ", Object(), line));
	return tokens;
}
