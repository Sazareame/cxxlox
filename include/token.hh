#pragma once

#include "stdafx.hh"

enum class TokenType{
  LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
  COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

  BANG, BANG_EQUAL,
  EQUAL, EQUAL_EQUAL,
  GREATER, GREATER_EQUAL,
  LESS, LESS_EQUAL,

  IDENTIFIER, STRING, NUMBER,

  AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
  PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

  EEOF
};

static const char* enum_table[39] = {
	"LEFT_PAREN", "RIGHT_PAREN", "LEFT_BRACE", "RIGHT_BRACE",
	"COMMA", "DOT", "MINUS", "PLUS", "SEMICOLON", "SLASH", "STAR",

	"BANG", "BANG_EQUAL",
	"EQUAL", "EQUAL_EQUAL",
	"GREATER", "GREATER_EQUAL",
	"LESS", "LESS_EQUAL",

	"IDENTIFIER", "STRING", "NUMBER",

	"AND", "CLASS", "ELSE", "FALSE", "FUN", "FOR", "IF", "NIL", "OR",
	"PRINT", "RETURN", "SUPER", "THIS", "TRUE", "VAR", "WHILE",

	"EOF"
};

class Callable;
typedef std::shared_ptr<Callable> CallablePtr;

class Object{
	std::optional<std::variant<double, std::string, bool, CallablePtr> > data;

public:
	Object(): data(std::nullopt){}

	template<class T>
	Object(T _data): data(_data){}

	Object(std::string_view _data){
		data = std::string(_data);
	}


	Object(Object const& other): data(other.data){}

	std::stringstream to_stringstream() const;
	
	template<class T>
	bool has_type()const{
		if(!data.has_value()) return false;
		if(std::get_if<T>(&data.value())) return true;
		return false;
	}

	// this function is noexcept, safety check should be done in caller.
	double number()const noexcept{
		return std::get<double>(data.value());
	}

	// this function is noexcept, safety check should be done in caller.
	std::string string()const noexcept{
		return std::get<std::string>(data.value());
	}

	// this function is noexcept, safety check should be done in caller.
	bool boolean()const noexcept{
		return std::get<bool>(data.value());
	}

	// this function is noexcept, safety check should be done in caller.
	CallablePtr callable()const noexcept{
		return std::get<CallablePtr>(data.value());
	}

	bool logic_not()const noexcept{
		if(!data.has_value()) return true;
		if(has_type<bool>()) return !std::get<bool>(data.value());
		return false;
	}

	bool is_true()const noexcept{
		return !logic_not();
	}

};

template<> inline Object::Object(int _data){
	data = static_cast<double>(_data);
}

class FuncDefinition;
class Environ;

class Callable{
public:
	virtual ~Callable() = default;
	virtual Object call(std::vector<Object> const& args) = 0;
	virtual int arity()const = 0;
	virtual std::stringstream to_stringstream()const = 0;
};

typedef std::shared_ptr<Callable> CallablePtr;

class FuncType: public Callable{
	FuncDefinition* def;
	int _arity;
	std::shared_ptr<Environ> closure;

public:
	FuncType(FuncDefinition* _def, std::shared_ptr<Environ> _closure);

	Object call(std::vector<Object> const& args)override;

	int
	arity()const override{
		return _arity;
	}
	std::stringstream to_stringstream()const override;
};

template<class F>
class BuiltinFunc: public Callable{
	F body;
	int func_arity;

public:
	BuiltinFunc(F _body, int _arity): body(_body), func_arity(_arity){}

	int
	arity()const override{
		return func_arity;
	}

	Object
	call(std::vector<Object> const& args)override{
		return Object(body());
	}

	std::stringstream
	to_stringstream()const override{
		std::stringstream ss;
		ss << "<Builtin Function>";
		return ss;
	}
};

class Token{
public:
	TokenType type;
	std::string lexeme;
	Object literal;
	size_t line;

	Token(TokenType _type, std::string_view _lexeme, Object _literal, size_t _line):
		type(_type), lexeme(_lexeme), literal(_literal), line(_line){}

	Token(Token const& other):
		type(other.type), lexeme(other.lexeme), literal(other.literal), line(other.line){}

	Token&
	operator=(Token const& other){
		if(this == &other) return *this;
		type = other.type;
		lexeme = other.lexeme;
		literal = other.literal;
		line = other.line;
		return *this;
	}

	friend std::ostream&
	operator<<(std::ostream& os, Token const& self){
		os << enum_table[static_cast<int>(self.type)] << " " << self.lexeme << " " << self.literal.to_stringstream().str() << " " << self.line;
		return os;
	}
};

typedef std::shared_ptr<Token> TokenPtr;

static const std::unordered_map<std::string, TokenType> key_words{
	{"and",    TokenType::AND},
	{"class",  TokenType::CLASS},
	{"else",   TokenType::ELSE},
	{"false",  TokenType::FALSE},
	{"for",    TokenType::FOR},
	{"fun",    TokenType::FUN},
	{"if",     TokenType::IF},
	{"nil",    TokenType::NIL},
	{"or",     TokenType::OR},
	{"print",  TokenType::PRINT},
	{"return", TokenType::RETURN},
	{"super",  TokenType::SUPER},
	{"this",   TokenType::THIS},
	{"true",   TokenType::TRUE},
	{"var",    TokenType::VAR},
	{"while",  TokenType::WHILE},
};