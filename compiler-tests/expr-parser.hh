#ifndef EXPR_PARSER_HH_
# define EXPR_PARSER_HH_

#include "parser.hh"

class ExprParser : public Parser
{

public:
   ExprParser(Scanner* scanner);

   virtual AST* exec() override;

private:
   AST* factor();
   AST* term();
   AST* expr();
};

#endif //!EXPR_PARSER_HH_
