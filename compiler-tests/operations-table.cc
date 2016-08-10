#include "operations-table.hh"

OperationsTable::OperationsTable()
   : _ops({
           {"print", op_print},
           {"+", op_plus},
           {"-", op_minus},
           {"*", op_multiplies},
           {"/", op_divides},
           {"var", op_var},
           {"load", op_load},
           {"store", op_store}
        })
{

}

bool OperationsTable::has(const std::string& name) const
{
   return _ops.find(name) != _ops.end();
}

void OperationsTable::add(const std::string& name, f_operation op)
{
   if(_ops.find(name) != _ops.end())
      throw std::runtime_error("Operation " + name + "already exists");

   _ops[name] = op;
}

void OperationsTable::remove(const std::string& name)
{
   auto it = _ops.find(name);
   if(it == _ops.end())
      throw std::runtime_error("Operation " + name + "doesn't exist");
   _ops.erase(it);
}

void OperationsTable::exec(const std::string& name, Unit& unit)
{
   auto it = _ops.find(name);
   if(it == _ops.end())
      throw std::runtime_error("Operation " + name + "doesn't exist");
   it->second(unit);
}
