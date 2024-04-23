#include "lox.hh"
#include "environment.hh"
#include "resolver.hh"

/*
In the rust implemention, `Object` type can be implemented through Enum, while in cc I have to use std::optional<std::varaint>. It is awful that my
Object type is so heavy, that is because its member, `Token` type, takes memory it should not take. In fact, since the source code is scanned into vector
of tokens, we could use its reference by wrapping it with share_ptr etc., which will reduce the additional cost. However, that is a hard work for me, meanwhile
the entire code perhaps need to be refactored. I shall probablely jump into it when I am not so busy as I am nowadays.
*/

/*
Well, I do have time to use smart pointer to replace those Token which should not be constructed.
*/

bool Lox::had_error = false;

std::shared_ptr<Environ> global_env{new Environ()};

decltype(Resolver::scopes) Resolver::scopes{};
decltype(Resolver::locals) Resolver::locals{};

auto builtin_clock_body = [](){return static_cast<double>(time(0));};
auto builtin_clock = Object(std::shared_ptr<Callable>(new BuiltinFunc(builtin_clock_body, 0)));


signed main(int argc, char const** argv){
	global_env->define("clock", builtin_clock);
  if(argc == 1) Lox::run_prompt();
  else if(argc == 2) Lox::run_file(argv[1]);
  else{
    puts("invalid input args.");
    exit(1);
  }
  return 0;
}