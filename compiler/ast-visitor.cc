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

void ASTVisitor::visit(ASTArray* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTSet* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTMap* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTThis* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTSuper* e)
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

void ASTVisitor::visit(ASTOpBnot* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOpLshift* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOpRshift* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOpBand* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOpBxor* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOpBor* e)
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

void ASTVisitor::visit(ASTOpLshifteq* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOpRshifteq* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOpBandeq* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOpBxoreq* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOpBoreq* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOpTernary* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOpSubscript* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOpMember* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTOpNew* e)
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

void ASTVisitor::visit(ASTStatementDo* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTStatementFor* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTStatementBreak* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTStatementContinue* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTModule* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTFunctionDef* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTGlobalDef* e)
{
   visitDefault(e);
}


void ASTVisitor::visit(ASTClass* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTClassMethod* e)
{
   visitDefault(e);
}

void ASTVisitor::visit(ASTClassVariable* e)
{
   visitDefault(e);
}
