#include "ast-state.hh"
#include "ast-visitor-scope.hh"
#include "ast-infos.hh"
#include "runtime-scope.hh"
#include "scanner.hh"
#include <cassert>

ASTState::ASTState(AST* ast, Scanner* scanner)
   : ASTState(ast, scanner, nullptr)
{

}

ASTState::~ASTState()
{
   for(ASTState* child: _children)
      delete child;

   if(_scope->hasFrame())
   {
      if(!_parent || !_parent->_scope->hasFrame()
         || _parent->frame() != frame())
         delete frame();
   }

   if(!_parent || _parent->_scope != _scope)
      delete _scope;
}

ASTState::ASTState(AST* ast, Scanner* scanner, RuntimeScope* parentScope)
   : _ast(ast), _scanner(scanner), _parent(nullptr)
{
   _scope = ASTVisitorScope::getScope(_ast, parentScope);
   _infos = ASTInfos::getInfos(_ast);



   for(AST* astChild: _ast->getChildren())
   {
      ASTState* stateChild = new ASTState(astChild, _scanner, _scope);
      _children.push_back(stateChild);
      stateChild->_parent = this;
   }
}

AST* ASTState::ast() const
{
   return _ast;
}

Scanner* ASTState::scanner() const
{
   return _scanner;
}

RuntimeScope* ASTState::scope() const
{
   return _scope;
}

StackFrame* ASTState::frame() const
{
   return _scope->getFrame();
}

ASTType ASTState::type() const
{
   return _infos.type;
}

ASTState* ASTState::parent() const
{
   return _parent;
}

ASTState* ASTState::root()
{
   ASTState* root = this;
   while(root->_parent)
      root = root->_parent;
   return root;
}

std::vector<ASTState*> ASTState::children() const
{
   return _children;
}

ASTState* ASTState::child(std::size_t pos) const
{
   assert(pos < _children.size());
   return _children[pos];
}

ASTState* ASTState::getChild(const AST* child) const
{
   for(ASTState* s: _children)
   {
      if(s->_ast == child)
         return s;
   }

   assert(0);
}

ASTState* ASTState::findChild(const AST* child) const
{
   for(ASTState* s: _children)
   {
      if(s->_ast == child)
         return s;
      s = s->findChild(child);
      if(s)
         return s;
   }

   return nullptr;
}


t_vm_saddr ASTState::getVar(std::size_t pos) const
{
   assert(pos < _vars.size());
   return _vars[pos];
}

void ASTState::addVar(t_vm_saddr v)
{
   _vars.push_back(v);
}

std::string ASTState::getLabel(std::size_t pos) const
{
   assert(pos < _labels.size());
   return _labels[pos];
}

void ASTState::addLabel(const std::string& label)
{
   _labels.push_back(label);
}


void ASTState::tokenError(const std::string& message)
{
   _scanner->tokenError(_ast->getToken(), message);
}
