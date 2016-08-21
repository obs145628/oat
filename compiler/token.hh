#ifndef TOKEN_HH_
# define TOKEN_HH_

# include <string>
# include <iostream>
# include <vector>
# include <map>

class Scanner;

enum class TokenType
{
   eof,
      unknown,
      reserved,
      integer,
      real,
      symbol,
      litteral1,
      litteral2,
      litteral3,
      inline_comment,
      multilines_comment,
      op_plus,
      op_minus,
      op_mul,
      op_div,
      op_mod,
      op_lshift,
      op_rshift,
      op_band,
      op_bxor,
      op_bor,
      op_assign,
      op_pluseq,
      op_minuseq,
      op_muleq,
      op_diveq,
      op_modeq,
      op_lshifteq,
      op_rshifteq,
      op_bandeq,
      op_bxoreq,
      op_boreq,
      op_inc,
      op_dec,
      op_new,
      op_dot,
      op_land,
      op_lor,
      op_lnot,
      op_bnot,
      op_eq,
      op_neq,
      op_gt,
      op_lt,
      op_geq,
      op_leq,
      qmark,
      collon,
      semicollon,
      comma,
      lcurlybracket,
      rcurlybracket,
      lparenthesis,
      rparenthesis,
      lbracket,
      rbracket,
      si_define,
      si_return,
      kw_if,
      kw_else,
      kw_while,
      kw_for,
      kw_do,
      kw_break,
      kw_continue,
      kw_null,
      kw_true,
      kw_false,
      kw_const,
      kw_let,
      kw_export,
      kw_import
};

class Token
{

public:
   Token(Scanner* scanner, const std::string& token,
         TokenType type, std::size_t pos);

   Scanner* getScanner() const;
   std::string getRepresentation() const;
   std::size_t getPosition() const;
   TokenType getType() const;
   std::string getTypeString() const;
   bool isOfType(TokenType type) const;
   bool isOfType(const std::vector<TokenType>& types) const;

   void err(const std::string& message);

   unsigned long getInt() const;
   double getReal() const;
   std::string getComment() const;
   std::string getRawLitteral() const;
   char getLitteral1() const;
   std::string getLitteral2() const;
   std::string getLitteral3() const;

   static std::string typeName(TokenType type);
   static TokenType findType(const std::string& token);
   static std::vector<TokenType> lookupTypes(const std::string& token);

private:
   Scanner* _scanner;
   std::string _representation;
   TokenType _type;
   std::size_t _pos;



   static const std::map<TokenType, std::string> TYPES_NAMES;
   static const std::map<std::string, TokenType> TYPES_LOOKUP;



};

std::ostream& operator<<(std::ostream& os, const Token& token);

#endif //!TOKEN_HH_
