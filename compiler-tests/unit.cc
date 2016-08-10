#include "unit.hh"
#include "scanner.hh"
#include "stack.hh"
#include <stdexcept>

Unit::Unit(const std::string& path)
   : _path(path),
     _scanner(_path),
     _stack(),
     _opsTable(),
     _varTable()
{

}

void Unit::run()
{
   Token token;
   while(_scanner.isEof())
   {
      token = _scanner.nextToken();

      if(_opsTable.has(token.getRepresentation()))
      {
         _opsTable.exec(token.getRepresentation(), *this);
      }

      else
      {
         _stack.pushToken(token.getRepresentation());
      }
   }

}

void Unit::execOp(const std::string& name)
{
   _opsTable.exec(name, *this);
}

Scanner& Unit::scanner()
{
   return _scanner;
}

Stack& Unit::stack()
{
   return _stack;
}

OperationsTable& Unit::opsTable()
{
   return _opsTable;
}

VarTable& Unit::varTable()
{
   return _varTable;
}
