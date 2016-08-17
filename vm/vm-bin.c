#include "vm-bin.h"
#include <string.h>
#include <assert.h>
#include "fs.h"

#include <stdio.h>

static const char* buffer_begin_ = NULL;
static const char* buffer_end_ = NULL;
static const char* buffer_pc_ = NULL;
static size_t buffer_size_;

void vm_bin_load_file(const char* binPath)
{
   assert(!buffer_begin_);
   buffer_begin_ = fsReadAllBytes(binPath, &buffer_size_);

   buffer_end_ = buffer_begin_ + buffer_size_;
   buffer_pc_ = buffer_begin_;
}

const char* vm_bin_buffer_begin()
{
   assert(buffer_begin_);
   return buffer_begin_;
}

const char* vm_bin_buffer_end()
{
   assert(buffer_end_);
   return buffer_end_;
}

const char* vm_bin_buffer_pc()
{
   assert(buffer_pc_);
   return buffer_pc_;
}

size_t vm_bin_buffer_size()
{
   assert(buffer_begin_);
   return buffer_size_;
}

void vm_bin_buffer_read(char* dst, size_t len)
{
   assert(buffer_pc_);
   assert(buffer_pc_ + len <= buffer_end_);
   memcpy(dst, buffer_pc_, len);
}

void vm_bin_buffer_set_pc(const char* pc)
{
   assert(buffer_pc_);
   assert(pc >= buffer_begin_ && pc <= buffer_end_);
   buffer_pc_ = pc;
}

void vm_bin_buffer_move_pc(long dp)
{
   vm_bin_buffer_set_pc(buffer_pc_ + dp);
}
