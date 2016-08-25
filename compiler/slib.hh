#ifndef SLIB_HH_
# define SLIB_HH_

# include <map>
# include <string>
# include "../vm/const.h"

struct SLibFunction
{
   t_vm_int syscall;
};

class SLib
{
public:
   static bool hasFunction(const std::string& name);
   static SLibFunction getFunction(const std::string& name);

   static t_vm_int getNbClasses();
   static bool hasClass(const std::string name);
   static t_vm_int getClass(const std::string& name);

private:
   static const std::map<std::string, SLibFunction> _funs;
   static const std::map<std::string, t_vm_int> _classes;

};

#endif //!SLIB_HH_
