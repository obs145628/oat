#ifndef OPERATIONS_TABLE_HH_
# define OPERATIONS_TABLE_HH_

#include <map>
#include <string>
#include "operations.hh"

class OperationsTable
{

public:
   OperationsTable();

   bool has(const std::string& name) const;
   void add(const std::string& name, f_operation op);
   void remove(const std::string& name);
   void exec(const std::string& name, Unit& unit);


private:
   std::map<std::string, f_operation> _ops;

};

#endif //!OPERATIONS_TABLE_HH_
