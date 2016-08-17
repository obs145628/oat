#ifndef RUNTIME_SCOPE_HH_
# define RUNTIME_SCOPE_HH_

# include <map>
# include <string>
# include "stack-frame.hh"

class ASTFunctionDef;

struct RuntimeVar
{
   char type;
   t_vm_saddr pos;
};

class RuntimeScope
{

public:
   RuntimeScope(StackFrame* frame, RuntimeScope* parent = nullptr);

   RuntimeScope* getRoot();

   RuntimeScope* getParent();
   bool hasFrame() const;
   StackFrame* getFrame() const;

   bool hasVar(const std::string& name) const;
   bool ownVar(const std::string& name) const;

   RuntimeVar getVar(const std::string& name) const;
   void defineVar(const std::string& name, char type);
   void setVar(const std::string& name, char type);

   bool hasFunction(const std::string& name);
   ASTFunctionDef* getFunction(const std::string& name);
   void defineFunction(const std::string& name, ASTFunctionDef* function);

private:
   std::map<std::string, RuntimeVar> _vars;
   std::map<std::string, ASTFunctionDef*> _fns;
   StackFrame* _frame;
   RuntimeScope* _parent;

};

#endif //!RUNTIME_SCOPE_HH_
