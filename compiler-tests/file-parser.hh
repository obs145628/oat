#ifndef FILE_PARSER_HH_
# define FILE_PARSER_HH_

#include "parser.hh"

class FileParser : public Parser
{

public:
   FileParser(Scanner* scanner);
   virtual AST* exec() override;

private:
   AST* program();
   AST* function_definition();

};

#endif //!FILE_PARSER_HH_
