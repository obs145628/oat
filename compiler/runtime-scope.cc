#include "runtime-scope.hh"
#include <cassert>

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

void RuntimeScope::defineVar(const std::string& name, char type)
{
   assert(!ownVar(name));
   RuntimeVar rvar;
   rvar.type = type;
   rvar.pos = getFrame()->addVar();
   _vars[name] = rvar;
}

void RuntimeScope::setVar(const std::string& name, char type)
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
   assert(!hasFunction(name));
   getRoot()->_fns[name] = function;
}
