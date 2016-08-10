#ifndef AST_STATEMENTS_HH_
# define AST_STATEMENTS_HH_

# include "ast.hh"

class ASTStatementsList : public AST
{
public:
   ASTStatementsList(Token token, std::vector<AST*> list);

   virtual void accept(ASTVisitor& v) override;
};

class ASTStatementEmpty : public AST
{
public:
   ASTStatementEmpty(Token token);

   virtual void accept(ASTVisitor& v) override;
};

class ASTStatementLet : public AST
{
public:
   ASTStatementLet(Token token, AST* left, AST* right);

   virtual void accept(ASTVisitor& v) override;
};

class ASTStatementPrint : public AST
{
public:
   ASTStatementPrint(Token token, AST* ast);

   virtual void accept(ASTVisitor& v) override;
};

class ASTStatementReturn : public AST
{
public:
   ASTStatementReturn(Token token, AST* ast);

   virtual void accept(ASTVisitor& v) override;
};




#endif //!AST_STATEMENTS_HH_
