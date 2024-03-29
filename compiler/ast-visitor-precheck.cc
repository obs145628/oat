#include "ast-visitor-precheck.hh"
#include "ast-state.hh"
#include <set>
#include "runtime-scope.hh"
#include "logs.hh"
#include "../vm/dvar.h"
#include "bin-builder.hh"
#include "slib.hh"

#include <iostream>

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
   std::string label = _builder->getUniqueLabel(name + "_global_");

   if(LOG_PRECHECK)
      std::cout << "precheck: global variable " << name << std::endl;

   if(scope->hasGlobalSymbol(name))
      _state->tokenError("Global symbol already defined");
   scope->defineGlobal(e, label, DVAR_TNOT);
}

void ASTVisitorPrecheck::visit(ASTClass* e)
{
   std::string name = e->getName()->getName();
   RuntimeScope* scope = _state->scope();

   if(LOG_PRECHECK)
      std::cout << "precheck: class " << name << std::endl;

   if(e->hasParent())
   {
      std::string parentName = e->getParent()->getName();
      if(SLib::hasClass(parentName))
         _state->tokenError("Parent class can't be a native class");
      if(!_state->scope()->hasClass(parentName))
         _state->tokenError("Undefined parent class");
   }

   if(scope->hasGlobalSymbol(name))
      _state->tokenError("Global symbol already defined");
   scope->defineClass(name, e);

   std::set<std::string> fields;
   for(size_t i = 0; i < e->fieldsSize(); ++i)
   {
      ASTClassField* child = e->getField(i);
      std::string fieldName = child->getChild(0)
         ->getToken().getRepresentation();
      if(fields.find(fieldName) != fields.end())
         _state->getChild(child)->tokenError("Class "
                                             + name + " already has a field "
                                             + fieldName);
      fields.insert(fieldName);
   }
}

void ASTVisitorPrecheck::visitDefault(AST*)
{
   visitChildren();
}
