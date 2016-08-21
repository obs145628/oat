#ifndef AST_VISTOR_PRECHECK_HH_
# define AST_VISTOR_PRECHECK_HH_

#include "ast-visitor.hh"

class BinBuilder;

class ASTState;

class ASTVisitorPrecheck : public ASTVisitor
{

public:
   ASTVisitorPrecheck(ASTState* state, BinBuilder* builder);

   virtual void visit(ASTModule* e) override;
   virtual void visit(ASTFunctionDef* e) override;
   virtual void visit(ASTGlobalDef* e) override;

private:
   ASTState* _state;
   BinBuilder* _builder;

   void visitChildren();

   virtual void visitDefault(AST* e) override;


};

# endif //!AST_VISTOR_PRECHECK_HH_
