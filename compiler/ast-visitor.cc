#include "ast-visitor.hh"



void ASTVisitor::visitDefault(AST*)
{

}


void ASTVisitor::visit(ASTInt* e)
{
   visitDefault(e);
}


void ASTVisitor::visit(ASTDouble* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTChar* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTString* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTTrue* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTFalse* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTNull* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTSymbol* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTSymbolValue* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOp1Plus* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOp1Minus* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOp2Plus* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOp2Minus* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOp2Mul* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOp2Div* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOp2Mod* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOpPreInc* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOpPreDec* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOpPostInc* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOpPostDec* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOpCall* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOpEq* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOpNeq* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOpGt* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOpLt* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOpGeq* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOpLeq* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOpLand* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOpLor* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOpLnot* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOpAssign* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOpPluseq* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOpMinuseq* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOpMuleq* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOpDiveq* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOpModeq* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTStatementsBlock* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTStatementEmpty* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTStatementDefine* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTStatementReturn* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTStatementIf* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTStatementWhile* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTFunctionDef* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTModule* e)
{
   visitDefault(e);
}
