#include "runtime-scope.hh"
#include <cassert>
#include "slib.hh"

RuntimeScope::RuntimeScope(StackFrame* frame, RuntimeScope* parent)
   : _frame(frame), _parent(parent)
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
   return root->_fns.find(name) != root->_fns.end();
}

ASTFunctionDef* RuntimeScope::getFunction(const std::string& name)
{
   assert(hasFunction(name));
   return getRoot()->_fns[name];
}

void RuntimeScope::defineFunction(const std::string& name,
                                  ASTFunctionDef* function)
{
   assert(!hasGlobalSymbol(name));
   getRoot()->_fns[name] = function;
}


bool RuntimeScope::hasGlobal(const std::string& name)
{
   RuntimeScope* root = getRoot();
   return root->_globals.find(name) != root->_globals.end();
}

GlobalVar RuntimeScope::getGlobal(const std::string& name)
{
   assert(hasGlobal(name));
   return getRoot()->_globals[name];
}

void RuntimeScope::defineGlobal(const std::string& name,
                                const std::string& label,
                                t_vm_type type, t_vm_mode mode)
{
   assert(!hasGlobalSymbol(name));
   GlobalVar global;
   global.type = type;
   global.mode = mode;
   global.initialized = false;
   global.label = label;


   getRoot()->_globals[name] = global;
}

void RuntimeScope::setGlobal(const std::string& name, t_vm_type type)
{
   assert(hasGlobal(name));
   auto it = getRoot()->_globals.find(name);
   it->second.type = type;
}

void RuntimeScope::initGlobal(const std::string& name)
{
   assert(hasGlobal(name));
   assert(!(getGlobal(name).initialized));
   auto it = getRoot()->_globals.find(name);
   it->second.initialized = true;
}

bool RuntimeScope::hasGlobalSymbol(const std::string& name)
{
   return hasFunction(name) || hasGlobal(name) || SLib::hasFunction(name);
}
