#include "parser.hh"
#include "ast-operators.hh"
#include "ast-values.hh"

AST* Parser::expr()
{
   return expr_p14();
}

AST* Parser::expr_p14() //RTL: =, +=, -=, *=, /=, %=
{
   AST* res = expr_p13();

   if(isTokenOfType({TokenType::op_assign, TokenType::op_pluseq,
               TokenType::op_minuseq, TokenType::op_muleq, TokenType::op_diveq,
               TokenType::op_modeq, TokenType::op_lshifteq,
               TokenType::op_rshifteq, TokenType::op_bandeq,
               TokenType::op_bxoreq, TokenType::op_boreq}))
   {
      Token t = getToken();
      next();

      if(t.isOfType(TokenType::op_assign))
         res = new ASTOpAssign(t, res, expr_p14());
      else if(t.isOfType(TokenType::op_pluseq))
         res = new ASTOpPluseq(t, res, expr_p14());
      else if(t.isOfType(TokenType::op_minuseq))
         res = new ASTOpMinuseq(t, res, expr_p14());
      else if(t.isOfType(TokenType::op_muleq))
         res = new ASTOpMuleq(t, res, expr_p14());
      else if(t.isOfType(TokenType::op_diveq))
         res = new ASTOpDiveq(t, res, expr_p14());
      else if(t.isOfType(TokenType::op_modeq))
         res = new ASTOpModeq(t, res, expr_p14());
      else if(t.isOfType(TokenType::op_lshifteq))
         res = new ASTOpLshifteq(t, res, expr_p14());
      else if(t.isOfType(TokenType::op_rshifteq))
         res = new ASTOpRshifteq(t, res, expr_p14());
      else if(t.isOfType(TokenType::op_bandeq))
         res = new ASTOpBandeq(t, res, expr_p14());
      else if(t.isOfType(TokenType::op_bxoreq))
         res = new ASTOpBxoreq(t, res, expr_p14());
      else
         res = new ASTOpBoreq(t, res, expr_p14());
   }

   return res;
}

AST* Parser::expr_p13() //RTL: ?: (ternary operator)
{
   AST* res = expr_p12();

   if(isTokenOfType(TokenType::qmark))
   {
      Token t = getToken();
      next();
      AST* op2 = expr_p13();

      if(!isTokenOfType(TokenType::collon))
         tokenError(getToken(), "ternary operator: ':' expected");
      next();

      AST* op3 = expr_p13();
      res = new ASTOpTernary(t, res, op2, op3);
   }

   return res;
}


AST* Parser::expr_p12() //LTR: ||
{
   AST* res = expr_p11();

   while(isTokenOfType(TokenType::op_lor))
   {
      Token t = getToken();
      next();

      res = new ASTOpLor(t, res, expr_p11());
   }

   return res;
}

AST* Parser::expr_p11() //LTR: &&
{
   AST* res = expr_p10();

   while(isTokenOfType(TokenType::op_land))
   {
      Token t = getToken();
      next();

      res = new ASTOpLand(t, res, expr_p10());
   }

   return res;
}

AST* Parser::expr_p10() //LTR: | (bitwise or)
{
   AST* res = expr_p9();

   while(isTokenOfType(TokenType::op_bor))
   {
      Token t = getToken();
      next();

      res = new ASTOpBor(t, res, expr_p9());
   }

   return res;
}

AST* Parser::expr_p9() //LTR: ^ (bitwise xor)
{
   AST* res = expr_p8();

   while(isTokenOfType(TokenType::op_bxor))
   {
      Token t = getToken();
      next();

      res = new ASTOpBxor(t, res, expr_p8());
   }

   return res;
}

AST* Parser::expr_p8() //LTR: & (bitwise and)
{
   AST* res = expr_p7();

   while(isTokenOfType(TokenType::op_band))
   {
      Token t = getToken();
      next();

      res = new ASTOpBand(t, res, expr_p7());
   }

   return res;
}

AST* Parser::expr_p7() //LTR: ==, !=
{
   AST* res = expr_p6();

   while(isTokenOfType({TokenType::op_eq, TokenType::op_neq}))
   {
      Token t = getToken();
      next();

      if(t.isOfType(TokenType::op_eq))
         res = new ASTOpEq(t, res, expr_p6());
      else
         res = new ASTOpNeq(t, res, expr_p6());
   }

   return res;
}

AST* Parser::expr_p6() //LTR: >, <, >=, <=
{
   AST* res = expr_p5();

   while(isTokenOfType({TokenType::op_gt, TokenType::op_lt,
               TokenType::op_geq, TokenType::op_leq}))
   {
      Token t = getToken();
      next();

      if(t.isOfType(TokenType::op_gt))
         res = new ASTOpGt(t, res, expr_p5());
      else if(t.isOfType(TokenType::op_lt))
         res = new ASTOpLt(t, res, expr_p5());
      else if(t.isOfType(TokenType::op_geq))
         res = new ASTOpGeq(t, res, expr_p5());
      else
         res = new ASTOpLeq(t, res, expr_p5());
   }

   return res;
}

AST* Parser::expr_p5() //LTR: <<, >> (bitwise shift)
{
   AST* res = expr_p4();

   while(isTokenOfType({TokenType::op_lshift, TokenType::op_rshift}))
   {
      Token t = getToken();
      next();

      if(t.isOfType(TokenType::op_lshift))
         res = new ASTOpLshift(t, res, expr_p4());
      else
         res = new ASTOpRshift(t, res, expr_p4());
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

AST* Parser::expr_p2() //RTL: ++ (prefix), -- (prefix), unary +, unary -, !,
// ~, new
{
   if(isTokenOfType({TokenType::op_inc, TokenType::op_dec,
               TokenType::op_plus, TokenType::op_minus,
               TokenType::op_lnot, TokenType::op_bnot, TokenType::op_new}))
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
      else if(t.isOfType(TokenType::op_lnot))
         return new ASTOpLnot(t, expr_p2());
      else if(t.isOfType(TokenType::op_bnot))
         return new ASTOpBnot(t, expr_p2());

      else
      {
         Token nameToken = getToken();
         if(!nameToken.isOfType(TokenType::symbol))
            tokenError(nameToken, "expected class name");
         next();
         AST* name = new ASTSymbol(nameToken);

         std::vector<AST*> child;

         if(isTokenOfType(TokenType::lparenthesis))
         {
            next();
            child = expr_call_list();
         }

         child.insert(child.begin(), name);
         return new ASTOpNew(t, child);
      }
   }
   else
      return expr_p1();
}

AST* Parser::expr_p1() //LTR: ++ (suffix), -- (suffix), () (call),
// [] (subscript), . (member)
{
   AST* res = expr_value();

   while(isTokenOfType({TokenType::op_inc, TokenType::op_dec,
               TokenType::lparenthesis, TokenType::lbracket,
               TokenType::op_dot}))
   {
      Token t = getToken();
      next();

      if(t.isOfType(TokenType::op_inc))
         res = new ASTOpPostInc(t, res);
      else if(t.isOfType(TokenType::op_dec))
         res = new ASTOpPostDec(t, res);

      else if(t.isOfType(TokenType::lparenthesis))
      {
         std::vector<AST*> child = expr_call_list();
         child.insert(child.begin(), res);
         res = new ASTOpCall(t, child);
      }

      else if(t.isOfType(TokenType::lbracket))
      {
         res = new ASTOpSubscript(t, res, expr());
         if(!isTokenOfType(TokenType::rbracket))
            tokenError(getToken(), "subscript operator error: ']' expected");
         next();
      }

      else
      {
         Token memberToken = getToken();
         if(!memberToken.isOfType(TokenType::symbol))
            tokenError(memberToken, "Operator '.' : member name expected");
         next();
         res = new ASTOpMember(t, res, new ASTSymbol(memberToken));
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
