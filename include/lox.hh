#pragma once

#include "stdafx.hh"
#include "scanner.hh"
#include "parser.hh"


class Lox{
public:
  static bool had_error;

  static void run_file(char const* path){
    auto f = std::ifstream(path);
    if(!f.is_open()){
      puts("error during open source file.");
      exit(1);
    }
    std::stringstream ss;
    ss << f.rdbuf();
    run(ss.str());
		getchar();
  }

  static void run_prompt(){
    std::string line;
    std::cout << "> ";
    while(std::getline(std::cin, line)){
      run(line);
      had_error = false;
      std::cout << "> ";
    }
  }

private:
  static void run(std::string_view source){
		Scanner scanner(source);
		auto tokens = scanner.scan_tokens();
		Parser parser(tokens);
		try{
			auto stmts = parser.parse();
			// std::cout << expression->ast_print();
			// if parser error occured, stmts could contains nullptr, which may result in violation during interpreting.
			// also, interpreter is not allowed since the script already has syntax error.
			if(Lox::had_error) return;
			interpreter(stmts);
		}catch(std::string e){
			Lox::report(e);
		}
  }

public:
	static void report(std::string_view msg){
		had_error = true;
		std::cout << msg << std::endl;
	}
};