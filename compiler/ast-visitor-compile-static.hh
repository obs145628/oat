#ifndef AST_VISTOR_COMPILE_STATIC_HH_
# define AST_VISTOR_COMPILE_STATIC_HH_

# include "ast-visitor.hh"
# include "../vm/const.h"

class ASTState;
class BinBuilder;

class ASTVisitorCompileStatic : public ASTVisitor
{

public:
   ASTVisitorCompileStatic(ASTState* state, BinBuilder* builder);

   virtual void visit(ASTModule* e) override;
   virtual void visit(ASTGlobalDef* e) override;
   virtual void visit(ASTClass* e) override;
   virtual void visit(ASTClassMethod* e) override;
   virtual void visit(ASTClassVariable* e) override;

private:
   ASTState* _state;
   BinBuilder* _builder;

   void visitChildren();

   std::string getClassName() const;
   t_vm_int getClassId() const;

};

# endif //!AST_VISTOR_COMPILE_STATIC_HH_
