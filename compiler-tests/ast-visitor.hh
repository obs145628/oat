#ifndef AST_VISITOR_HH_
# define AST_VISITOR_HH_

# include "ast-operators.hh"
# include "ast-values.hh"
# include "ast-statements.hh"
# include "ast-functions.hh"
# include "ast-modules.hh"

class ASTVisitor
{

public:
   virtual void visit(ASTOp1Plus* e) = 0;
   virtual void visit(ASTOp1Minus* e) = 0;
   virtual void visit(ASTOp2Plus* e) = 0;
   virtual void visit(ASTOp2Minus* e) = 0;
   virtual void visit(ASTOp2Mul* e) = 0;
   virtual void visit(ASTOp2Div* e) = 0;
   virtual void visit(ASTOp2Mod* e) = 0;
   virtual void visit(ASTIntValue* e) = 0;
   virtual void visit(ASTVarValue* e) = 0;
   virtual void visit(ASTStatementsList* e) = 0;
   virtual void visit(ASTStatementEmpty* e) = 0;
   virtual void visit(ASTStatementPrint* e) = 0;
   virtual void visit(ASTStatementLet* e) = 0;
   virtual void visit(ASTFunctionDef* e) = 0;
   virtual void visit(ASTProgram* e) = 0;
   virtual void visit(ASTStatementReturn* e) = 0;
   virtual void visit(ASTFunctionCall* e) = 0;

};

#endif //!AST_VISITOR_HH_
