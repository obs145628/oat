#include "app.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "fs.h"


typedef struct
{
   const char* name;
   SList* args;
   char* dataPath;
} Application;

static Application* app_get()
{
   static Application app;
   return &app;
}

void app_init(const char* name, int argc, char** argv)
{
   static int init = 0;
   assert(!init);
   init = 1;

   Application* app = app_get();
   app->name = name;

   SList* args = SListNew();
   for(int i = 0; i < argc; ++i)
      SListInsertBack(args, argv[i]);
   app->args = args;

   char* homePath = fsHomePath();
   size_t homePathLen = strlen(homePath);
   size_t nameLen = strlen(name);

   size_t dataPathLen = homePathLen + nameLen + 2;
   char* dataPath = malloc(dataPathLen + 1);
   memcpy(dataPath, homePath, homePathLen);
   dataPath[homePathLen] = '/';
   dataPath[homePathLen + 1] = '.';
   memcpy(dataPath + homePathLen + 2, name, nameLen);
   dataPath[dataPathLen] = '\0';

   free(homePath);
   app->dataPath = dataPath;
}

const char* app_get_name()
{
   return app_get()->name;
}

SList* app_get_args()
{
   return app_get()->args;
}

char* app_get_data_path()
{
   return app_get()->dataPath;
}
