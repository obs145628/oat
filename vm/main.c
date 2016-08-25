#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vm-bin.h"
#include "vm-debug.h"
#include "vm-exec.h"
#include "vm-args.h"

#include "slist.h"
#include "args.h"
#include "path.h"
#include "str.h"

# define BIN "/home/obs/it/oat/tests/main.oatbin"

int main(int argc, char** argv)
{

   vm_args_init(argc, argv);

   SList* args = SListNewFromIt(argv, argv + argc);

   SList* p = argsGetParameters(args);

   if(!p->len)
   {
      fprintf(stderr, "No input file\n");
      return 1;
   }

   int modeDebug = argsHasOption(args, 'd', "debug");
   int modePrint = argsHasOption(args, 'p', "print");

   char* path = *(p->begin);
   char* ext = pathExtname(path);

   if(strcmp(ext, ".oat") == 0)
   {
      char* tempFile = strClone("/tmp/out.oatbin");
      char* cmd = strPrintf("oatc %s -o %s", path, tempFile);
      int code = system(cmd);
      if(code != 0)
         return code;

      free(cmd);
      path = tempFile;
   }

   else if(strcmp(ext, ".oatbin") != 0)
   {
      fprintf(stderr, "Invalid file extension\n");
      return 1;
   }

   free(ext);



   vm_bin_load_file(path);
   vm_exec_init();

   if(modePrint)
      vm_debug_print_code();

   if(modeDebug)
      vm_debug_step_by_step();
   else
      vm_exec_run();

   SListFree(args);
   SListFree(p);
   return 0;
}
