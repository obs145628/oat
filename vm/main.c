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

#include "http-client.h"
#include "http-request.h"
#include "http-response.h"
#include "http-server.h"

static void on_request_(s_http_request* req, s_http_response* res)
{
   printf("=======Request=======\n");

   printf("Method: %d\n", req->method);
   printf("Version: %d\n", req->version);
   printf("Path: %s\n", req->path);

   s_pmap_node* it = pmap_begin(req->headers);
   while(it != pmap_end(req->headers))
   {
      printf("%s => %s\n", (const char*)it->key, (const char*) it->value);
      it = pmap_next(req->headers, it);
   }

   if(req->body_buff)
   {
      printf("\n\n<BEGIN>");
      fwrite(req->body_buff, 1, req->body_len, stdout);
      printf("<END>\n");
   }

   printf("=====================\n");

   const char* message = "Hello, World !\n";
   http_response_body_write(res, message, strlen(message));
}

int main(int argc, char** argv)
{

   s_http_server* server = http_server_new(on_request_, 3333);
   http_server_listen(server);


   return 0;

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
