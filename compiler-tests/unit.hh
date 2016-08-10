#ifndef UNIT_HH_
# define UNIT_HH_

# include "operations.hh"
# include "scanner.hh"
# include "stack.hh"
# include "operations-table.hh"
# include "var-table.hh"

class Unit
{

public:
   Unit(const std::string& path);

   void run();

   Scanner& scanner();
   Stack& stack();
   OperationsTable& opsTable();
   VarTable& varTable();

   void execOp(const std::string& name);

private:
   std::string _path;
   Scanner _scanner;
   Stack _stack;
   OperationsTable _opsTable;
   VarTable _varTable;

};


#endif //!UNIT_HH_
