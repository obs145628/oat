#ifndef VM_BIN_H_
# define VM_BIN_H_

# include "const.h"

void vm_bin_load_file(const char* binPath);
char* vm_bin_buffer_begin();
char* vm_bin_buffer_end();
char* vm_bin_buffer_pc();
size_t vm_bin_buffer_size();
void vm_bin_buffer_read(char* dst, size_t len);
void vm_bin_buffer_set_pc(char* pc);
void vm_bin_buffer_move_pc(long dp);


#endif //!VM_BIN_H_
