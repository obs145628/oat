/*
 * @file function to manipulates files systems
 */

#ifndef FS_H_
# define FS_H_

# include <stddef.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
# include "slist.h"

typedef struct stat FsStat;


/**
 * Allocates a byte array cotaining all bytes in the files
 * @param {const char*} path - file path
 * @param {size_t*} size - if not null, will contain the array size
 * @return {char*}
 */
char* fsReadAllBytes(const char* path, size_t* size);

/**
 * Allocates a string cotaining all text in the file
 * @param {const char*} path - file path
 * @param {size_t*} size - if not null, will contain the string length
 * @return {char*}
 */
char* fsReadAllText(const char* path, size_t* size);

/**
 * Write bytes to a file from a char* array
 * Erase the previous file content
 * @param path - string containing the file path
 * @param data - array containing the bytes to write
 * @param len - length of the array
 */
void fsWriteBytes(const char* path, const char* data, size_t len);

/**
 * Write char array to a file
 * Erase the previous file content
 * @param path - string containing the file path
 * @param content - file content
 */
void fsWriteText(const char* path, const char* content);

/**
 * Write bytes at the end of a file from a char* array
 * @param path - string containing the file path
 * @param data - array containing the bytes to write
 * @param len - length of the array
 */
void fsAppendBytes(const char* path, const char* data, size_t len);

/**
 * Write char array at the end of a file
 * @param path - string containing the file path
 * @param content - file content
 */
void fsAppendText(const char* path, const char* content);

/**
 * Copy the file content of src to dst
 * If dst already exists, its previous content is erased
 * @param src - path of the source file
 * @param dst - path of the destination file
 */
void fsCopy(const char* src, const char* dst);

/**
 * Move a file or a folder from src to dst
 * @param src - path of the source file / dir
 * @param dst- path of destination file / dir
 */
void fsMove(const char* src, const char* dst);

/**
 * Remove a file or an empty folder
 * @param path - path of the file / dir
 */
void fsRemove(const char* path);

/**
 * Returns a struct stat value of a file / dir
 * @param path - path of the file / dir
 */
FsStat fsStat(const char* path);


int fsExists(const char* path);

int fsIsFile(const char* path);

int fsIsDir(const char* path);

/**
 * Allocates and returns a SList containing names of all files in a folder
 * Ignore '.' and '..'
 */
SList* fsReadDir(const char* path);

/**
 * Creates a new directory with specified mode
 * @param path - full path of the new directory
 * @param mode - directory mode
 */
void fsMakeDir(const char* path, int mode);

/**
 * Removes a folder
 * If not empty, it's content is removed too
 * @param path -  dir path
 */
void fsRemoveDir(const char* path);

/**
 * Copy a directory from src to dst
 * @param src - path of the source dictory
 * @param dst - path of the destination directory
 */
void fsCopyDir(const char* src, const char* dst);



/**
 * Allocates a new string containing the home directory path
 */
char* fsHomePath();


#endif //!FS_H_
