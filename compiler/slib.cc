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
   {"print", {VM_SYSCALL_PRINT}},
   {"typename", {VM_SYSCALL_TYPENAME}},
   {"isNull", {VM_SYSCALL_IS_NULL}},
   {"isInt", {VM_SYSCALL_IS_INT}},
   {"isDouble", {VM_SYSCALL_IS_DOUBLE}},
   {"isChar", {VM_SYSCALL_IS_CHAR}},
   {"isBool", {VM_SYSCALL_IS_BOOL}},
   {"isString", {VM_SYSCALL_IS_STRING}},
   {"isFunction", {VM_SYSCALL_IS_FUNCTION}},
   {"isArray", {VM_SYSCALL_IS_ARRAY}},
   {"isSet", {VM_SYSCALL_IS_SET}},
   {"isMap", {VM_SYSCALL_IS_MAP}},
   {"isObject", {VM_SYSCALL_IS_OBJECT}},
   {"isClass", {VM_SYSCALL_IS_CLASS}},
   {"isInstanceOf", {VM_SYSCALL_IS_INSTANCE_OF}},
   {"min", {VM_SYSCALL_MIN}},
   {"max", {VM_SYSCALL_MAX}},
   {"abs", {VM_SYSCALL_ABS}},
   {"round", {VM_SYSCALL_ROUND}},
   {"floor", {VM_SYSCALL_FLOOR}},
   {"ceil", {VM_SYSCALL_CEIL}},
   {"fmod", {VM_SYSCALL_FMOD}},
   {"exp", {VM_SYSCALL_EXP}},
   {"log", {VM_SYSCALL_LOG}},
   {"pow", {VM_SYSCALL_POW}},
   {"sqrt", {VM_SYSCALL_SQRT}},
   {"sin", {VM_SYSCALL_SIN}},
   {"cos", {VM_SYSCALL_COS}},
   {"tan", {VM_SYSCALL_TAN}},
   {"asin", {VM_SYSCALL_ASIN}},
   {"acos", {VM_SYSCALL_ACOS}},
   {"atan", {VM_SYSCALL_ATAN}},
   {"sinh", {VM_SYSCALL_SINH}},
   {"cosh", {VM_SYSCALL_COSH}},
   {"tanh", {VM_SYSCALL_TANH}},
   {"asinh", {VM_SYSCALL_ASINH}},
   {"acosh", {VM_SYSCALL_ACOSH}},
   {"atanh", {VM_SYSCALL_ATANH}},
   {"promptLine", {VM_SYSCALL_PROMPT_LINE}},
   {"promptInt", {VM_SYSCALL_PROMPT_INT}},
   {"promptDouble", {VM_SYSCALL_PROMPT_DOUBLE}},
   {"promptChar", {VM_SYSCALL_PROMPT_CHAR}},
   {"promptBool", {VM_SYSCALL_PROMPT_BOOL}},
};

const std::map<std::string, t_vm_int> SLib::_classes  = {
   {"Rect", VM_CLASS_RECT},
   {"Date", VM_CLASS_DATE},
   {"ArrayIterator", VM_CLASS_ARRAY_ITERATOR},
   {"SetIterator", VM_CLASS_SET_ITERATOR},
   {"MapIterator", VM_CLASS_MAP_ITERATOR},
   {"RandomGenerator", VM_CLASS_RANDOM_GENERATOR}
};
