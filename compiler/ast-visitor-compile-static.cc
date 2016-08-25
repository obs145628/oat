#include "ast-visitor-compile-static.hh"
#include "ast-visitor-compile.hh"
#include "ast-state.hh"
#include "bin-builder.hh"
#include "runtime-scope.hh"
#include "slib.hh"
#include "logs.hh"
#include "../vm/dvar.h"
#include <cassert>
#include <iostream>

ASTVisitorCompileStatic::ASTVisitorCompileStatic(ASTState* state, BinBuilder* builder)
   : _state(state), _builder(builder)
{
   _state->ast()->accept(*this);
}

void ASTVisitorCompileStatic::visitChildren()
{
   for(ASTState* child: _state->children())
   {
      ASTVisitorCompileStatic {child, _builder};
   }
}

std::string ASTVisitorCompileStatic::getClassName() const
{
   assert(_state->parent()->type() == ASTType::class_def);
   ASTClass* c = dynamic_cast<ASTClass*> (_state->parent()->ast());
   return c->getName()->getName();
}

t_vm_int ASTVisitorCompileStatic::getClassId() const
{
   return _state->scope()->getClass(getClassName()).id;
}

void ASTVisitorCompileStatic::visit(ASTModule*)
{
   ASTState* main = _state->getChild(_state->scope()->getFunction("main"));
   std::string mainLabel = main->getLabel(0);

   if(LOG_COMPILE)
      std::cout << "compile: create internal _main function" << std::endl;

   _builder->addiNop("_main");
   visitChildren();

   t_vm_saddr scopeSize = _state->frame()->getSize();
   t_vm_saddr temp1 = _state->getVar(0);
   t_vm_saddr temp2 = _state->getVar(1);

   _builder->addiFjump(mainLabel, scopeSize);
   _builder->addiPutnull(temp1, DVAR_MVAR);
   _builder->addiNeq(scopeSize, temp1, temp2);
   _builder->addiCjump(temp2, "_main_end");
   _builder->addiPutint(scopeSize, DVAR_MVAR, 0);
   _builder->addiNop("_main_end");
   _builder->addiMove(0, scopeSize);
   _builder->addiSyscall(VM_SYSCALL_EXIT);
}




void ASTVisitorCompileStatic::visit(ASTGlobalDef* e)
{
   std::string name = e->getSymbol()->getName();
   GlobalVar g = _state->scope()->getGlobal(name);
   t_vm_mode mode = g.mode;
   std::string label = g.label;

   t_vm_saddr temp = _state->getVar(0);

   if(LOG_COMPILE)
      std::cout << "compile: global variable " + name << std::endl;

   _builder->addVar(label);

   if(e->hasValue())
   {
      ASTState* value = _state->getChild(e->getValue());
      ASTVisitorCompile {value, _builder};
      t_vm_saddr result = value->getVar(0);
      _builder->addiPutvar(temp, mode, result);
   }

   else
   {
      _builder->addiPutnull(temp, mode);
   }

   _builder->addiInit(temp, label);
}

void ASTVisitorCompileStatic::visit(ASTClass* e)
{
   std::string name = e->getName()->getName();
   t_vm_int len = static_cast<t_vm_int> (name.size());
   t_vm_int id = _state->scope()->getClass(name).id;
   std::string nameLabel = _state->getLabel(0);

   if(LOG_COMPILE)
      std::cout << "compile: class " + name << std::endl;

   _builder->addiDefclass(nameLabel, len, id);
   visitChildren();
   _builder->addiDefend(id);
}

void ASTVisitorCompileStatic::visit(ASTClassMethod* e)
{
   std::string name = e->getName()->getName();
   t_vm_int len = static_cast<t_vm_int> (name.size());
   std::string nameLabel = _state->getLabel(0);
   std::string label = _state->getLabel(1);
   t_vm_int id = getClassId();
   t_vm_saddr temp = _state->getVar(0);


   if(LOG_COMPILE)
      std::cout << "compile: method " + name << std::endl;

   _builder->addiPutfunction(temp, DVAR_MVAR, label);
   if(e->isStatic())
      _builder->addiDefsfield(id, nameLabel, len, 3, temp);
   else
      _builder->addiDeffield(id, nameLabel, len, 3, temp);
}

void ASTVisitorCompileStatic::visit(ASTClassVariable* e)
{
   std::string name = e->getSymbol()->getName();
   t_vm_int len = static_cast<t_vm_int> (name.size());
   std::string nameLabel = _state->getLabel(0);
   t_vm_int id = getClassId();
   t_vm_int type = e->isConst() ? 2 : 1;
   t_vm_saddr temp;

   if(LOG_COMPILE)
      std::cout << "compile: class variable " + name << std::endl;

   if(e->hasValue())
   {
      ASTState* value = _state->getChild(e->getValue());
      temp = value->getVar(0);
      ASTVisitorCompile {value, _builder};
   }

   else
   {
      temp = _state->getVar(0);
      _builder->addiPutnull(temp, DVAR_MVAR);
   }

   if(e->isStatic())
      _builder->addiDefsfield(id, nameLabel, len, type, temp);
   else
      _builder->addiDeffield(id, nameLabel, len, type, temp);
}
