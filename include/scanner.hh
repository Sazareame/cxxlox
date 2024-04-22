#pragma once

#include "stdafx.hh"
#include "token.hh"

class Scanner{
	std::string_view source;
	std::vector<TokenPtr> tokens{};
	size_t start = 0;
	size_t current = 0;
	size_t line = 1;

	bool
	is_at_end(void)const{
		return current >= source.length();
	}

	char
	advance(void){
		return source[current++];
	}

	void
	add_token(TokenType type, Object literal){
		auto text = source.substr(start, current - start);
		tokens.emplace_back(new Token(type, text, literal, line));
	}

	void
	add_token(TokenType type){
		add_token(type, Object());
	}

	bool
	is_match(char expected){
		if(is_at_end()) return false;
		if(source[current] != expected) return false;
		++current;
		return true;
	}

	char
	peek(void)const{
		if(is_at_end()) return '\0';
		return source[current];
	}

	char
	peek_next(void)const{
		if(current + 1 >= source.length()) return '\0';
		return source[current + 1];
	}

	bool
	is_digit(char c)const{
		return c >= '0' && c <= '9';
	}

	bool
	is_alphanumeric(char c)const{
		if(is_digit(c)) return true;
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
	}

	void string(void);
	void number(void);
	void identifier(void);
	void scan_token(void);

public:
	Scanner(std::string_view _source): source(_source){}

	std::vector<TokenPtr> scan_tokens();
};