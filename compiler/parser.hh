#ifndef PARSER_HH_
# define PARSER_HH_

# include <string>
# include <vector>
# include "token.hh"

class AST;
class ASTStatementsBlock;
class ASTStatementEmpty;
class ASTStatementDefine;
class ASTStatementReturn;
class ASTStatementIf;
class ASTStatementWhile;
class ASTStatementDo;
class ASTStatementFor;
class ASTStatementBreak;
class ASTStatementContinue;
class ASTFunctionDef;
class ASTGlobalDef;
class ASTModule;

class Scanner;

class Parser
{
public:
   Parser(Scanner* scanner);

   AST* buildAST();


private:
   Scanner* _scanner;

   AST* expr();
   AST* expr_p14();
   AST* expr_p13();
   AST* expr_p12();
   AST* expr_p11();
   AST* expr_p10();
   AST* expr_p9();
   AST* expr_p8();
   AST* expr_p7();
   AST* expr_p6();
   AST* expr_p5();
   AST* expr_p4();
   AST* expr_p3();
   AST* expr_p2();
   AST* expr_p1();
   AST* expr_value();
   std::vector<AST*> expr_call_list();

   ASTStatementsBlock* statement();
   ASTStatementsBlock* statements_block();
   AST* statement_unique();
   AST* statement_body();
   ASTStatementEmpty* statement_empty();
   ASTStatementDefine* statement_define();
   ASTStatementReturn* statement_return();
   ASTStatementIf* statement_if();
   ASTStatementWhile* statement_while();
   ASTStatementDo* statement_do();
   ASTStatementFor* statement_for();
   ASTStatementBreak* statement_break();
   ASTStatementContinue* statement_continue();
   AST* statement_expr();

   ASTFunctionDef* function_def();

   ASTModule* module();
   void component(std::vector<AST*>& child);

   ASTGlobalDef* global_def();



   bool isEof() const;
   Token getToken() const;
   void next();
   void prev();
   void move(long dp);
   void tokenError(Token t, const std::string& message);

   TokenType getTokenType() const;
   bool isTokenOfType(TokenType t) const;
   bool isTokenOfType(const std::vector<TokenType>& types) const;
   Token peekToken();
};

#endif //!PARSER_HH_
