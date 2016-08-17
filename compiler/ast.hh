#ifndef AST_HH_
# define AST_HH_

#include <vector>
#include <iostream>
#include <map>
#include <string>
#include "token.hh"

class ASTVisitor;

class AST
{
public:

   AST(Token token, const std::vector<AST*>& children = {});
   virtual ~AST();

   AST(const AST& ast) = delete;

   std::vector<AST*> getChildren() const;

   std::size_t size() const;
   AST* getChild(size_t pos) const;

   Token getToken() const;

   virtual void accept(ASTVisitor&) = 0;

protected:
   Token _token;
   std::vector<AST*> _children;

};

#endif //!AST_HH_
