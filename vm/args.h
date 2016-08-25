#ifndef ARGS_H_
# define ARGS_H_

# include "slist.h"



int argsHasOption(const SList* l, char c, const char* s);

SList* argsGetParameters(const SList* l);

#endif //!ARGS_H_
