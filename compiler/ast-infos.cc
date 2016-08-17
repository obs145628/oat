#include "ast-infos.hh"

ASTInfos ASTInfos::getInfos(AST* ast)
{
   return ASTVisitorInfos(ast).getInfos();
}

DotTree* ASTInfos::buildDotTree(AST* ast)
{
   std::vector<DotTree*> children;
   for(AST* child: ast->getChildren())
      children.push_back(buildDotTree(child));
   return new DotTree(getInfos(ast).desc, children);
}







ASTVisitorInfos::ASTVisitorInfos(AST* ast)
{
   ast->accept(*this);
}

ASTInfos ASTVisitorInfos::getInfos() const
{
   return _infos;
}



void ASTVisitorInfos::visit(ASTInt* e)
{
   _infos.type = ASTType::val_int;
   _infos.desc = "val_int: " + std::to_string(e->getValue());
}


void ASTVisitorInfos::visit(ASTDouble* e)
{
   _infos.type = ASTType::val_double;
   _infos.desc = "val_double: " + std::to_string(e->getValue());
}

void ASTVisitorInfos::visit(ASTChar* e)
{
   _infos.type = ASTType::val_char;
   char c = e->getValue();
   _infos.desc = "val_char: " + std::string(&c, 1);
}

void ASTVisitorInfos::visit(ASTString* e)
{
   _infos.type = ASTType::val_int;
   _infos.desc = "val_string: " + e->getValue();
}

void ASTVisitorInfos::visit(ASTTrue*)
{
   _infos.type = ASTType::val_true;
   _infos.desc = "val_bool: true";
}

void ASTVisitorInfos::visit(ASTFalse*)
{
   _infos.type = ASTType::val_false;
   _infos.desc = "val_bool: false";
}

void ASTVisitorInfos::visit(ASTNull*)
{
   _infos.type = ASTType::val_int;
   _infos.desc = "val_null";
}

void ASTVisitorInfos::visit(ASTSymbol* e)
{
   _infos.type = ASTType::val_symbol;
   _infos.desc = "val_symbol: " + e->getName();
}

void ASTVisitorInfos::visit(ASTSymbolValue* e)
{
   _infos.type = ASTType::val_symbol_val;
   _infos.desc = "val_symbol_val: " + e->getName();
}

void ASTVisitorInfos::visit(ASTOp1Plus*)
{
   _infos.type = ASTType::op1_plus;
   _infos.desc = "op1_plus";
}

void ASTVisitorInfos::visit(ASTOp1Minus*)
{
   _infos.type = ASTType::op1_minus;
   _infos.desc = "op1_minus";
}

void ASTVisitorInfos::visit(ASTOp2Plus*)
{
   _infos.type = ASTType::op2_plus;
   _infos.desc = "op2_plus";
}

void ASTVisitorInfos::visit(ASTOp2Minus*)
{
   _infos.type = ASTType::op2_minus;
   _infos.desc = "op2_minus";
}

void ASTVisitorInfos::visit(ASTOp2Mul*)
{
   _infos.type = ASTType::op2_mul;
   _infos.desc = "op2_mul";
}

void ASTVisitorInfos::visit(ASTOp2Div*)
{
   _infos.type = ASTType::op2_div;
   _infos.desc = "op2_div";
}

void ASTVisitorInfos::visit(ASTOp2Mod*)
{
   _infos.type = ASTType::op2_mod;
   _infos.desc = "op2_mod";
}

void ASTVisitorInfos::visit(ASTOpPreInc*)
{
   _infos.type = ASTType::op_preinc;
   _infos.desc = "op_preinc";
}

void ASTVisitorInfos::visit(ASTOpPreDec*)
{
   _infos.type = ASTType::op_predec;
   _infos.desc = "op_predec";
}

void ASTVisitorInfos::visit(ASTOpPostInc*)
{
   _infos.type = ASTType::op_postinc;
   _infos.desc = "op_postinc";
}

void ASTVisitorInfos::visit(ASTOpPostDec*)
{
   _infos.type = ASTType::op_postdec;
   _infos.desc = "op_postdec";
}

void ASTVisitorInfos::visit(ASTOpCall*)
{
   _infos.type = ASTType::op_call;
   _infos.desc = "op_call";
}

void ASTVisitorInfos::visit(ASTOpEq*)
{
   _infos.type = ASTType::op_eq;
   _infos.desc = "op_eq";
}

void ASTVisitorInfos::visit(ASTOpNeq*)
{
   _infos.type = ASTType::op_neq;
   _infos.desc = "op_neq";
}

void ASTVisitorInfos::visit(ASTOpGt*)
{
   _infos.type = ASTType::op_gt;
   _infos.desc = "op_gt";
}

void ASTVisitorInfos::visit(ASTOpLt*)
{
   _infos.type = ASTType::op_lt;
   _infos.desc = "op_lt";
}

void ASTVisitorInfos::visit(ASTOpGeq*)
{
   _infos.type = ASTType::op_geq;
   _infos.desc = "op_geq";
}

void ASTVisitorInfos::visit(ASTOpLeq*)
{
   _infos.type = ASTType::op_leq;
   _infos.desc = "op_leq";
}

void ASTVisitorInfos::visit(ASTOpLand*)
{
   _infos.type = ASTType::op_land;
   _infos.desc = "op_land";
}

void ASTVisitorInfos::visit(ASTOpLor*)
{
   _infos.type = ASTType::op_lor;
   _infos.desc = "op_lor";
}

void ASTVisitorInfos::visit(ASTOpLnot*)
{
   _infos.type = ASTType::op_lnot;
   _infos.desc = "op_lnot";
}

void ASTVisitorInfos::visit(ASTOpAssign*)
{
   _infos.type = ASTType::op_assign;
   _infos.desc = "op_assign";
}

void ASTVisitorInfos::visit(ASTOpPluseq*)
{
   _infos.type = ASTType::op_pluseq;
   _infos.desc = "op_pluseq";
}

void ASTVisitorInfos::visit(ASTOpMinuseq*)
{
   _infos.type = ASTType::op_minuseq;
   _infos.desc = "op_minuseq";
}

void ASTVisitorInfos::visit(ASTOpMuleq*)
{
   _infos.type = ASTType::op_muleq;
   _infos.desc = "op_muleq";
}

void ASTVisitorInfos::visit(ASTOpDiveq*)
{
   _infos.type = ASTType::op_diveq;
   _infos.desc = "op_diveq";
}

void ASTVisitorInfos::visit(ASTOpModeq*)
{
   _infos.type = ASTType::op_modeq;
   _infos.desc = "op_modeq";
}

void ASTVisitorInfos::visit(ASTStatementsBlock*)
{
   _infos.type = ASTType::statements_block;
   _infos.desc = "statements_block";
}

void ASTVisitorInfos::visit(ASTStatementEmpty*)
{
   _infos.type = ASTType::statement_empty;
   _infos.desc = "statement_empty";
}

void ASTVisitorInfos::visit(ASTStatementDefine*)
{
   _infos.type = ASTType::statement_define;
   _infos.desc = "statement_define";
}

void ASTVisitorInfos::visit(ASTStatementReturn*)
{
   _infos.type = ASTType::statement_return;
   _infos.desc = "statement_return";
}

void ASTVisitorInfos::visit(ASTStatementIf* e)
{
   _infos.type = ASTType::statement_empty;
   if(e->hasElseStatement())
      _infos.desc = "statement_if_else";
   else
      _infos.desc = "statement_if";
}

void ASTVisitorInfos::visit(ASTStatementWhile*)
{
   _infos.type = ASTType::statement_while;
   _infos.desc = "statement_while";
}

void ASTVisitorInfos::visit(ASTFunctionDef*)
{
   _infos.type = ASTType::function_def;
   _infos.desc = "function_def";
}

void ASTVisitorInfos::visit(ASTModule*)
{
   _infos.type = ASTType::module;
   _infos.desc = "module";
}
