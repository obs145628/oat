#ifndef DOT_BUILDER_VISITOR_HH_
# define DOT_BUILDER_VISITOR_HH_

#include "ast-visitor.hh"
#include "dot-tree.hh"

class DotBuilderVisitor : public ASTVisitor
{
public:
   DotBuilderVisitor(AST* ast);

   DotTree* getTree() const;

   virtual void visit(ASTOp1Plus* e) override;
   virtual void visit(ASTOp1Minus* e) override;
   virtual void visit(ASTOp2Plus* e) override;
   virtual void visit(ASTOp2Minus* e) override;
   virtual void visit(ASTOp2Mul* e) override;
   virtual void visit(ASTOp2Div* e) override;
   virtual void visit(ASTOp2Mod* e) override;
   virtual void visit(ASTIntValue* e) override;
   virtual void visit(ASTVarValue* e) override;
   virtual void visit(ASTStatementsList* e) override;
   virtual void visit(ASTStatementEmpty* e) override;
   virtual void visit(ASTStatementLet* e) override;
   virtual void visit(ASTStatementPrint* e) override;
   virtual void visit(ASTFunctionDef* e) override;
   virtual void visit(ASTProgram* e) override;
   virtual void visit(ASTStatementReturn* e) override;
   virtual void visit(ASTFunctionCall* e) override;


private:
   DotTree* _tree;

};



# endif //!DOT_BUILDER_VISITOR_HH_
