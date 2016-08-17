#ifndef AST_STATEMENTS_HH_
# define AST_STATEMENTS_HH_

# include "ast.hh"

class ASTSymbol;

class ASTStatementsBlock : public AST
{
public:
   ASTStatementsBlock(Token t, const std::vector<AST*> children);

   virtual void accept(ASTVisitor& v) override;
};

class ASTStatementEmpty : public AST
{
public:
   ASTStatementEmpty(Token t);

   virtual void accept(ASTVisitor& v) override;
};

class ASTStatementDefine : public AST
{
public:
   ASTStatementDefine(Token t, ASTSymbol* symbol, AST* value);

   virtual void accept(ASTVisitor& v) override;

   ASTSymbol* getSymbol() const;
   AST* getValue() const;
};

class ASTStatementReturn : public AST
{
public:
   ASTStatementReturn(Token t, AST* value);

   virtual void accept(ASTVisitor& v) override;

   AST* getValue() const;
};

class ASTStatementIf : public AST
{
public:
   ASTStatementIf(Token t, AST* condition, ASTStatementsBlock* ifStatement);
   ASTStatementIf(Token t, AST* condition, ASTStatementsBlock* ifStatement,
                  ASTStatementsBlock* elseStatement);

   virtual void accept(ASTVisitor& v) override;

   AST* getCondition() const;
   ASTStatementsBlock* getIfStatement() const;
   ASTStatementsBlock* getElseStatement() const;
   bool hasElseStatement() const;
};

class ASTStatementWhile : public AST
{
public:
   ASTStatementWhile(Token t, AST* condition,
                     ASTStatementsBlock* whileStatement);

   virtual void accept(ASTVisitor& v) override;

   AST* getCondition() const;
   ASTStatementsBlock* getWhileStatement() const;
};

#endif //!AST_STATEMENTS_HH_
