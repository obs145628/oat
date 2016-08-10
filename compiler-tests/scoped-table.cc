#include "scoped-table.hh"
#include <stdexcept>

ScopedTable::ScopedTable(ScopedTable* parent)
   : _parent(parent)
{

}

void ScopedTable::define(const std::string& key, long value)
{
   if(_table.find(key) != _table.end())
      throw std::runtime_error{"Variable " + key + " already defined"};
   _table[key] = value;
}

void ScopedTable::set(const std::string& key, long value)
{
   auto it = _table.find(key);

   if(it == _table.end())
   {
      if(_parent)
         _parent->set(key, value);
      else
         throw std::runtime_error{"Undefined variable " + key};
   }

   else
   {
      it->second = value;
   }
}

long ScopedTable::get(const std::string& key) const
{
   auto it = _table.find(key);

   if(it == _table.end())
   {
      if(_parent)
         return _parent->get(key);
      else
         throw std::runtime_error{"Undefined variable " + key};
   }

   else
   {
      return it->second;
   }
}
