#include "ast-visitor-precheck.hh"
#include "ast-state.hh"
#include "runtime-scope.hh"

ASTVisitorPrecheck::ASTVisitorPrecheck(ASTState* state)
   : _state(state)
{
   _state->ast()->accept(*this);
}

void ASTVisitorPrecheck::visitChildren()
{
   for(ASTState* child: _state->children())
      ASTVisitorPrecheck{child};
}

void ASTVisitorPrecheck::visit(ASTFunctionDef* e)
{
   std::string name = e->getName()->getName();
   RuntimeScope* scope = _state->scope();
   if(scope->hasFunction(name))
      _state->tokenError("Function already defined");
   scope->defineFunction(name, e);
   visitChildren();
}

void ASTVisitorPrecheck::visitDefault(AST*)
{
   visitChildren();
}
