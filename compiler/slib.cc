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

const std::map<std::string, SLibFunction> SLib::_funs = {
   {"exit", {VM_SYSCALL_EXIT}},
   {"kprint", {VM_SYSCALL_KPRINT}},
   {"print", {VM_SYSCALL_PRINT}}
};
