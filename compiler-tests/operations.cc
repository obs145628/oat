#include "operations.hh"
#include "unit.hh"
#include "stack.hh"
#include "scanner.hh"
#include <iostream>

void op_print(Unit& unit)
{
   std::cout << unit.stack().popToken() << std::endl;
}

void op_plus(Unit& unit)
{
   double a = unit.stack().popNumber();
   double b = unit.stack().popNumber();
   unit.stack().pushNumber(a + b);
}

void op_minus(Unit& unit)
{
   double a = unit.stack().popNumber();
   double b = unit.stack().popNumber();
   unit.stack().pushNumber(a - b);
}

void op_multiplies(Unit& unit)
{
   double a = unit.stack().popNumber();
   double b = unit.stack().popNumber();
   unit.stack().pushNumber(a * b);
}

void op_divides(Unit& unit)
{
   double a = unit.stack().popNumber();
   double b = unit.stack().popNumber();
   unit.stack().pushNumber(a / b);
}

void op_var(Unit& unit)
{
   //std::string name = unit.scanner().nextToken();
   unit.varTable().put("", "");
}

void op_load(Unit& unit)
{
   std::string name = unit.stack().popToken();
   std::string value = unit.varTable().get(name);
   unit.stack().pushToken(value);
}

void op_store(Unit& unit)
{
   std::string name = unit.stack().popToken();
   std::string value = unit.stack().popToken();
   unit.varTable().set(name, value);
   unit.stack().pushToken(value);
}
