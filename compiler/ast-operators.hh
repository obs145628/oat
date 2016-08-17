#ifndef AST_OPERATORS_HH_
 #define AST_OPERATORS_HH_

# include "ast.hh"

class ASTOpUnary : public AST
{
public:
   ASTOpUnary(Token token, AST* op);

   AST* op() const;
};

class ASTOpBinary : public AST
{
public:
   ASTOpBinary(Token token, AST* op1, AST* op2);

   AST* op1() const;
   AST* op2() const;
};

class ASTOp1Plus : public ASTOpUnary
{
public:
   ASTOp1Plus(Token token, AST* op);

   virtual void accept(ASTVisitor& v) override;
};

class ASTOp1Minus : public ASTOpUnary
{
public:
   ASTOp1Minus(Token token, AST* ast);

   virtual void accept(ASTVisitor& v) override;
};

class ASTOp2Plus : public ASTOpBinary
{
public:
   ASTOp2Plus(Token token, AST* op1, AST* op2);

   virtual void accept(ASTVisitor& v) override;
};

class ASTOp2Minus : public ASTOpBinary
{
public:
   ASTOp2Minus(Token token, AST* op1, AST* op2);

   virtual void accept(ASTVisitor& v) override;
};

class ASTOp2Mul : public ASTOpBinary
{
public:
   ASTOp2Mul(Token token, AST* op1, AST* op2);

   virtual void accept(ASTVisitor& v) override;
};

class ASTOp2Div : public ASTOpBinary
{
public:
   ASTOp2Div(Token token, AST* op1, AST* op2);

   virtual void accept(ASTVisitor& v) override;
};

class ASTOp2Mod : public ASTOpBinary
{
public:
   ASTOp2Mod(Token token, AST* op1, AST* op2);

   virtual void accept(ASTVisitor& v) override;
};

class ASTOpPreInc : public ASTOpUnary
{
public:
   ASTOpPreInc(Token token, AST* op);

   virtual void accept(ASTVisitor& v) override;
};

class ASTOpPreDec : public ASTOpUnary
{
public:
   ASTOpPreDec(Token token, AST* op);

   virtual void accept(ASTVisitor& v) override;
};

class ASTOpPostInc : public ASTOpUnary
{
public:
   ASTOpPostInc(Token token, AST* op);

   virtual void accept(ASTVisitor& v) override;
};

class ASTOpPostDec : public ASTOpUnary
{
public:
   ASTOpPostDec(Token token, AST* op);

   virtual void accept(ASTVisitor& v) override;
};

class ASTOpCall : public AST
{
public:
   ASTOpCall(Token token, const std::vector<AST*>& children);

   virtual void accept(ASTVisitor& v) override;

   AST* left() const;
   size_t argsSize() const;
   AST* getArg(size_t pos) const;
};

class ASTOpEq : public ASTOpBinary
{
public:
   ASTOpEq(Token token, AST* op1, AST* op2);

   virtual void accept(ASTVisitor& v) override;
};

class ASTOpNeq : public ASTOpBinary
{
public:
   ASTOpNeq(Token token, AST* op1, AST* op2);

   virtual void accept(ASTVisitor& v) override;
};

class ASTOpGt : public ASTOpBinary
{
public:
   ASTOpGt(Token token, AST* op1, AST* op2);

   virtual void accept(ASTVisitor& v) override;
};

class ASTOpLt : public ASTOpBinary
{
public:
   ASTOpLt(Token token, AST* op1, AST* op2);

   virtual void accept(ASTVisitor& v) override;
};

class ASTOpGeq : public ASTOpBinary
{
public:
   ASTOpGeq(Token token, AST* op1, AST* op2);

   virtual void accept(ASTVisitor& v) override;
};

class ASTOpLeq : public ASTOpBinary
{
public:
   ASTOpLeq(Token token, AST* op1, AST* op2);

   virtual void accept(ASTVisitor& v) override;
};

class ASTOpLand : public ASTOpBinary
{
public:
   ASTOpLand(Token token, AST* op1, AST* op2);

   virtual void accept(ASTVisitor& v) override;
};

class ASTOpLor : public ASTOpBinary
{
public:
   ASTOpLor(Token token, AST* op1, AST* op2);

   virtual void accept(ASTVisitor& v) override;
};

class ASTOpLnot : public ASTOpUnary
{
public:
   ASTOpLnot(Token token, AST* op2);

   virtual void accept(ASTVisitor& v) override;
};

class ASTOpAssign : public ASTOpBinary
{
public:
   ASTOpAssign(Token token, AST* op1, AST* op2);

   virtual void accept(ASTVisitor& v) override;
};

class ASTOpPluseq : public ASTOpBinary
{
public:
   ASTOpPluseq(Token token, AST* op1, AST* op2);

   virtual void accept(ASTVisitor& v) override;
};

class ASTOpMinuseq : public ASTOpBinary
{
public:
   ASTOpMinuseq(Token token, AST* op1, AST* op2);

   virtual void accept(ASTVisitor& v) override;
};

class ASTOpMuleq : public ASTOpBinary
{
public:
   ASTOpMuleq(Token token, AST* op1, AST* op2);

   virtual void accept(ASTVisitor& v) override;
};

class ASTOpDiveq : public ASTOpBinary
{
public:
   ASTOpDiveq(Token token, AST* op1, AST* op2);

   virtual void accept(ASTVisitor& v) override;
};

class ASTOpModeq : public ASTOpBinary
{
public:
   ASTOpModeq(Token token, AST* op1, AST* op2);

   virtual void accept(ASTVisitor& v) override;
};

#endif //!AST_OPERATORS_HH_
