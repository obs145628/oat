#include "ast.hh"
#include <cassert>

AST::AST(Token token, const std::vector<AST*>& children)
   : _token(token), _children(children)
{

}

AST::~AST()
{
   for(AST* child : _children)
      delete child;
}

std::vector<AST*> AST::getChildren() const
{
   return _children;
}

std::size_t AST::size() const
{
   return _children.size();
}

AST* AST::getChild(size_t pos) const
{
   assert(pos < _children.size());
   return _children[pos];
}

Token AST::getToken() const
{
   return _token;
}
