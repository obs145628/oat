#include "var-table.hh"
#include <stdexcept>

bool VarTable::contains(const std::string& key) const
{
   return _map.find(key) != _map.end();
}

std::string VarTable::get(const std::string& key) const
{
   auto it = _map.find(key);
   if(it == _map.end())
      throw std::invalid_argument("Undefined variable '" + key + "'");
   return it->second;
}

void VarTable::set(const std::string& key, const std::string& value)
{
   auto it = _map.find(key);
   if(it == _map.end())
      throw std::invalid_argument("Undefined variable '" + key + "'");
   else
      it->second = value;
}

void VarTable::put(const std::string& key, const std::string& value)
{
   if(_map.find(key) != _map.end())
      throw std::invalid_argument("Variable '" + key + "' already defined.");
   _map[key] = value;
}

void VarTable::remove(const std::string& key)
{
   auto it = _map.find(key);
   if(it == _map.end())
      throw std::invalid_argument("Undefined variable '" + key + "'");
   _map.erase(it);
}
