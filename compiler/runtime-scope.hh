#ifndef RUNTIME_SCOPE_HH_
# define RUNTIME_SCOPE_HH_

# include <map>
# include <string>
# include "stack-frame.hh"

class ASTFunctionDef;

struct RuntimeVar
{
   t_vm_type type;
   t_vm_mode mode;
   t_vm_saddr pos;
};

struct GlobalVar
{
   t_vm_type type;
   t_vm_mode mode;
   bool initialized;
   std::string label;
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
   void defineVar(const std::string& name, t_vm_type type, t_vm_mode mode);
   void setVar(const std::string& name, t_vm_type type);

   bool hasFunction(const std::string& name);
   ASTFunctionDef* getFunction(const std::string& name);
   void defineFunction(const std::string& name, ASTFunctionDef* function);

   bool hasGlobal(const std::string& name);
   GlobalVar getGlobal(const std::string& name);
   void defineGlobal(const std::string& name, const std::string& label,
                     t_vm_type type, t_vm_mode mode);
   void setGlobal(const std::string& name, t_vm_type type);
   void initGlobal(const std::string& name);

   bool hasGlobalSymbol(const std::string& name);



private:
   std::map<std::string, RuntimeVar> _vars;
   std::map<std::string, ASTFunctionDef*> _fns;
   std::map<std::string, GlobalVar> _globals;
   StackFrame* _frame;
   RuntimeScope* _parent;

};

#endif //!RUNTIME_SCOPE_HH_
