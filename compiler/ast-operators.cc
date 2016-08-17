#include "ast-operators.hh"
#include "ast-visitor.hh"
#include <cassert>


ASTOpUnary::ASTOpUnary(Token token, AST* op)
   : AST(token, {op})
{

}

AST* ASTOpUnary::op() const
{
   return _children[0];
}

ASTOpBinary::ASTOpBinary(Token token, AST* op1, AST* op2)
   : AST(token, {op1, op2})
{

}

AST* ASTOpBinary::op1() const
{
   return _children[0];
}

AST* ASTOpBinary::op2() const
{
   return _children[1];
}


ASTOp1Plus::ASTOp1Plus(Token token, AST* op)
   : ASTOpUnary(token, op)
{
   assert(token.getType() == TokenType::op_plus);
}

void ASTOp1Plus::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTOp1Minus::ASTOp1Minus(Token token, AST* op)
   : ASTOpUnary(token, op)
{
   assert(token.getType() == TokenType::op_minus);
}

void ASTOp1Minus::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTOp2Plus::ASTOp2Plus(Token token, AST* op1, AST* op2)
   : ASTOpBinary(token, op1, op2)
{
   assert(token.getType() == TokenType::op_plus);
}

void ASTOp2Plus::accept(ASTVisitor& v)
{
   v.visit(this);
}



ASTOp2Minus::ASTOp2Minus(Token token, AST* op1, AST* op2)
   : ASTOpBinary(token, op1, op2)
{
   assert(token.getType() == TokenType::op_minus);
}

void ASTOp2Minus::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTOp2Mul::ASTOp2Mul(Token token, AST* op1, AST* op2)
   : ASTOpBinary(token, op1, op2)
{
   assert(token.getType() == TokenType::op_mul);
}

void ASTOp2Mul::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTOp2Div::ASTOp2Div(Token token, AST* op1, AST* op2)
   : ASTOpBinary(token, op1, op2)
{
   assert(token.getType() == TokenType::op_div);
}

void ASTOp2Div::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTOp2Mod::ASTOp2Mod(Token token, AST* op1, AST* op2)
   : ASTOpBinary(token, op1, op2)
{
   assert(token.getType() == TokenType::op_mod);
}

void ASTOp2Mod::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTOpPreInc::ASTOpPreInc(Token token, AST* op)
   : ASTOpUnary(token, op)
{
   assert(token.getType() == TokenType::op_inc);
}

void ASTOpPreInc::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTOpPreDec::ASTOpPreDec(Token token, AST* op)
   : ASTOpUnary(token, op)
{
   assert(token.getType() == TokenType::op_dec);
}

void ASTOpPreDec::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTOpPostInc::ASTOpPostInc(Token token, AST* op)
   : ASTOpUnary(token, op)
{
   assert(token.getType() == TokenType::op_inc);
}

void ASTOpPostInc::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTOpPostDec::ASTOpPostDec(Token token, AST* op)
   : ASTOpUnary(token, op)
{
   assert(token.getType() == TokenType::op_dec);
}

void ASTOpPostDec::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTOpCall::ASTOpCall(Token token, const std::vector<AST*>& children)
   : AST(token, children)
{
   assert(token.getType() == TokenType::lparenthesis);
   assert(children.size() > 0);
}

void ASTOpCall::accept(ASTVisitor& v)
{
   v.visit(this);
}

AST* ASTOpCall::left() const
{
   return _children[0];
}

size_t ASTOpCall::argsSize() const
{
   return _children.size() - 1;
}

AST* ASTOpCall::getArg(size_t pos) const
{
   assert(pos + 1 < _children.size());
   return _children[pos + 1];
}



ASTOpEq::ASTOpEq(Token token, AST* op1, AST* op2)
   : ASTOpBinary(token, op1, op2)
{
   assert(token.getType() == TokenType::op_eq);
}

void ASTOpEq::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTOpNeq::ASTOpNeq(Token token, AST* op1, AST* op2)
   : ASTOpBinary(token, op1, op2)
{
   assert(token.getType() == TokenType::op_neq);
}

void ASTOpNeq::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTOpGt::ASTOpGt(Token token, AST* op1, AST* op2)
   : ASTOpBinary(token, op1, op2)
{
   assert(token.getType() == TokenType::op_gt);
}

void ASTOpGt::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTOpLt::ASTOpLt(Token token, AST* op1, AST* op2)
   : ASTOpBinary(token, op1, op2)
{
   assert(token.getType() == TokenType::op_lt);
}

void ASTOpLt::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTOpGeq::ASTOpGeq(Token token, AST* op1, AST* op2)
   : ASTOpBinary(token, op1, op2)
{
   assert(token.getType() == TokenType::op_geq);
}

void ASTOpGeq::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTOpLeq::ASTOpLeq(Token token, AST* op1, AST* op2)
   : ASTOpBinary(token, op1, op2)
{
   assert(token.getType() == TokenType::op_leq);
}

void ASTOpLeq::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTOpLand::ASTOpLand(Token token, AST* op1, AST* op2)
   : ASTOpBinary(token, op1, op2)
{
   assert(token.getType() == TokenType::op_land);
}

void ASTOpLand::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTOpLor::ASTOpLor(Token token, AST* op1, AST* op2)
   : ASTOpBinary(token, op1, op2)
{
   assert(token.getType() == TokenType::op_lor);
}

void ASTOpLor::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTOpLnot::ASTOpLnot(Token token, AST* op)
   : ASTOpUnary(token, op)
{
   assert(token.getType() == TokenType::op_lnot);
}

void ASTOpLnot::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTOpAssign::ASTOpAssign(Token token, AST* op1, AST* op2)
   : ASTOpBinary(token, op1, op2)
{
   assert(token.getType() == TokenType::op_assign);
}

void ASTOpAssign::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTOpPluseq::ASTOpPluseq(Token token, AST* op1, AST* op2)
   : ASTOpBinary(token, op1, op2)
{
   assert(token.getType() == TokenType::op_pluseq);
}

void ASTOpPluseq::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTOpMinuseq::ASTOpMinuseq(Token token, AST* op1, AST* op2)
   : ASTOpBinary(token, op1, op2)
{
   assert(token.getType() == TokenType::op_minuseq);
}

void ASTOpMinuseq::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTOpMuleq::ASTOpMuleq(Token token, AST* op1, AST* op2)
   : ASTOpBinary(token, op1, op2)
{
   assert(token.getType() == TokenType::op_muleq);
}

void ASTOpMuleq::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTOpDiveq::ASTOpDiveq(Token token, AST* op1, AST* op2)
   : ASTOpBinary(token, op1, op2)
{
   assert(token.getType() == TokenType::op_diveq);
}

void ASTOpDiveq::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTOpModeq::ASTOpModeq(Token token, AST* op1, AST* op2)
   : ASTOpBinary(token, op1, op2)
{
   assert(token.getType() == TokenType::op_modeq);
}

void ASTOpModeq::accept(ASTVisitor& v)
{
   v.visit(this);
}
