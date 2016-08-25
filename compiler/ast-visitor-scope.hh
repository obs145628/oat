#ifndef AST_VISTOR_SCOPE_HH_
# define AST_VISTOR_SCOPE_HH_

#include "ast-visitor.hh"

class RuntimeScope;

class ASTVisitorScope : public ASTVisitor
{

private:
   ASTVisitorScope(AST* ast, RuntimeScope* parent);

public:

   static RuntimeScope* getScope(AST* ast, RuntimeScope* parent = nullptr);


   virtual void visit(ASTStatementsBlock* e) override;
   virtual void visit(ASTStatementFor* e) override;
   virtual void visit(ASTModule* e) override;
   virtual void visit(ASTFunctionDef* e) override;
   virtual void visit(ASTGlobalDef* e) override;
   virtual void visit(ASTClass* e) override;
   virtual void visit(ASTClassMethod* e) override;

private:
   RuntimeScope* _parent;
   RuntimeScope* _scope;

   virtual void visitDefault(AST* ast) override;

};

# endif //!AST_VISTOR_SCOPE_HH_
