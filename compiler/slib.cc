#include "slib.hh"
#include <cassert>

bool SLib::hasFunction(const std::string& name)
{
   return _funs.find(name) != _funs.end();
}

SLibFunction SLib::getFunction(const std::string& name)
{
   assert(hasFunction(name));
   return _funs.find(name)->second;
}

t_vm_int SLib::getNbClasses()
{
   return static_cast<t_vm_int> (_classes.size());
}

bool SLib::hasClass(const std::string name)
{
   return _classes.find(name) != _classes.end();
}

t_vm_int SLib::getClass(const std::string& name)
{
   assert(hasClass(name));
   return _classes.find(name)->second;
}

const std::map<std::string, SLibFunction> SLib::_funs = {
   {"exit", {VM_SYSCALL_EXIT}},
   {"kprint", {VM_SYSCALL_KPRINT}},
   {"print", {VM_SYSCALL_PRINT}}
};

const std::map<std::string, t_vm_int> SLib::_classes  = {
   {"Rect", VM_CLASS_RECT},
   {"Date", VM_CLASS_DATE}
};
