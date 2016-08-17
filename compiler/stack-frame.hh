#ifndef STACK_FRAME_HH_
# define STACK_FRAME_HH_

# include "../vm/const.h"
# include <set>

class StackFrame
{
public:
   StackFrame();

   t_vm_saddr addVar();

   void removeVar(t_vm_saddr addr);

   t_vm_saddr getSize() const;

private:
   std::set<t_vm_saddr> _vars;
   t_vm_saddr _size;

};

#endif //!STACK_FRAME_HH_
