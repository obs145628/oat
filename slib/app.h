#ifndef APP_H_
# define APP_H_

# include "slist.h"

void app_init(const char* name, int argc, char** argv);

const char* app_get_name();
SList* app_get_args();
char* app_get_data_path();

#endif //!APP_H_
