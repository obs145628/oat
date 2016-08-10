#ifndef OPERATIONS_HH_
# define OPERATIONS_HH_

class Unit;

typedef void (*f_operation)(Unit& unit);

void op_print(Unit& unit);
void op_plus(Unit& unit);
void op_minus(Unit& unit);
void op_multiplies(Unit& unit);
void op_divides(Unit& unit);
void op_var(Unit& unit);
void op_load(Unit& unit);
void op_store(Unit& unit);

#endif //!OPERATIONS_H_
