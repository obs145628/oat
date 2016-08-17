#ifndef SLIB_HH_
# define SLIB_HH_

# include <map>
# include <string>
# include "../vm/const.h"

struct SLibFunction
{
   std::size_t nbArgs;
   t_vm_int syscall;
};

class SLib
{
public:
   static bool hasFunction(const std::string& name);
   static SLibFunction getFunction(const std::string& name);

private:
   static const std::map<std::string, SLibFunction> _funs;

};

#endif //!SLIB_HH_
