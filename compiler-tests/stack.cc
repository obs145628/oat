#include "stack.hh"
#include <stdexcept>
#include <cstdlib>


bool Stack::empty() const
{
   return _stack.empty();
}

size_t Stack::size() const
{
   return _stack.size();
}

std::string Stack::popToken()
{
   if(_stack.empty())
      throw std::runtime_error("missing operand");
   std::string value = _stack.top();
   _stack.pop();
   return value;
}

void Stack::pushToken(const std::string& token)
{
   _stack.push(token);
}

double Stack::popNumber()
{
   std::string token = popToken();
   const char* str = token.c_str();
   char* end = 0;

   double value = strtod(str, &end);
   if(end == str || *end != '\0')
      throw std::runtime_error("Operand must be a number");
   return value;
}

void Stack::pushNumber(double x)
{
   _stack.push(std::to_string(x));
}
