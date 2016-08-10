#ifndef AST_OPERATORS_HH_
 #define AST_OPERATORS_HH_

# include "ast.hh"

class ASTOp1Plus : public AST
{
public:
   ASTOp1Plus(Token token, AST*ast);

   virtual void accept(ASTVisitor& v) override;
};

class ASTOp1Minus : public AST
{
public:
   ASTOp1Minus(Token token, AST* ast);

   virtual void accept(ASTVisitor& v) override;
};

class ASTOp2Plus : public AST
{
public:
   ASTOp2Plus(Token token, AST* left, AST* right);

   virtual void accept(ASTVisitor& v) override;
};

class ASTOp2Minus : public AST
{
public:
   ASTOp2Minus(Token token, AST* left, AST* right);

   virtual void accept(ASTVisitor& v) override;
};

class ASTOp2Mul : public AST
{
public:
   ASTOp2Mul(Token token, AST* left, AST* right);

   virtual void accept(ASTVisitor& v) override;
};

class ASTOp2Div : public AST
{
public:
   ASTOp2Div(Token token, AST* left, AST* right);

   virtual void accept(ASTVisitor& v) override;
};

class ASTOp2Mod : public AST
{
public:
   ASTOp2Mod(Token token, AST* left, AST* right);

   virtual void accept(ASTVisitor& v) override;
};


#endif //!AST_OPERATORS_HH_
