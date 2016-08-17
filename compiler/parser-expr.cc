#include "parser.hh"
#include "ast-operators.hh"
#include "ast-values.hh"

AST* Parser::expr()
{
   return expr_p9();
}

AST* Parser::expr_p9() //RTL: =, +=, -=, *=, /=, %=
{
   AST* res = expr_p8();

   if(isTokenOfType({TokenType::op_assign, TokenType::op_pluseq,
               TokenType::op_minuseq, TokenType::op_muleq, TokenType::op_diveq,
               TokenType::op_modeq}))
   {
      Token t = getToken();
      next();

      if(t.isOfType(TokenType::op_assign))
         res = new ASTOpAssign(t, res, expr_p9());
      else if(t.isOfType(TokenType::op_pluseq))
         res = new ASTOpPluseq(t, res, expr_p9());
      else if(t.isOfType(TokenType::op_minuseq))
         res = new ASTOpMinuseq(t, res, expr_p9());
      else if(t.isOfType(TokenType::op_muleq))
         res = new ASTOpMuleq(t, res, expr_p9());
      else if(t.isOfType(TokenType::op_diveq))
         res = new ASTOpDiveq(t, res, expr_p9());
      else
         res = new ASTOpModeq(t, res, expr_p9());
   }

   return res;
}

AST* Parser::expr_p8() //LTR: ||
{
   AST* res = expr_p7();

   while(isTokenOfType(TokenType::op_lor))
   {
      Token t = getToken();
      next();

      res = new ASTOpLor(t, res, expr_p7());
   }

   return res;
}

AST* Parser::expr_p7() //LTR: &&
{
   AST* res = expr_p6();

   while(isTokenOfType(TokenType::op_land))
   {
      Token t = getToken();
      next();

      res = new ASTOpLand(t, res, expr_p6());
   }

   return res;
}

AST* Parser::expr_p6() //LTR: ==, !=
{
   AST* res = expr_p5();

   while(isTokenOfType({TokenType::op_eq, TokenType::op_neq}))
   {
      Token t = getToken();
      next();

      if(t.isOfType(TokenType::op_eq))
         res = new ASTOpEq(t, res, expr_p5());
      else
         res = new ASTOpNeq(t, res, expr_p5());
   }

   return res;
}

AST* Parser::expr_p5() //LTR: >, <, >=, <=
{
   AST* res = expr_p4();

   while(isTokenOfType({TokenType::op_gt, TokenType::op_lt,
               TokenType::op_geq, TokenType::op_leq}))
   {
      Token t = getToken();
      next();

      if(t.isOfType(TokenType::op_gt))
         res = new ASTOpGt(t, res, expr_p4());
      else if(t.isOfType(TokenType::op_lt))
         res = new ASTOpLt(t, res, expr_p4());
      else if(t.isOfType(TokenType::op_geq))
         res = new ASTOpGeq(t, res, expr_p4());
      else
         res = new ASTOpLeq(t, res, expr_p4());
   }

   return res;
}

AST* Parser::expr_p4() //LTR: binary +, binary -
{
   AST* res = expr_p3();

   while(isTokenOfType({TokenType::op_plus, TokenType::op_minus}))
   {
      Token t = getToken();
      next();

      if(t.isOfType(TokenType::op_plus))
         res = new ASTOp2Plus(t, res, expr_p3());
      else
         res = new ASTOp2Minus(t, res, expr_p3());
   }

   return res;
}

AST* Parser::expr_p3() //LTR: *, /, %
{
   AST* res = expr_p2();

   while(isTokenOfType({TokenType::op_mul, TokenType::op_div,
               TokenType::op_mod}))
   {
      Token t = getToken();
      next();

      if(t.isOfType(TokenType::op_mul))
         res = new ASTOp2Mul(t, res, expr_p2());
      else if(t.isOfType(TokenType::op_div))
         res = new ASTOp2Div(t, res, expr_p2());
      else
         res = new ASTOp2Mod(t, res, expr_p2());
   }


   return res;
}

AST* Parser::expr_p2() //RTL: ++ (prefix), -- (prefix), unary +, unary -, !
{
   if(isTokenOfType({TokenType::op_inc, TokenType::op_dec,
               TokenType::op_plus, TokenType::op_minus,
               TokenType::op_lnot}))
   {
      Token t = getToken();
      next();

      if(t.isOfType(TokenType::op_inc))
         return new ASTOpPreInc(t, expr_p2());
      else if(t.isOfType(TokenType::op_dec))
         return new ASTOpPreDec(t, expr_p2());
      else if(t.isOfType(TokenType::op_plus))
         return new ASTOp1Plus(t, expr_p2());
      else if(t.isOfType(TokenType::op_minus))
         return new ASTOp1Minus(t, expr_p2());
      else
         return new ASTOpLnot(t, expr_p2());
   }
   else
      return expr_p1();
}

AST* Parser::expr_p1() //LTR: ++ (suffix), -- (suffix), () (call)
{
   AST* res = expr_value();

   while(isTokenOfType({TokenType::op_inc, TokenType::op_dec,
               TokenType::lparenthesis}))
   {
      Token t = getToken();
      next();

      if(t.isOfType(TokenType::op_inc))
         res = new ASTOpPostInc(t, res);
      else if(t.isOfType(TokenType::op_dec))
         res = new ASTOpPostDec(t, res);
      else
      {
         std::vector<AST*> child = expr_call_list();
         child.insert(child.begin(), res);
         res = new ASTOpCall(t, child);
      }
   }

   return res;
}

AST* Parser::expr_value()
{
   Token t = getToken();
   next();

   if(t.isOfType(TokenType::lparenthesis)) //"(" expr ")"
   {
      AST* res = expr();
      if(!isTokenOfType(TokenType::rparenthesis))
         tokenError(getToken(), "Closing parenthesis expected");
      next();
      return res;
   }

   else if(t.isOfType(TokenType::kw_null)) //null value
   {
      return new ASTNull(t);
   }

   else if(t.isOfType(TokenType::integer)) //integer value
   {
      return new ASTInt(t);
   }

   else if(t.isOfType(TokenType::real)) //double value
   {
      return new ASTDouble(t);
   }

   else if(t.isOfType(TokenType::litteral1)) //char value ('...')
   {
      return new ASTChar(t);
   }

   else if(t.isOfType(TokenType::kw_true)) //bool value (true)
   {
      return new ASTTrue(t);
   }

   else if(t.isOfType(TokenType::kw_false)) //bool value (false)
   {
      return new ASTFalse(t);
   }

   else if(t.isOfType(TokenType::litteral2)) //string value ("...")
   {
      return new ASTString(t);
   }

   else if(t.isOfType(TokenType::symbol)) //variable value
   {
      return new ASTSymbolValue(t);
   }

   else
   {
      tokenError(t, "Unexpected token (invalid value)");
      return nullptr;
   }
}

std::vector<AST*> Parser::expr_call_list()
{
   std::vector<AST*> v;
   bool hasComma = false;

   while(true)
   {
      Token t = getToken();
      if(!hasComma && t.isOfType(TokenType::rparenthesis))
         break;

      v.push_back(expr());

      if(isTokenOfType(TokenType::comma))
      {
         hasComma = true;
         next();
      }
      else
      {
         hasComma = false;
      }
   }

   next();
   return v;
}
