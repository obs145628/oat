#ifndef TOKEN_HH_
# define TOKEN_HH_

# include <string>
# include <iostream>
# include <vector>
# include <map>

enum class TokenType
{
   eof,
      unknown,
      integer,
      real,
      inline_comment,
      multilines_comment,
      litteral1,
      litteral2,
      litteral3,
      reserved,
      symbol,
      lparenthesis,
      rparenthesis,
      op_plus,
      op_minus,
      op_mul,
      op_div,
      op_mod,
      op_assign,
      kw_let,
      kw_print,
      semicollon,
      lcurlybracket,
      rcurlybracket,
      comma,
      kw_return
};

class Token
{

public:
   Token();
   Token(const std::string& token, TokenType type, std::size_t pos);

   std::string getRepresentation() const;
   std::size_t getPosition() const;
   TokenType getType() const;
   std::string getTypeString() const;
   bool isOfType(TokenType type) const;

   long getInt() const;
   double getReal() const;
   std::string getComment() const;
   std::string getLitteral() const;
   std::string getRawLitteral() const;

   static std::string typeName(TokenType type);
   static TokenType findType(const std::string& token);
   static std::vector<TokenType> lookupTypes(const std::string& token);

private:
   std::string _representation;
   TokenType _type;
   std::size_t _pos;

   std::string getLitteral1() const;
   std::string getLitteral2() const;
   std::string getLitteral3() const;

   static const std::map<TokenType, std::string> TYPES_NAMES;
   static const std::map<std::string, TokenType> TYPES_LOOKUP;



};

std::ostream& operator<<(std::ostream& os, const Token& token);

#endif //!TOKEN_HH_
