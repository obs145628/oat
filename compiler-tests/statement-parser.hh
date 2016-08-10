#ifndef STATEMENT_PARSER_HH_
# define STATEMENT_PARSER_HH_

# include "parser.hh"

class StatementParser : public Parser
{

public:
   StatementParser(Scanner* scanner);

   virtual AST* exec() override;

private:
   AST* statement();
   AST* body();
   AST* let();
   AST* print();
   AST* statement_return();

};

#endif //!STATEMENT_PARSER_HH_
