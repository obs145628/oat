#ifndef AST_VISITOR_HH_
# define AST_VISITOR_HH_

# include "ast-values.hh"
# include "ast-operators.hh"
# include "ast-statements.hh"
# include "ast-functions.hh"
# include "ast-modules.hh"

class ASTVisitor
{

public:

   virtual void visit(ASTInt* e);
   virtual void visit(ASTDouble* e);
   virtual void visit(ASTChar* e);
   virtual void visit(ASTString* e);
   virtual void visit(ASTTrue* e);
   virtual void visit(ASTFalse* e);
   virtual void visit(ASTNull* e);
   virtual void visit(ASTSymbol* e);
   virtual void visit(ASTSymbolValue* e);

   virtual void visit(ASTOp1Plus* e);
   virtual void visit(ASTOp1Minus* e);
   virtual void visit(ASTOp2Plus* e);
   virtual void visit(ASTOp2Minus* e);
   virtual void visit(ASTOp2Mul* e);
   virtual void visit(ASTOp2Div* e);
   virtual void visit(ASTOp2Mod* e);
   virtual void visit(ASTOpPreInc* e);
   virtual void visit(ASTOpPreDec* e);
   virtual void visit(ASTOpPostInc* e);
   virtual void visit(ASTOpPostDec* e);
   virtual void visit(ASTOpCall* e);
   virtual void visit(ASTOpEq* e);
   virtual void visit(ASTOpNeq* e);
   virtual void visit(ASTOpGt* e);
   virtual void visit(ASTOpLt* e);
   virtual void visit(ASTOpGeq* e);
   virtual void visit(ASTOpLeq* e);
   virtual void visit(ASTOpLand* e);
   virtual void visit(ASTOpLor* e);
   virtual void visit(ASTOpLnot* e);
   virtual void visit(ASTOpBnot* e);
   virtual void visit(ASTOpLshift* e);
   virtual void visit(ASTOpRshift* e);
   virtual void visit(ASTOpBand* e);
   virtual void visit(ASTOpBxor* e);
   virtual void visit(ASTOpBor* e);
   virtual void visit(ASTOpAssign* e);
   virtual void visit(ASTOpPluseq* e);
   virtual void visit(ASTOpMinuseq* e);
   virtual void visit(ASTOpMuleq* e);
   virtual void visit(ASTOpDiveq* e);
   virtual void visit(ASTOpModeq* e);
   virtual void visit(ASTOpLshifteq* e);
   virtual void visit(ASTOpRshifteq* e);
   virtual void visit(ASTOpBandeq* e);
   virtual void visit(ASTOpBxoreq* e);
   virtual void visit(ASTOpBoreq* e);
   virtual void visit(ASTOpTernary* e);
   virtual void visit(ASTOpSubscript* e);
   virtual void visit(ASTOpMember* e);
   virtual void visit(ASTOpNew* e);

   virtual void visit(ASTStatementsBlock* e);
   virtual void visit(ASTStatementEmpty* e);
   virtual void visit(ASTStatementDefine* e);
   virtual void visit(ASTStatementReturn* e);
   virtual void visit(ASTStatementIf* e);
   virtual void visit(ASTStatementWhile* e);
   virtual void visit(ASTStatementDo* e);
   virtual void visit(ASTStatementFor* e);
   virtual void visit(ASTStatementBreak* e);
   virtual void visit(ASTStatementContinue* e);

   virtual void visit(ASTModule* e);
   virtual void visit(ASTFunctionDef* e);
   virtual void visit(ASTGlobalDef* e);

private:

   virtual void visitDefault(AST* ast);
};

#endif //!AST_VISITOR_HH_
