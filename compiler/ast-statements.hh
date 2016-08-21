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
   ASTStatementDefine(Token t, ASTSymbol* symbol, AST* value, bool isConst);
   ASTStatementDefine(Token t, ASTSymbol* symbol);

   virtual void accept(ASTVisitor& v) override;

   ASTSymbol* getSymbol() const;
   AST* getValue() const;
   bool hasValue() const;
   bool isConst() const;

private:
   bool _const;
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

class ASTStatementDo : public AST
{
public:
   ASTStatementDo(Token t, AST* condition,
                  ASTStatementsBlock* doStatement);

   virtual void accept(ASTVisitor& v) override;

   AST* getCondition() const;
   ASTStatementsBlock* getDoStatement() const;
};

class ASTStatementFor : public AST
{
public:
   ASTStatementFor(Token t, AST* init, AST* condition, AST* inc,
                  ASTStatementsBlock* forStatement);

   virtual void accept(ASTVisitor& v) override;

   AST* getInit() const;
   AST* getCondition() const;
   AST* getInc() const;
   ASTStatementsBlock* getForStatement() const;
};

class ASTStatementBreak : public AST
{
public:
   ASTStatementBreak(Token t);

   virtual void accept(ASTVisitor& v) override;
};

class ASTStatementContinue : public AST
{
public:
   ASTStatementContinue(Token t);

   virtual void accept(ASTVisitor& v) override;
};

#endif //!AST_STATEMENTS_HH_
