#include "token.hh"
#include <stdexcept>
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

long Token::getInt() const
{
   if(_type != TokenType::integer)
      throw std::runtime_error {"Token type is " + getTypeString() +
            ", integer expected"};
   return str::toInt(_representation);
}

double Token::getReal() const
{
   if(_type != TokenType::real)
      throw std::runtime_error {"Token type is " + getTypeString() +
            ", real expected"};
   return str::toFloat(_representation);
}

std::string Token::getComment() const
{
   if(_type == TokenType::inline_comment)
      return _representation.substr(2);
   else if(_type == TokenType::multilines_comment)
      return _representation.substr(2, _representation.size() - 4);
   else
      throw std::runtime_error {"Token type is " + getTypeString() +
            ", comment expected"};
}

std::string Token::getLitteral() const
{
   if(_type == TokenType::litteral1)
      return getLitteral1();
   else if(_type == TokenType::litteral2)
      return getLitteral2();
   else if(_type == TokenType::litteral3)
      return getLitteral3();
   else
      throw std::runtime_error {"Token type is " + getTypeString() +
            ", litteral expected"};
}

std::string Token::getRawLitteral() const
{
   if(_type != TokenType::litteral1 && _type != TokenType::litteral2
      && _type != TokenType::litteral3)
      throw std::runtime_error {"Token type is " + getTypeString() +
            ", litteral expected"};
   else
      return _representation.substr(1, _representation.size() - 2);
}


std::string Token::getLitteral1() const
{
   return getRawLitteral();
}

std::string Token::getLitteral2() const
{
   return getRawLitteral();
}

std::string Token::getLitteral3() const
{
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
   {TokenType::integer, "integer"},
   {TokenType::real, "real"},
   {TokenType::inline_comment, "inline_comment"},
   {TokenType::multilines_comment, "multiline_comment"},
   {TokenType::litteral1, "litteral1"},
   {TokenType::litteral2, "litteral2"},
   {TokenType::litteral3, "litteral3"},
   {TokenType::reserved, "reserved"},
   {TokenType::symbol, "symbol"},
   {TokenType::lparenthesis, "lparenthesis"},
   {TokenType::rparenthesis, "rparenthesis"},
   {TokenType::op_plus, "op_plus"},
   {TokenType::op_minus, "op_minus"},
   {TokenType::op_mul, "op_mul"},
   {TokenType::op_div, "op_div"},
   {TokenType::op_mod, "op_mod"},
   {TokenType::op_assign, "op_assign"},
   {TokenType::kw_let, "kw_let"},
   {TokenType::kw_print, "kw_print"},
   {TokenType::semicollon, "semicollon"},
   {TokenType::lcurlybracket, "lcurlybracket"},
   {TokenType::rcurlybracket, "rcurlybracket"},
   {TokenType::comma, "comma"},
   {TokenType::kw_return, "kw_return"}
};

const std::map<std::string, TokenType> Token::TYPES_LOOKUP = {
   {"+", TokenType::op_plus},
   {"-", TokenType::op_minus},
   {"*", TokenType::op_mul},
   {"/", TokenType::op_div},
   {"%", TokenType::op_mod},
   {"(", TokenType::lparenthesis},
   {")", TokenType::rparenthesis},
   {"=", TokenType::op_assign},
   {"let", TokenType::kw_let},
   {"print", TokenType::kw_print},
   {";", TokenType::semicollon},
   {"{", TokenType::lcurlybracket},
   {"}", TokenType::rcurlybracket},
   {",", TokenType::comma},
   {"return", TokenType::kw_return},


   {"class", TokenType::reserved},
   {"public", TokenType::reserved},
   {"protected", TokenType::reserved},
   {"private", TokenType::reserved}
};
