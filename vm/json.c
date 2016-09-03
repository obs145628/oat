#include "json.h"
#include <stdlib.h>
#include <string.h>
#include "const.h"
#include "json.h"
#include "str.h"
#include "err.h"
#include "numbers.h"
#include "dstr.h"

static const char* type_to_str(short type)
{
   static const char* types[] = {"null", "bool", "int", "real",
                                 "string", "array", "map"};
   return types[(size_t) type];
}

static void check_type(const json* j, short type)
{
   if(j->type != type)
      err("json type is %s, but %s was excepted",
          json_typename(j), type_to_str(type));
}

static void pair_free(char* key, json* value)
{
   free(key);
   json_free(value);
}

static void json_destruct(json* j)
{
   if(j->type == JSON_STRING)
      free(j->j_str);

   else if(j->type == JSON_ARRAY)
   {
      parray_foreach(j->j_arr, (f_parray_foreach) json_free);
      parray_free(j->j_arr);
   }

   else if(j->type == JSON_MAP)
   {
      pmap_foreach(j->j_map, (f_pmap_foreach) pair_free);
      pmap_free(j->j_map);
   }
}

static size_t write_string(ioWrite_f writter, const char* str)
{
   size_t len = 0;
   char c;
   len += ioWriteChar(writter, '"');

   while((c = *(str++)) != '\0')
   {
      if(c == '"')
         len += ioWriteBytes(writter, "\\\"", 2);
      else if(c == '\n')
         len += ioWriteBytes(writter, "\\n", 2);
      else if(c == '\t')
         len += ioWriteBytes(writter, "\\t", 2);
      else
         len += ioWriteChar(writter, c);
   }

   len += ioWriteChar(writter, '"');
   return len;
}

static size_t write_spaces(ioWrite_f writter, size_t len)
{
   char c = ' ';
   for(size_t i = 0; i < len; ++i)
      ioWriteBytes(writter, &c, 1);
   return len;
}

static int is_json_blanck(char c)
{
   return c == ' ' || c == '\t' || c  =='\n';
}

static void read_error(size_t* row, size_t* col)
{
   err("Unable to parse JSON text: unexpected token (line %zu, column %zu)",
       *row, *col - 1);
}

static char read_char(ioRead_f reader, size_t* row, size_t* col, int* end)
{
   if(end)
      *end = 0;
   char c;
   ++(*col);
   if(reader(&c, 1) != 1)
   {
      if(end)
      {
         *end = 1;
         return 0;
      }
      read_error(row, col);
   }

   if(c == '\n')
   {
      ++(*row);
      *col = 2;
   }
   return c;
}

static char read_nonblanck_char(ioRead_f reader, size_t* row, size_t* col)
{
   char c;
   do
   {
      c = read_char(reader, row, col, NULL);
   } while(is_json_blanck(c));
   return c;
}

static void test_read(ioRead_f reader, const char* str,
                      size_t* row, size_t* col)
{
   while(*str != '\0')
      if(read_char(reader, row, col, NULL) != *(str++))
         read_error(row, col);
}

static char* read_string(ioRead_f reader, size_t* row, size_t* col)
{
   dstr ds;
   dstr_init(&ds);
   char c;
   int escaped = 0;

   while((c = read_char(reader, row, col, NULL)) != '"' || escaped)
   {
      if(c == '\n')
         read_error(row, col);

      if(!escaped)
      {
         if(c == '\\')
            escaped = 1;
         else
            dstr_append(&ds, &c, 1);
      }

      else
      {
         char special;
         if(c == 't')
            special = '\t';
         else if(c == 'n')
            special = '\n';
         else if(c == '"')
            special = '"';
         else if(c == '\\')
            special = '\\';
         else
            read_error(row, col);
         dstr_append(&ds, &special, 1);
         escaped = 0;
      }
   }

   return ds.s;
}

json* json_new_null()
{
   json* j = malloc(sizeof(json));
   j->type = JSON_NULL;
   return j;
}

json* json_new_bool(int x)
{
   json* j = malloc(sizeof(json));
   j->type = JSON_BOOL;
   j->j_bool = x;
   return j;
}

json* json_new_int(int64_t x)
{
   json* j = malloc(sizeof(json));
   j->type = JSON_INT;
   j->j_int = x;
   return j;
}

json* json_new_real(double x)
{
   json* j = malloc(sizeof(json));
   j->type = JSON_REAL;
   j->j_real = x;
   return j;
}

json* json_new_str(const char* x)
{
   json* j = malloc(sizeof(json));
   j->type = JSON_STRING;
   j->j_str = strClone(x);
   return j;
}

json* json_new_arr()
{
   json* j = malloc(sizeof(json));
   j->type = JSON_ARRAY;
   j->j_arr = parray_new();
   return j;
}

json* json_new_map()
{
   json* j = malloc(sizeof(json));
   j->type = JSON_MAP;
   j->j_map = pmap_new((f_pmap_cmp) strcmp);
   return j;
}

json* json_clone(const json* j)
{
   json* clone = malloc(sizeof(json));
   clone->type = j->type;

   if(j->type == JSON_BOOL)
      clone->j_bool = j->j_bool;
   else if(j->type == JSON_INT)
      clone->j_int = j->j_int;
   else if(j->type == JSON_REAL)
      clone->j_real = JSON_REAL;
   else if(j->type == JSON_STRING)
      clone->j_str = strClone(j->j_str);

   else if(j->type == JSON_ARRAY)
   {
      clone->j_arr = parray_new();
      for(void** it = parray_begin(j->j_arr); it != parray_end(j->j_arr); ++it)
         parray_insert_back(clone->j_arr, json_clone(*it));
   }

   else if(j->type == JSON_MAP)
   {
      clone->j_map = pmap_new((f_pmap_cmp) strcmp);
      for(s_pmap_node* it = pmap_begin(j->j_map); it != pmap_end(j->j_map);
          it = pmap_next(j->j_map, it))
         pmap_set(clone->j_map, strClone(it->key), json_clone(it->value));
   }

   return clone;
}

void json_free(json* j)
{
   json_destruct(j);
   free(j);
}

int json_equals(const json* a, const json* b)
{
   if(a == b)
      return 1;

   if(a->type != b->type)
      return 0;

   if(a->type == JSON_NULL)
      return 1;

   if(a->type == JSON_BOOL)
      return (!a->j_bool) == (!b->j_bool);

   if(a->type == JSON_INT)
      return a->j_int == b->j_int;

   if(a->type == JSON_REAL)
      return a->j_real == b->j_real;

   if(a->type == JSON_STRING)
      return !strcmp(a->j_str, b->j_str);

   if(a->type == JSON_ARRAY)
   {
      if(a->j_arr->_size != b->j_arr->_size)
         return 0;

      for(size_t i = 0; i < a->j_arr->_size; ++i)
         if(!json_equals(parray_get(a->j_arr, i), parray_get(b->j_arr, i)))
            return 0;
      return 1;
   }

   if(a->type == JSON_MAP)
   {
      for(s_pmap_node* it = pmap_begin(a->j_map); it != pmap_end(a->j_map);
          it = pmap_next(a->j_map, it))
      {
         if(!pmap_contains(b->j_map, it->key) ||
            !json_equals(pmap_get(a->j_map, it->key),
                         pmap_get(b->j_map, it->key)))
            return 0;
      }

      for(s_pmap_node* it = pmap_begin(b->j_map); it != pmap_end(b->j_map);
          it = pmap_next(b->j_map, it))
      {
         if(!pmap_contains(a->j_map, it->key))
            return 0;
      }

      return 1;
   }

   return 0;
}

void json_assign_null(json* j)
{
   json_destruct(j);
   j->type = JSON_NULL;
}

void json_assign_bool(json* j, int x)
{
   json_destruct(j);
   j->type = JSON_BOOL;
   j->j_bool = x;
}

void json_assign_int(json* j, int64_t x)
{
   json_destruct(j);
   j->type = JSON_INT;
   j->j_int = x;
}

void json_assign_real(json* j, double x)
{
   json_destruct(j);
   j->type = JSON_REAL;
   j->j_real = x;
}

void json_assign_str(json* j, const char* x)
{
   json_destruct(j);
   j->type = JSON_STRING;
   j->j_str =  strClone(x);
}

void json_assign_arr(json* j)
{
   json_destruct(j);
   j->type = JSON_ARRAY;
   j->j_arr = parray_new();
}

void json_assign_map(json* j)
{
   json_destruct(j);
   j->type = JSON_MAP;
   j->j_map = pmap_new((f_pmap_cmp) strcmp);
}

int json_is_null(const json* j)
{
   return j->type == JSON_NULL;
}

int json_is_bool(const json* j)
{
   return j->type == JSON_BOOL;
}

int json_is_int(const json* j)
{
   return j->type == JSON_INT;
}

int json_is_real(const json* j)
{
   return j->type == JSON_REAL;
}

int json_is_str(const json* j)
{
   return j->type == JSON_STRING;
}

int json_is_arr(const json* j)
{
   return j->type == JSON_ARRAY;
}

int json_is_map(const json* j)
{
   return j->type == JSON_MAP;
}

int json_get_bool(const json* j)
{
   check_type(j, JSON_BOOL);
   return j->j_bool;
}

int64_t json_get_int(const json* j)
{
   check_type(j, JSON_INT);
   return j->j_int;
}

double json_get_real(const json* j)
{
   check_type(j, JSON_REAL);
   return j->j_real;
}

char* json_get_str(const json* j)
{
   check_type(j, JSON_STRING);
   return j->j_str;
}

s_parray* json_get_arr(const json* j)
{
   check_type(j, JSON_ARRAY);
   return j->j_arr;
}

s_pmap* json_get_map(const json* j)
{
   check_type(j, JSON_MAP);
   return j->j_map;
}

size_t json_arr_size(const json* j)
{
   check_type(j, JSON_ARRAY);
   return parray_size(j->j_arr);
}

int json_arr_empty(const json* j)
{
   check_type(j, JSON_ARRAY);
   return parray_empty(j->j_arr);
}

void json_arr_clear(json* j)
{
   check_type(j, JSON_ARRAY);
   json_arr_remove(j, 0, j->j_arr->_size);
}

json* json_arr_get(json* j, size_t pos)
{
   check_type(j, JSON_ARRAY);
   if(pos >= j->j_arr->_size)
      err("Unable to acces %zu element of json array of size %zu",
          pos, j->j_arr->_size);
   return parray_get(j->j_arr, pos);
}

void json_arr_set(json*j, size_t pos, json* value)
{
   check_type(j, JSON_ARRAY);
   if(pos >= j->j_arr->_size)
      err("Unable to acces %zu element of json array of size %zu",
          pos, j->j_arr->_size);
   parray_set(j->j_arr, pos, value);
}

void json_arr_insert(json* j, size_t pos, json* value)
{
   check_type(j, JSON_ARRAY);
   if(pos > j->j_arr->_size)
      err("Unable to insert element at %zu in a json array of size %zu",
          pos, j->j_arr->_size);
   parray_insert(j->j_arr, pos, value);
}

void json_arr_insert_front(json* j, json* value)
{
   check_type(j, JSON_ARRAY);
   parray_insert_front(j->j_arr, value);
}

void json_arr_insert_back(json* j, json* value)
{
   check_type(j, JSON_ARRAY);
   parray_insert_back(j->j_arr, value);
}

void json_arr_remove(json* j, size_t pos, size_t n)
{
   check_type(j, JSON_ARRAY);
   if(n && pos + n > j->j_arr->_size)
      err("Unable to remove %zu elements at pos %zu in json array of size %zu",
          n, pos, j->j_arr->_size);

   for(size_t i = pos; i < pos + n; ++i)
      json_free(parray_get(j->j_arr, i));
   parray_remove(j->j_arr, pos, n);
}

void json_arr_remove_front(json* j, size_t n)
{
   check_type(j, JSON_ARRAY);
   if(n > j->j_arr->_size)
      err("Unable to remove %zu elements at front of a json array of size %zu",
          n, j->j_arr->_size);
   json_arr_remove(j, 0, n);
}

void json_arr_remove_back(json* j, size_t n)
{
   check_type(j, JSON_ARRAY);
   if(n > j->j_arr->_size)
      err("Unable to remove %zu elements at back of a json array of size %zu",
          n, j->j_arr->_size);
   json_arr_remove(j, j->j_arr->_size - n, n);
}

size_t json_map_size(const json* j)
{
   check_type(j, JSON_MAP);
   return pmap_size(j->j_map);
}

int json_map_empty(const json* j)
{
   check_type(j, JSON_MAP);
   return pmap_empty(j->j_map);
}

void json_map_clear(json* j)
{
   check_type(j, JSON_MAP);
   pmap_foreach(j->j_map, (f_pmap_foreach) pair_free);
   pmap_clear(j->j_map);
}

int json_map_contains(const json* j, const char* key)
{
   check_type(j, JSON_MAP);
   return pmap_contains(j->j_map, (void*) key);
}

json* json_map_get(json* j, const char* key)
{
   check_type(j, JSON_MAP);
   s_pmap_node* it = pmap_find(j->j_map, (void*) key);
   if(it == pmap_end(j->j_map))
      err("Unable to get element of json map: key %s not found", key);
   return it->value;
}

void json_map_set(json* j, const char* key, json* value)
{
   check_type(j, JSON_MAP);
   pmap_set(j->j_map, strClone(key), value);
}

void json_map_remove(json* j, const char* key)
{
   check_type(j, JSON_MAP);
   s_pmap_node* it = pmap_find(j->j_map, (void*) key);
   if(it == pmap_end(j->j_map))
      err("Unable to remove element of json map: key %s not found", key);
   pair_free(it->key, it->value);
   pmap_remove(j->j_map, (void*) key);
}

const char* json_typename(const json* j)
{
   return type_to_str(j->type);
}

static size_t json_write_rec(ioWrite_f writter, const json* j, size_t spaces,
                             size_t dec)
{
   if(j->type == JSON_NULL)
   {
      return ioWriteBytes(writter, "null", 4);
   }

   else if(j->type == JSON_BOOL)
   {
      if(j->j_bool)
         return ioWriteBytes(writter, "true", 4);
      else
         return ioWriteBytes(writter, "false", 5);
   }

   else if(j->type == JSON_INT)
   {
      return ioWriteInt(writter, (long) j->j_int);
   }

   else if(j->type == JSON_REAL)
   {
      return ioWriteFloat(writter, (double) j->j_real);
   }

   else if(j->type == JSON_STRING)
   {
      return write_string(writter, j->j_str);
   }

   else if(j->type == JSON_ARRAY)
   {
      s_parray* arr = j->j_arr;
      size_t len = 0;
      size_t n = parray_size(arr);
      if(!n)
         return ioWriteBytes(writter, "[]", 2);

      dec += spaces;
      len += ioWriteChar(writter, '[');
      if(spaces)
         len += ioWriteChar(writter, '\n');

      for(size_t i = 0; i < n; ++i)
      {
         json* item = parray_get(arr, i);
         if(spaces)
            len += write_spaces(writter, dec);
         len += json_write_rec(writter, item, spaces, dec);
         if(i + 1 != n)
            len += ioWriteChar(writter, ',');
         if(spaces)
            len += ioWriteChar(writter, '\n');
      }

      if(spaces)
         len += write_spaces(writter, dec - spaces);
      len += ioWriteChar(writter, ']');
      return len;
   }

   else if(j->type == JSON_MAP)
   {
      s_pmap* map = j->j_map;
      size_t len = 0;
      if(pmap_empty(map))
         return ioWriteBytes(writter, "{}", 2);

      dec += spaces;
      len += ioWriteChar(writter, '{');
      if(spaces)
         len += ioWriteChar(writter, '\n');

      s_pmap_node* it = pmap_begin(map);
      while(1)
      {
         const char* key = it->key;
         json* item = it->value;
         if(spaces)
            len += write_spaces(writter, dec);

         len += write_string(writter, key);
         len += ioWriteBytes(writter, ": ", 2);
         len += json_write_rec(writter, item, spaces, dec);

         it = pmap_next(map, it);
         if(it == pmap_end(map))
         {
            if(spaces)
               len += ioWriteChar(writter, '\n');
            break;
         }


         len += ioWriteChar(writter, ',');
         if(spaces)
            len += ioWriteChar(writter, '\n');
      }

      if(spaces)
         len += write_spaces(writter, dec - spaces);
      len += ioWriteChar(writter, '}');
      return len;
   }

   return 0;
}

size_t json_write(ioWrite_f writter, const json* j, size_t spaces)
{
   return json_write_rec(writter, j, spaces, 0);
}

static json* json_read_rec(ioRead_f reader, size_t* row, size_t* col,
                           char* next)
{

   char c = read_nonblanck_char(reader, row, col);

   if(c == ']' || c == '}')
   {
      if(next)
         *next = c;
      else
         read_error(row, col);
      return NULL;
   }

   if(c == 'n')
   {
      test_read(reader, "ull", row, col);
      return json_new_null();
   }

   if(c == 't')
   {
      test_read(reader, "rue", row, col);
      return json_new_bool(1);
   }

   if(c == 'f')
   {
      test_read(reader, "alse", row, col);
      return json_new_bool(0);
   }

   if(c == '-' || (c >= '0' && c <= '9'))
   {
      char buffer[1024];
      *buffer = c;
      size_t len = 1;
      int end;
      int dot = 0;
      char c;

      while(1)
      {
         c = read_char(reader, row, col, &end);
         if(end)
            break;
         if(c == '.')
         {
            if(dot)
               break;
            dot = 1;
         }

         else if(c < '0' || c > '9')
            break;

         buffer[len++] = c;
      }

      buffer[len] = '\0';
      int ok;
      json* j = dot ? json_new_real(strToDouble(buffer, &ok))
         : json_new_int(strToLong(buffer, &ok));
      if(!ok)
         read_error(row, col);

      if(next)
         *next = c;
      return j;
   }

   if(c == '"')
   {
      char* str = read_string(reader, row, col);
      json* j = malloc(sizeof(json));
      j->type = JSON_STRING;
      j->j_str = str;
      return j;
   }

   if(c == '[')
   {
      json* j = json_new_arr();
      int empty = 1;

      while(1)
      {
         char next;
         json* item = json_read_rec(reader, row, col, &next);
         if(!item)
         {
            if(!empty || next != ']')
               read_error(row, col);
            break;
         }
         json_arr_insert_back(j, item);
         empty = 0;

         if(item->type == JSON_INT || item->type == JSON_REAL)
         {
            if(is_json_blanck(next))
               next = read_nonblanck_char(reader, row, col);
         }
         else
         {
            next = read_nonblanck_char(reader, row, col);
         }

         if(next == ',')
            continue;
         else if(next == ']')
            break;
         else
            read_error(row, col);
      }

      return j;
   }

   if(c == '{')
   {
      json* j = json_new_map();
      int empty = 1;

      while(1)
      {
         char c = read_nonblanck_char(reader, row, col);
         if(c == '}')
         {
            if(!empty)
               read_error(row, col);
            break;
         }
         if(c != '"')
            read_error(row, col);

         char* key = read_string(reader, row, col);
         if(read_nonblanck_char(reader, row, col) != ':')
            read_error(row, col);

         char next;
         json* item = json_read_rec(reader, row, col, &next);
         if(!item)
            read_error(row, col);

         json_map_set(j, key, item);
         empty = 0;

         if(item->type == JSON_INT || item->type == JSON_REAL)
         {
            if(is_json_blanck(next))
               next = read_nonblanck_char(reader, row, col);
         }
         else
         {
            next = read_nonblanck_char(reader, row, col);
         }

         if(next == ',')
            continue;
         else if(next == '}')
            break;
         else
            read_error(row, col);
      }

      return j;

   }

   read_error(row, col);
   return 0;
}

json* json_read(ioRead_f reader)
{
   size_t row = 1;
   size_t col = 1;
   return json_read_rec(reader, &row, &col, NULL);
}
