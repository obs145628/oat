#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "fs.h"
#include "numbers.h"
#include "str.h"
#include "dstr.h"
#include "io.h"
#include "date.h"
#include "err.h"
#include "path.h"
#include "slist.h"
#include "pset.h"
#include "pmap.h"
#include "json.h"
#include "phtable.h"
#include "app.h"

#define PATH1 "/home/obs/Documents/test.json"
#define PATH2 "/home/obs/Documents/oat-books"
#define PATH3 "/home/obs/Documents/unknown"

int main(int argc, char** argv)
{
   app_init("oat-tests", argc, argv);

   printf("[%s]\n", app_get_data_path());

   return 0;
}
