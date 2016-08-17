#ifndef AST_VISTOR_COMPILE_HH_
# define AST_VISTOR_COMPILE_HH_

# include "ast-visitor.hh"
# include "../vm/const.h"

class ASTState;
class BinBuilder;

class ASTVisitorCompile : public ASTVisitor
{

public:
   ASTVisitorCompile(ASTState* state, BinBuilder* builder);

   virtual void visit(ASTInt* e) override;
   virtual void visit(ASTDouble* e) override;
   virtual void visit(ASTChar* e) override;
   virtual void visit(ASTString* e) override;
   virtual void visit(ASTTrue* e) override;
   virtual void visit(ASTFalse* e) override;
   virtual void visit(ASTNull* e) override;
   virtual void visit(ASTSymbol* e) override;
   virtual void visit(ASTSymbolValue* e) override;

   virtual void visit(ASTOp1Plus* e) override;
   virtual void visit(ASTOp1Minus* e) override;
   virtual void visit(ASTOp2Plus* e) override;
   virtual void visit(ASTOp2Minus* e) override;
   virtual void visit(ASTOp2Mul* e) override;
   virtual void visit(ASTOp2Div* e) override;
   virtual void visit(ASTOp2Mod* e) override;
   virtual void visit(ASTOpPreInc* e) override;
   virtual void visit(ASTOpPreDec* e) override;
   virtual void visit(ASTOpPostInc* e) override;
   virtual void visit(ASTOpPostDec* e) override;
   virtual void visit(ASTOpCall* e) override;
   virtual void visit(ASTOpEq* e) override;
   virtual void visit(ASTOpNeq* e) override;
   virtual void visit(ASTOpGt* e) override;
   virtual void visit(ASTOpLt* e) override;
   virtual void visit(ASTOpGeq* e) override;
   virtual void visit(ASTOpLeq* e) override;
   virtual void visit(ASTOpLand* e) override;
   virtual void visit(ASTOpLor* e) override;
   virtual void visit(ASTOpLnot* e) override;
   virtual void visit(ASTOpAssign* e) override;
   virtual void visit(ASTOpPluseq* e) override;
   virtual void visit(ASTOpMinuseq* e) override;
   virtual void visit(ASTOpMuleq* e) override;
   virtual void visit(ASTOpDiveq* e) override;
   virtual void visit(ASTOpModeq* e) override;

   virtual void visit(ASTStatementsBlock* e) override;
   virtual void visit(ASTStatementEmpty* e) override;
   virtual void visit(ASTStatementDefine* e) override;
   virtual void visit(ASTStatementReturn* e) override;
   virtual void visit(ASTStatementIf* e) override;
   virtual void visit(ASTStatementWhile* e) override;

   virtual void visit(ASTFunctionDef* e) override;

   virtual void visit(ASTModule* e) override;

private:
   ASTState* _state;
   BinBuilder* _builder;

   void visitChildren();

   void compileOpu(t_vm_ins code);
   void compileOpb(t_vm_ins code);
   void compileOpIncDec(t_vm_ins code);
   void compileAssign(t_vm_ins code);

};

# endif //!AST_VISTOR_COMPILE_HH_
