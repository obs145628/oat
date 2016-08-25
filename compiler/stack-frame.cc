#include "stack-frame.hh"
#include <cassert>
#include <iostream>


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

t_vm_saddr StackFrame::addBlock(t_vm_saddr size)
{
   t_vm_saddr n = 0;
   t_vm_saddr cur = 0;

   while(cur < size)
   {
      if(_vars.find(n) == _vars.end())
         ++cur;
      else
         cur = 0;

      ++n;
   }

   n -= size;

   for(t_vm_saddr i = n; i < n + size; ++i)
      _vars.insert(i);

   if(n + size > _size)
      _size = n + size;
   return n;
}

void StackFrame::removeVar(t_vm_saddr addr)
{
   auto it = _vars.find(addr);
   assert(it != _vars.end());
   _vars.erase(it);
}

void StackFrame::removeBlock(t_vm_saddr addr, t_vm_saddr n)
{
   for(t_vm_saddr i = addr; i < addr + n; ++i)
      removeVar(i);
}

t_vm_saddr StackFrame::getSize() const
{
   return _size;
}
