#ifndef STACK_HH_
# define STACK_HH_

#include <string>
#include <stack>

class Stack
{
public:
   bool empty() const;
   size_t size() const;

   std::string popToken();
   void pushToken(const std::string& token);

   double popNumber();
   void pushNumber(double x);


private:
   std::stack<std::string> _stack;

};

# endif //!STACK_HH_
