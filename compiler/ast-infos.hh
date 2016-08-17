#ifndef AST_INFOS_HH_
# define AST_INFOS_HH_

# include "ast-visitor.hh"
# include "dot-tree.hh"

enum class ASTType
{
   val_int,
      val_double,
      val_char,
      val_string,
      val_true,
      val_false,
      val_null,
      val_symbol,
      val_symbol_val,
      op1_plus,
      op1_minus,
      op2_plus,
      op2_minus,
      op2_mul,
      op2_div,
      op2_mod,
      op_preinc,
      op_predec,
      op_postinc,
      op_postdec,
      op_call,
      op_eq,
      op_neq,
      op_gt,
      op_lt,
      op_geq,
      op_leq,
      op_land,
      op_lor,
      op_lnot,
      op_assign,
      op_pluseq,
      op_minuseq,
      op_muleq,
      op_diveq,
      op_modeq,
      statements_block,
      statement_empty,
      statement_define,
      statement_return,
      statement_if,
      statement_while,
      function_def,
      module
};

struct ASTInfos
{
   ASTType type;
   std::string desc;

   static ASTInfos getInfos(AST* ast);

   static DotTree* buildDotTree(AST* ast);
};


class ASTVisitorInfos : public ASTVisitor
{

public:
   ASTVisitorInfos(AST* ast);

   ASTInfos getInfos() const;

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
   ASTInfos _infos;

};

#endif //!AST_INFOS_HH_
