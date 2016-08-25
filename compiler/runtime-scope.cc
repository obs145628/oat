#include "runtime-scope.hh"
#include <cassert>
#include "slib.hh"
#include "ast-functions.hh"
#include "ast-modules.hh"
#include "ast-values.hh"
#include "ast-class.hh"
#include "../vm/dvar.h"

RuntimeScope::RuntimeScope(Scanner* scanner, StackFrame* frame,
                           RuntimeScope* parent)
   : _nextClassId(SLib::getNbClasses()),
   _scanner(scanner), _frame(frame), _parent(parent)
{

}

RuntimeScope* RuntimeScope::getRoot()
{
   return _parent ? _parent->getRoot() : this;
}

RuntimeScope* RuntimeScope::getParent()
{
   assert(_parent);
   return _parent;
}

bool RuntimeScope::hasFrame() const
{
   return !!_frame;
}

StackFrame* RuntimeScope::getFrame() const
{
   assert(_frame);
   return _frame;
}

bool RuntimeScope::hasVar(const std::string& name) const
{
   return ownVar(name) || (_parent && _parent->hasVar(name));
}

bool RuntimeScope::ownVar(const std::string& name) const
{
   return _vars.find(name) != _vars.end();
}

RuntimeVar RuntimeScope::getVar(const std::string& name) const
{
   assert(hasVar(name));
   auto it = _vars.find(name);
   return it == _vars.end() ? _parent->getVar(name) : it->second;
}

void RuntimeScope::defineVar(const std::string& name,
                             t_vm_type type, t_vm_mode mode)
{
   assert(!ownVar(name));
   RuntimeVar rvar;
   rvar.type = type;
   rvar.mode = mode;
   rvar.pos = getFrame()->addVar();
   _vars[name] = rvar;
}

void RuntimeScope::setVar(const std::string& name, t_vm_type type)
{
   assert(hasVar(name));
   auto it = _vars.find(name);
   if(it == _vars.end())
      _parent->setVar(name, type);
   else
      it->second.type = type;
}

bool RuntimeScope::hasFunction(const std::string& name)
{
   RuntimeScope* root = getRoot();
   auto it = root->_fns.find(name);
   if(it == root->_fns.end())
      return false;

   std::vector<ASTFunctionDef*>& fns = it->second;
   for(ASTFunctionDef* f: fns)
      if(f->isExported() || f->getToken().getScanner() == _scanner)
         return true;

   return false;
}

ASTFunctionDef* RuntimeScope::getFunction(const std::string& name)
{
   assert(hasFunction(name));
   RuntimeScope* root = getRoot();
   auto it = root->_fns.find(name);

   std::vector<ASTFunctionDef*>& fns = it->second;
   for(ASTFunctionDef* f: fns)
      if(f->isExported() || f->getToken().getScanner() == _scanner)
         return f;

   return nullptr;
}

void RuntimeScope::defineFunction(const std::string& name,
                                  ASTFunctionDef* function)
{
   assert(!hasGlobalSymbol(name));
   RuntimeScope* root = getRoot();
   auto it = root->_fns.find(name);

   if(it == root->_fns.end())
   {
      root->_fns[name] = {function};
   }

   else
   {
      it->second.push_back(function);
   }
}


bool RuntimeScope::hasGlobal(const std::string& name)
{
   RuntimeScope* root = getRoot();
   auto it = root->_globals.find(name);
   if(it == root->_globals.end())
      return false;

   std::vector<GlobalVar>& globals = it->second;
   for(const GlobalVar&  g: globals)
      if(g.ast->isExported() || g.ast->getToken().getScanner() == _scanner)
         return true;

   return false;
}

GlobalVar RuntimeScope::getGlobal(const std::string& name)
{
   assert(hasGlobal(name));
   RuntimeScope* root = getRoot();
   auto it = root->_globals.find(name);

   std::vector<GlobalVar>& globals = it->second;
   for(const GlobalVar&  g: globals)
      if(g.ast->isExported() || g.ast->getToken().getScanner() == _scanner)
         return g;

   return globals[0];
}

void RuntimeScope::defineGlobal(ASTGlobalDef* g,
                                const std::string& label, t_vm_type type)
{
   std::string name = g->getSymbol()->getName();
   assert(!hasGlobalSymbol(name));
   GlobalVar global;
   global.type = type;
   global.mode = g->isConst() ? DVAR_MCONST : DVAR_MVAR;
   global.initialized = false;
   global.label = label;
   global.ast = g;

   RuntimeScope* root = getRoot();
   auto it = root->_globals.find(name);

   if(it == root->_globals.end())
   {
      root->_globals[name] = {global};
   }

   else
   {
      it->second.push_back(global);
   }
}

bool RuntimeScope::hasGlobalSymbol(const std::string& name)
{
   return hasFunction(name) || hasGlobal(name) || hasClass(name)
      || SLib::hasFunction(name) || SLib::hasClass(name);
}

bool RuntimeScope::hasClass(const std::string& name)
{
   RuntimeScope* root = getRoot();
   auto it = root->_classes.find(name);
   if(it == root->_classes.end())
      return false;

   std::vector<RuntimeClass>& classes = it->second;
   for(RuntimeClass c: classes)
      if(c.ast->isExported() || c.ast->getToken().getScanner() == _scanner)
         return true;

   return false;
}

RuntimeClass RuntimeScope::getClass(const std::string& name)
{
   assert(hasClass(name));
   RuntimeScope* root = getRoot();
   auto it = root->_classes.find(name);

   std::vector<RuntimeClass>& classes = it->second;
   for(RuntimeClass c: classes)
      if(c.ast->isExported() || c.ast->getToken().getScanner() == _scanner)
         return c;

   //never happens
   throw std::runtime_error{"RuntimeScope::getClass"};
}

void RuntimeScope::defineClass(const std::string& name, ASTClass* ast)
{
   assert(!hasGlobalSymbol(name));
   RuntimeScope* root = getRoot();

   RuntimeClass c;
   c.ast = ast;
   c.id = root->_nextClassId++;

   auto it = root->_classes.find(name);

   if(it == root->_classes.end())
   {
      root->_classes[name] = {c};
   }

   else
   {
      it->second.push_back(c);
   }
}
