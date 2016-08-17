#include "stack-frame.hh"
#include <cassert>


StackFrame::StackFrame()
   : _size(0)
{

}

t_vm_saddr StackFrame::addVar()
{
   t_vm_saddr n = 0;
   while(true)
   {
      if(_vars.find(n) == _vars.end())
      {
         _vars.insert(n);
         break;
      }
      ++n;
   }

   if(n == _size)
      ++_size;
   return n;
}

void StackFrame::removeVar(t_vm_saddr addr)
{
   auto it = _vars.find(addr);
   assert(it != _vars.end());
   _vars.erase(it);
}

t_vm_saddr StackFrame::getSize() const
{
   return _size;
}
