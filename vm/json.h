#ifndef JSON_H_
# define JSON_H_

# include <stdint.h>
# include "parray.h"
# include "pmap.h"
# include "json.h"
# include "io.h"


# define JSON_NULL (0)
# define JSON_BOOL (1)
# define JSON_INT (2)
# define JSON_REAL (3)
# define JSON_STRING (4)
# define JSON_ARRAY (5)
# define JSON_MAP (6)

typedef struct {
   char type;
   union {
      int j_bool;
      int64_t j_int;
      double j_real;
      char* j_str;
      s_parray* j_arr;
      s_pmap* j_map;
   };
} json;

json* json_new_null();
json* json_new_bool(int x);
json* json_new_int(int64_t x);
json* json_new_real(double x);
json* json_new_str(const char* x);
json* json_new_arr();
json* json_new_map();
json* json_clone(const json* j);
void json_free(json* j);
int json_equals(const json* a, const json* b);

void json_assign_null(json* j);
void json_assign_bool(json* j, int x);
void json_assign_int(json* j, int64_t x);
void json_assign_real(json* j, double x);
void json_assign_str(json* j, const char* x);
void json_assign_arr(json* j);
void json_assign_map(json* j);

int json_is_null(const json* j);
int json_is_bool(const json* j);
int json_is_int(const json* j);
int json_is_real(const json* j);
int json_is_str(const json* j);
int json_is_arr(const json* j);
int json_is_map(const json* j);

int json_get_bool(const json* j);
int64_t json_get_int(const json* j);
double json_get_real(const json* j);
char* json_get_str(const json* j);
s_parray* json_get_arr(const json* j);
s_pmap* json_get_map(const json* j);

size_t json_arr_size(const json* j);
int json_arr_empty(const json* j);
void json_arr_clear(json* j);
json* json_arr_get(json* j, size_t pos);
void json_arr_set(json* j, size_t pos, json* value);
void json_arr_insert(json* j, size_t pos, json* value);
void json_arr_insert_front(json* j, json* value);
void json_arr_insert_back(json* j, json* value);
void json_arr_remove(json* j, size_t pos, size_t n);
void json_arr_remove_front(json* j, size_t n);
void json_arr_remove_back(json* j, size_t n);

size_t json_map_size(const json* j);
int json_map_empty(const json* j);
void json_map_clear(json* j);
int json_map_contains(const json* j, const char* key);
json* json_map_get(json* j, const char* key);
void json_map_set(json* j, const char* key, json* value);
void json_map_remove(json* j, const char* key);


const char* json_typename(const json* j);

size_t json_write(ioWrite_f writter, const json* j, size_t spaces);
json* json_read(ioRead_f reader);



#endif //!JSON_H_
