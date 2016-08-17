#include "token.hh"
#include <cassert>
#include <sstream>
#include "str.hh"


Token::Token()
   : Token("", TokenType::eof, 0)
{

}

Token::Token(const std::string& token, TokenType type, std::size_t pos)
   : _representation(token), _type(type), _pos(pos)
{

}

std::string Token::getRepresentation() const
{
   return _representation;
}

std::size_t Token::getPosition() const
{
   return _pos;
}

TokenType Token::getType() const
{
   return _type;
}

std::string Token::getTypeString() const
{
   return TYPES_NAMES.find(_type)->second;
}

bool Token::isOfType(TokenType type) const
{
   return _type == type;
}

bool Token::isOfType(const std::vector<TokenType>& types) const
{
   for(const TokenType& t: types)
      if(_type == t)
         return true;
   return false;
}

unsigned long Token::getInt() const
{
   assert(_type == TokenType::integer);
   return str::toInt(_representation);
}

double Token::getReal() const
{
   assert(_type == TokenType::real);
   return str::toFloat(_representation);
}

std::string Token::getComment() const
{
   assert(_type == TokenType::inline_comment
          || _type == TokenType::multilines_comment);

   if(_type == TokenType::inline_comment)
      return _representation.substr(2);
   else
      return _representation.substr(2, _representation.size() - 4);
}

std::string Token::getRawLitteral() const
{
   assert(_type == TokenType::litteral1
          || _type == TokenType::litteral2
          || _type == TokenType::litteral3);
   return _representation.substr(1, _representation.size() - 2);
}


char Token::getLitteral1() const
{
   assert(_type == TokenType::litteral1);
   return str::litteralToChar(getRawLitteral());
}

std::string Token::getLitteral2() const
{
   assert(_type == TokenType::litteral2);
   return str::litteralToStr(getRawLitteral());
}

std::string Token::getLitteral3() const
{
   assert(_type == TokenType::litteral3);
   return getRawLitteral();
}

std::string Token::typeName(TokenType type)
{
   return TYPES_NAMES.find(type)->second;
}

TokenType Token::findType(const std::string& token)
{
   auto it = TYPES_LOOKUP.find(token);
   if(it == TYPES_LOOKUP.end())
      return TokenType::unknown;
   else
      return it->second;
}

std::vector<TokenType> Token::lookupTypes(const std::string& token)
{
   std::vector<TokenType> types;

   for(const auto& it: TYPES_LOOKUP)
   {
      if(str::startsWith(it.first, token))
         types.push_back(it.second);
   }

   return types;
}

std::ostream& operator<<(std::ostream& os, const Token& token)
{
   os << "<" << token.getRepresentation() << "> ("
      << token.getTypeString() << ")";
   return os;
}

const std::map<TokenType, std::string> Token::TYPES_NAMES = {
   {TokenType::eof, "eof"},
   {TokenType::unknown, "unknown"},
   {TokenType::reserved, "reserved"},
   {TokenType::integer, "integer"},
   {TokenType::real, "real"},
   {TokenType::symbol, "symbol"},
   {TokenType::litteral1, "litteral1"},
   {TokenType::litteral2, "litteral2"},
   {TokenType::litteral3, "litteral3"},
   {TokenType::inline_comment, "inline_comment"},
   {TokenType::multilines_comment, "multiline_comment"},
   {TokenType::op_plus, "op_plus"},
   {TokenType::op_minus, "op_minus"},
   {TokenType::op_mul, "op_mul"},
   {TokenType::op_div, "op_div"},
   {TokenType::op_mod, "op_mod"},
   {TokenType::op_assign, "op_assign"},
   {TokenType::op_pluseq, "op_pluseq"},
   {TokenType::op_minuseq, "op_minuseq"},
   {TokenType::op_muleq, "op_muleq"},
   {TokenType::op_diveq, "op_diveq"},
   {TokenType::op_modeq, "op_modeq"},
   {TokenType::op_inc, "op_inc"},
   {TokenType::op_dec, "op_dec"},
   {TokenType::op_land, "op_land"},
   {TokenType::op_lor, "op_lor"},
   {TokenType::op_lnot, "op_lnot"},
   {TokenType::op_eq, "op_eq"},
   {TokenType::op_neq, "op_neq"},
   {TokenType::op_gt, "op_gt"},
   {TokenType::op_lt, "op_lt"},
   {TokenType::op_geq, "op_geq"},
   {TokenType::op_leq, "op_leq"},
   {TokenType::semicollon, "semicollon"},
   {TokenType::comma, "comma"},
   {TokenType::lcurlybracket, "lcurlybracket"},
   {TokenType::rcurlybracket, "rcurlybracket"},
   {TokenType::lparenthesis, "lparenthesis"},
   {TokenType::rparenthesis, "rparenthesis"},
   {TokenType::si_define, "si_define"},
   {TokenType::si_return, "si_return"},
   {TokenType::kw_if, "kw_if"},
   {TokenType::kw_else, "kw_else"},
   {TokenType::kw_while, "kw_while"},
   {TokenType::kw_null, "kw_null"},
   {TokenType::kw_true, "kw_true"},
   {TokenType::kw_false, "kw_false"}
};

const std::map<std::string, TokenType> Token::TYPES_LOOKUP = {
   {"+", TokenType::op_plus},
   {"-", TokenType::op_minus},
   {"*", TokenType::op_mul},
   {"/", TokenType::op_div},
   {"%", TokenType::op_mod},
   {"=", TokenType::op_assign},
   {"+=", TokenType::op_pluseq},
   {"-=", TokenType::op_minuseq},
   {"*=", TokenType::op_muleq},
   {"/=", TokenType::op_diveq},
   {"%=", TokenType::op_modeq},
   {"++", TokenType::op_inc},
   {"--", TokenType::op_dec},
   {"&&", TokenType::op_land},
   {"||", TokenType::op_lor},
   {"!", TokenType::op_lnot},
   {"==", TokenType::op_eq},
   {"!=", TokenType::op_neq},
   {">", TokenType::op_gt},
   {"<", TokenType::op_lt},
   {">=", TokenType::op_geq},
   {"<=", TokenType::op_leq},
   {";", TokenType::semicollon},
   {",", TokenType::comma},
   {"{", TokenType::lcurlybracket},
   {"}", TokenType::rcurlybracket},
   {"(", TokenType::lparenthesis},
   {")", TokenType::rparenthesis},
   {":=", TokenType::si_define},
   {"=>", TokenType::si_return},
   {"if", TokenType::kw_if},
   {"else", TokenType::kw_else},
   {"while", TokenType::kw_while},
   {"null", TokenType::kw_null},
   {"true", TokenType::kw_true},
   {"false", TokenType::kw_false},


   {"int", TokenType::reserved},
   {"double", TokenType::reserved},
   {"char", TokenType::reserved},
   {"bool", TokenType::reserved},
   {"string", TokenType::reserved}
};
