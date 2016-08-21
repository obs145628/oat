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

void ASTVisitorInfos::visit(ASTOpBnot*)
{
   _infos.type = ASTType::op_bnot;
   _infos.desc = "op_bnot";
}

void ASTVisitorInfos::visit(ASTOpLshift*)
{
   _infos.type = ASTType::op_lshift;
   _infos.desc = "op_lshift";
}

void ASTVisitorInfos::visit(ASTOpRshift*)
{
   _infos.type = ASTType::op_rshift;
   _infos.desc = "op_rshift";
}

void ASTVisitorInfos::visit(ASTOpBand*)
{
   _infos.type = ASTType::op_band;
   _infos.desc = "op_band";
}

void ASTVisitorInfos::visit(ASTOpBxor*)
{
   _infos.type = ASTType::op_bxor;
   _infos.desc = "op_bxor";
}

void ASTVisitorInfos::visit(ASTOpBor*)
{
   _infos.type = ASTType::op_bor;
   _infos.desc = "op_bor";
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

void ASTVisitorInfos::visit(ASTOpLshifteq*)
{
   _infos.type = ASTType::op_lshifteq;
   _infos.desc = "op_lshifteq";
}

void ASTVisitorInfos::visit(ASTOpRshifteq*)
{
   _infos.type = ASTType::op_rshifteq;
   _infos.desc = "op_rshifteq";
}

void ASTVisitorInfos::visit(ASTOpBandeq*)
{
   _infos.type = ASTType::op_bandeq;
   _infos.desc = "op_bandeq";
}

void ASTVisitorInfos::visit(ASTOpBxoreq*)
{
   _infos.type = ASTType::op_bxoreq;
   _infos.desc = "op_bxoreq";
}

void ASTVisitorInfos::visit(ASTOpBoreq*)
{
   _infos.type = ASTType::op_boreq;
   _infos.desc = "op_boreq";
}

void ASTVisitorInfos::visit(ASTOpTernary*)
{
   _infos.type = ASTType::op_ternary;
   _infos.desc = "op_ternary";
}

void ASTVisitorInfos::visit(ASTOpSubscript*)
{
   _infos.type = ASTType::op_subscript;
   _infos.desc = "op_subscript";
}

void ASTVisitorInfos::visit(ASTOpMember*)
{
   _infos.type = ASTType::op_member;
   _infos.desc = "op_member";
}

void ASTVisitorInfos::visit(ASTOpNew*)
{
   _infos.type = ASTType::op_new;
   _infos.desc = "op_new";
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

void ASTVisitorInfos::visit(ASTStatementDo*)
{
   _infos.type = ASTType::statement_do;
   _infos.desc = "statement_do";
}

void ASTVisitorInfos::visit(ASTStatementFor*)
{
   _infos.type = ASTType::statement_for;
   _infos.desc = "statement_for";
}

void ASTVisitorInfos::visit(ASTStatementBreak*)
{
   _infos.type = ASTType::statement_break;
   _infos.desc = "statement_break";
}

void ASTVisitorInfos::visit(ASTStatementContinue*)
{
   _infos.type = ASTType::statement_continue;
   _infos.desc = "statement_continue";
}

void ASTVisitorInfos::visit(ASTModule*)
{
   _infos.type = ASTType::module;
   _infos.desc = "module";
}

void ASTVisitorInfos::visit(ASTFunctionDef*)
{
   _infos.type = ASTType::function_def;
   _infos.desc = "function_def";
}

void ASTVisitorInfos::visit(ASTGlobalDef*)
{
   _infos.type = ASTType::global_def;
   _infos.desc = "global_def";
}
