#include "ast-visitor-precheck.hh"
#include "ast-state.hh"
#include "runtime-scope.hh"
#include "logs.hh"
#include "../vm/dvar.h"
#include "bin-builder.hh"

ASTVisitorPrecheck::ASTVisitorPrecheck(ASTState* state, BinBuilder* builder)
   : _state(state), _builder(builder)
{
   _state->ast()->accept(*this);
}

void ASTVisitorPrecheck::visitChildren()
{
   for(ASTState* child: _state->children())
      ASTVisitorPrecheck{child, _builder};
}

void ASTVisitorPrecheck::visit(ASTModule*)
{
   visitChildren();

   if(LOG_PRECHECK)
      std::cout << "precheck: verify if main function is defined" << std::endl;

   if(!_state->scope()->hasFunction("main"))
      _state->tokenError("main function missing");

   if(LOG_PRECHECK)
      std::cout << "precheck: done !" << std::endl;
}

void ASTVisitorPrecheck::visit(ASTFunctionDef* e)
{
   std::string name = e->getName()->getName();
   RuntimeScope* scope = _state->scope();

   if(LOG_PRECHECK)
      std::cout << "precheck: function " << name << std::endl;

   if(scope->hasGlobalSymbol(name))
      _state->tokenError("Global symbol already defined");
   scope->defineFunction(name, e);
}

void ASTVisitorPrecheck::visit(ASTGlobalDef* e)
{
   std::string name = e->getSymbol()->getName();
   RuntimeScope* scope = _state->scope();
   t_vm_mode mode = e->isConst() ? DVAR_MCONST : DVAR_MVAR;
   std::string label = _builder->getUniqueLabel(name + "_global_");

   if(LOG_PRECHECK)
      std::cout << "precheck: global variable " << name << std::endl;

   if(scope->hasGlobalSymbol(name))
      _state->tokenError("Global symbol already defined");
   scope->defineGlobal(name, label, DVAR_TNOT, mode);
}

void ASTVisitorPrecheck::visitDefault(AST*)
{
   visitChildren();
}
