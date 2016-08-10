#include "dot-builder-visitor.hh"
#include <string>


DotBuilderVisitor:: DotBuilderVisitor(AST* ast)
{
   ast->accept(*this);
}

DotTree* DotBuilderVisitor::getTree() const
{
   return _tree;
}

void DotBuilderVisitor::visit(ASTOp1Plus* e)
{
   DotBuilderVisitor v1(e->getChild(0));
   _tree = new DotTree("op1_plus", {v1._tree});
}

void DotBuilderVisitor::visit(ASTOp1Minus* e)
{
   DotBuilderVisitor v1(e->getChild(0));
   _tree = new DotTree("op1_minus", {v1._tree});
}

void DotBuilderVisitor::visit(ASTOp2Plus* e)
{
   DotBuilderVisitor v1(e->getChild(0));
   DotBuilderVisitor v2(e->getChild(1));
   _tree = new DotTree("op2_plus", {v1._tree, v2._tree});
}

void DotBuilderVisitor::visit(ASTOp2Minus* e)
{
   DotBuilderVisitor v1(e->getChild(0));
   DotBuilderVisitor v2(e->getChild(1));
   _tree = new DotTree("op2_minus", {v1._tree, v2._tree});
}

void DotBuilderVisitor::visit(ASTOp2Mul* e)
{
   DotBuilderVisitor v1(e->getChild(0));
   DotBuilderVisitor v2(e->getChild(1));
   _tree = new DotTree("op2_mul", {v1._tree, v2._tree});
}

void DotBuilderVisitor::visit(ASTOp2Div* e)
{
   DotBuilderVisitor v1(e->getChild(0));
   DotBuilderVisitor v2(e->getChild(1));
   _tree = new DotTree("op2_div", {v1._tree, v2._tree});
}

void DotBuilderVisitor::visit(ASTOp2Mod* e)
{
   DotBuilderVisitor v1(e->getChild(0));
   DotBuilderVisitor v2(e->getChild(1));
   _tree = new DotTree("op2_mod", {v1._tree, v2._tree});
}

void DotBuilderVisitor::visit(ASTIntValue* e)
{
   _tree = new DotTree("int_value: " + std::to_string(e->getValue()));
}

void DotBuilderVisitor::visit(ASTVarValue* e)
{
   _tree = new DotTree("var_value: " + e->getVar());
}

void DotBuilderVisitor::visit(ASTStatementsList* e)
{
   std::vector<DotTree*> children;
   for(AST* child: e->getChildren())
      children.push_back(DotBuilderVisitor(child)._tree);

   _tree = new DotTree("statements_list", children);
}

void DotBuilderVisitor::visit(ASTStatementEmpty*)
{
   _tree = new DotTree("statement_empty");
}

void DotBuilderVisitor::visit(ASTStatementLet* e)
{
   DotBuilderVisitor v1(e->getChild(0));
   DotBuilderVisitor v2(e->getChild(1));
   _tree = new DotTree("statement_let", {v1._tree, v2._tree});
}

void DotBuilderVisitor::visit(ASTStatementPrint* e)
{
   DotBuilderVisitor v1(e->getChild(0));
   _tree = new DotTree("statement_print", {v1._tree});
}

void DotBuilderVisitor::visit(ASTFunctionDef* e)
{
   std::vector<DotTree*> children;
   for(AST* child: e->getChildren())
      children.push_back(DotBuilderVisitor(child)._tree);

   _tree = new DotTree("function_def", children);
}

void DotBuilderVisitor::visit(ASTProgram* e)
{
   std::vector<DotTree*> children;
   for(AST* child: e->getChildren())
      children.push_back(DotBuilderVisitor(child)._tree);

   _tree = new DotTree("program", children);
}

void DotBuilderVisitor::visit(ASTStatementReturn* e)
{
   DotBuilderVisitor v1(e->getChild(0));
   _tree = new DotTree("statement_return", {v1._tree});
}

void DotBuilderVisitor::visit(ASTFunctionCall* e)
{
   std::vector<DotTree*> children;
   for(AST* child: e->getChildren())
      children.push_back(DotBuilderVisitor(child)._tree);

   _tree = new DotTree("function_call", children);
}
