#ifndef AST_STATE_HH_
# define AST_STATE_HH_

# include <vector>
# include <string>
# include "ast-infos.hh"
# include "../vm/const.h"

class AST;
class Scanner;
class RuntimeScope;
class StackFrame;

class ASTState
{
public:
   ASTState(AST* ast);
   ~ASTState();

   ASTState(const ASTState&) = delete;

private:
   ASTState(AST* ast, RuntimeScope* parentScope);

public:

   AST* ast() const;
   Scanner* scanner() const;
   RuntimeScope* scope() const;
   StackFrame* frame() const;
   ASTType type() const;
   ASTState* parent() const;
   ASTState* root();

   std::vector<ASTState*> children() const;
   ASTState* child(std::size_t pos) const;
   ASTState* getChild(const AST* child) const;
   ASTState* findChild(const AST* child) const;

   t_vm_saddr getVar(std::size_t pos) const;
   void addVar(t_vm_saddr v);

   std::string getLabel(std::size_t pos) const;
   void addLabel(const std::string& label);

   void tokenError(const std::string& message);

private:
   AST* _ast;
   RuntimeScope* _scope;
   ASTState* _parent;
   ASTInfos _infos;
   std::vector<t_vm_saddr> _vars;
   std::vector<std::string> _labels;
   std::vector<ASTState*> _children;

};

#endif //!AST_STATE_HH_
