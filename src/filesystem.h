#if defined(FS_IMPL) && !defined(FS_IMPLEMENTATION)
#define FS_IMPLEMENTATION
#endif
#ifndef FS_INCLUDED
/*
    filesystem.h -- a *tiny* library for interfacing with a filesystem.

    Project URL: https://github.com/mathewmariani/filesystem

    Do this:
      #define FS_IMPL or
      #define FS_IMPLEMENTATION

    before you include this file in *one* C or C++ file to create the
    implementation.

    ...optionally you can provide the following macros to override defaults:

    FS_ASSERT(c)     - your own assert function (default: assert(c))
    FS_MALLOC(s)     - your own malloc function (default: malloc(s))
    FS_FREE(p)       - your own free function (default: free(p))


    FEATURE OVERVIEW:
    =================

    filesystem.h provides a minimalistic API which implements
    the basics functions for interfacing with a filesystem.

    - file reading, appending, and writing
    - creating and deleting files and directories
    - retrieving information on files
    - write directory to only allow writes in specific directory
    - search path for searching multiple directories


    FUNCTIONS:
    ==========

    fs_setup(const fs_desc* desc)
    fs_shutdown(void)
    fs_is_valid(void)

    fs_append(const char* name, const fs_data* data)
    fs_delete(const char* name)
    fs_exists(const char* path)
    fs_free(void* p)
    fs_get_cwd()
    fs_get_info(const char* path, fs_info* info)
    fs_insert_basepath(const char* path)
    fs_mkdir(const char* path)
    fs_read(const char* name, size_t* size)
    fs_remove_basepath(const char* path)
    fs_write(const char* name, const fs_data* data)


    STEP BY STEP:
    =============

    --- to initialize and cleanup filesystem.h, call:

            fs_setup(const fs_desc* desc)
            fs_shutdown()

    --- to add/remove a directory from the search path, call:

            fs_insert_basepath(const char* path)
            fs_remove_basepath(const char* path)
    
    --- to check if a file exists, call:

            fs_exists(const char* path)

    --- to write data to a file, call:

            fs_append(const char* name, fs_data* data)
            fs_write(const char* name, fs_data* data)

    --- to read data from a file, call:

            fs_read(const char* name, size_t* size);

    --- to get information about a file or directory, call:

            fs_get_info(const char* path, fs_info* info)

    --- to create a directory or directory tree, call:

            fs_mkdir(const char* path)

    --- to delete a file or directory, call:

            fs_delete(const char* name)

    --- to get the current working directory, call:

            fs_get_cwd()


    READING FROM A FILE:
    ====================

    --- When reading from a file, the file is opened, memory is allocated,
        copied, and the file is closed before the function returns a pointer
        to the data read.

        Ownership of the pointer is that of the user, and the pointer must
        freed after use to avoid memory leaks.


        size_t size;
        const char* data = (char*) fs_read("example.txt", &size);

        fs_free(data);


    WRITTING TO A FILE:
    ===================

    --- When writting, or appending, to a file, the file is closed after
        writting has completed, or failed.

        If the file already exists, it will be completely replaced by the new contents.
        However, if the file doesn't exist a new one will be created.


        const char* text = "the quick brown fox jumps over the lazy dog";
        if (!fs_write("example.txt", FS_DATA_STR_REF(text))) {
          return -1;
        }


    LICENSE:
    ========

    MIT License

    Copyright (c) 2022 Mat Mariani

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/
#define FS_INCLUDED (1)

#include <stdbool.h>
#include <stddef.h> /* size_t */

#if !defined (FS_API_DECL)
  #define FS_API_DECL extern
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct fs_data {
  const void* data;
  size_t size;
} fs_data;

#if defined(__cplusplus)
  #define FS_DATA_STR(x) fs_data { &x, strlen(x) }
  #define FS_DATA_STR_REF(x) fs_data { &x, strlen(x) }
#else
  #define FS_DATA_STR(x) (fs_data) { x, strlen(x) }
  #define FS_DATA_STR_REF(x) &(fs_data) { x, strlen(x) }
#endif

/* compile-time constants */
enum {
  FS_MAX_PATH = 256,
  FS_MAX_MOUNTS = 3,
};

typedef enum fs_file_type {
  FS_FILETYPE_NONE,
  FS_FILETYPE_REG,
  FS_FILETYPE_DIR,
  FS_FILETYPE_SYM,
} fs_file_type;

typedef struct fs_info {
  fs_file_type type;
  size_t size;
  long int modtime;
} fs_info;

typedef struct fs_desc {
  const char* write_dir;
  const char* base_paths[3];
} fs_desc;

/* setup filesystem */
FS_API_DECL void fs_setup(const fs_desc* desc);
/* shutdown filesystem */
FS_API_DECL void fs_shutdown(void);
/* true between `fs_setup()` and `fs_shutdown()` */
FS_API_DECL bool fs_is_valid(void);
/* adds a directory to the search path */
FS_API_DECL bool fs_insert_basepath(const char* path);
/* removes a directory to the search path */
FS_API_DECL bool fs_remove_basepath(const char* path);
/* return true if a file or a directory exists */
FS_API_DECL bool fs_exists(const char* path);
/* reads the contents of a file */
FS_API_DECL void* fs_read(const char* name, size_t* size);
/* writes data to a file */
FS_API_DECL bool fs_write(const char* name, const fs_data* data);
/* writes data to the end of a file */
FS_API_DECL bool fs_append(const char* name, const fs_data* data);
/* gets information about the specified file or directory */
FS_API_DECL bool fs_get_info(const char* path, fs_info* info);
/* gets the current working directory */
FS_API_DECL const char* fs_get_cwd();
/* creates a directory */
FS_API_DECL bool fs_mkdir(const char* path);
/* deletes a file or directory */
FS_API_DECL bool fs_delete(const char* name);
/* frees allocated memory */
FS_API_DECL inline void fs_free(void* p);

#ifdef __cplusplus
}

/* reference-based equivalents for c++ */
inline void fs_setup(const fs_desc& data) { return fs_setup(&data); }
inline bool fs_get_info(const char* filename, fs_info &info) { return fs_get_info(filename, &info); }
inline bool fs_write(const char* name, fs_data &data) { return fs_write(name, &data); }
inline bool fs_append(const char* name, fs_data &data) { return fs_append(name, &data); }

#endif

#endif /* FS_INCLUDED */

#ifdef FS_IMPLEMENTATION
#define FS_IMPL_INCLUDED (1)

#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>

#if defined (_WIN32)
  #ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
  #endif
  #ifndef NOMINMAX
  #define NOMINMAX
  #endif
  #include <windows.h>
  #include <direct.h>
  #define _getcwd getcwd
#else
  #include <sys/param.h>
  #include <unistd.h>
#endif

#ifndef _FS_PRIVATE
  #if defined(__GNUC__) || defined(__clang__)
    #define _FS_PRIVATE __attribute__((unused)) static
  #else
    #define _FS_PRIVATE static
  #endif
#endif

#if !defined (FS_ASSERT)
  #include <assert.h>
  #define FS_ASSERT(c) assert(c)
#endif

#if !defined (FS_MALLOC)
  #include <stdlib.h>
  #define FS_MALLOC(s) malloc(s)
  #define FS_FREE(p)   free(p)
#endif

enum {
  _FS_MREAD,
  _FS_MWRITE,
  _FS_MAPPEND,
};

typedef struct {
  char buf[FS_MAX_PATH];
} _fs_path;

typedef struct {
  int count;
  _fs_path base_path[FS_MAX_PATH];
  _fs_path write_dir;
  char cwd[FS_MAX_PATH];
  bool valid;
} _fs_state_t;
static _fs_state_t _fs;

/* private implementation functions */

_FS_PRIVATE inline bool _fs_strempty(const _fs_path* str) {
  return 0 == str->buf[0];
}

_FS_PRIVATE void _fs_strcpy(_fs_path* dst, const char* src) {
  if (src) {
    strncpy(dst->buf, src, FS_MAX_PATH);
    dst->buf[FS_MAX_PATH-1] = 0;
  } else {
    memset(dst->buf, 0, FS_MAX_PATH);
  }
}

_FS_PRIVATE bool _fs_concat_path(char* dst, const _fs_path* dir, const char* filename) {
  const size_t dirlen = strlen(dir->buf);
  if (dirlen + strlen(filename) + 2 > FS_MAX_PATH) {
    return false;
  }
  const char* str = (dir->buf[dirlen - 1] == '/') ? "%s%s" : "%s/%s";
  sprintf(dst, str, dir->buf, filename);
  return true;
}

_FS_PRIVATE bool _fs_get_file_info(const char* filename, fs_info* info) {
  struct stat fstat;
  if (stat(filename, &fstat) != 0) {
    return false;
  }
  if (info != NULL) {
    info->size = fstat.st_size;
    info->modtime = fstat.st_mtime;
    if (S_ISREG(fstat.st_mode)) {
      info->type = FS_FILETYPE_REG;
    } else if (S_ISDIR(fstat.st_mode)) {
      info->type = FS_FILETYPE_DIR;
    } else if (S_ISLNK(fstat.st_mode)) {
      info->type = FS_FILETYPE_SYM;
    } else {
      info->type = FS_FILETYPE_NONE;
    }
  }
  return true;
}

_FS_PRIVATE bool _fs_native_delete(const char* filename) {
  return remove(filename) == 0;
}

_FS_PRIVATE bool _fs_native_mkdir(const char* path) {
  char buf[FS_MAX_PATH];
  if (strlen(path) > FS_MAX_PATH - 1) {
    return false; 
  }
  strcpy(buf, path);
  for (char* p = buf + 1; *p; p++) {
    if (*p == '/') {
      *p = '\0';
      if (mkdir(buf, S_IRWXU) != 0 && errno != EEXIST) {
        return false;
      }
      *p = '/';
    }
  }
  if (mkdir(buf, S_IRWXU) != 0 && errno != EEXIST) {
    return false;
  }   
  return true;
}

_FS_PRIVATE FILE* _fs_native_open(const char* filename, int mode) {
  if ((mode == _FS_MREAD) && !_fs_get_file_info(filename, NULL)) {
    return NULL;
  }
  if ((mode == _FS_MAPPEND || mode == _FS_MWRITE) && _fs_strempty(&_fs.write_dir)) {
    return NULL;
  }
  FILE* fp = NULL;
  switch (mode) {
  case _FS_MREAD: fp = fopen(filename, "rb"); break;
  case _FS_MAPPEND: fp = fopen(filename, "a"); break;
  case _FS_MWRITE: fp = fopen(filename, "wb"); break;
  }
  return (!fp) ? NULL : fp;
}

_FS_PRIVATE void* _fs_native_read(FILE* fp, size_t* size) {
  if (fp == NULL) {
    return NULL;
  }
  fseek(fp, 0, SEEK_END);
  *size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  void* buf = FS_MALLOC(*size);
  if (!buf) {
    return NULL;
  }
  fread(buf, 1, *size, fp);
  fclose(fp);
  return buf;
}

_FS_PRIVATE int _fs_native_write(FILE* fp, const fs_data* data) {
  if (fp == NULL) {
    return false;
  }
  size_t size = fwrite((char*)(data->data), sizeof(char), data->size, fp);
  fclose(fp);
  return size == data->size;
}

/* public api functions */

void fs_setup(const fs_desc* desc) {
  FS_ASSERT(desc);
  _fs_strcpy(&_fs.write_dir, desc->write_dir);
  for (int i = 0; i < FS_MAX_MOUNTS; i++) {
    if (desc->base_paths[i]) {
      _fs_strcpy(&_fs.base_path[i], desc->base_paths[i]);
      _fs.count++;  
    }
  }
  /* always last */
  _fs.valid = true;
}

void fs_shutdown(void) {
  FS_ASSERT(_fs.valid);
  _fs.valid = false;
}

bool fs_is_valid(void) {
  return _fs.valid;
}

bool fs_insert_basepath(const char* path) {
  FS_ASSERT(path);
  if (strlen(path) >= FS_MAX_PATH || _fs.count >= FS_MAX_MOUNTS) {
    return false;
  }
  _fs_path* dir = &_fs.base_path[_fs.count - 1];
  for (; dir >= _fs.base_path; dir--) {
    if (strcmp(dir->buf, path) == 0) {
      return false;
    }
  }
  dir = &_fs.base_path[_fs.count++];
  strcpy(dir->buf, path);
  return true;
}

bool fs_remove_basepath(const char* path) {
  FS_ASSERT(path);
  _fs_path* dir = &_fs.base_path[_fs.count - 1];
  for (; dir >= _fs.base_path; dir--) {
    if (strcmp(dir->buf, path) == 0) {
      int idx = dir - _fs.base_path;
      memmove(dir, dir + 1, (_fs.count - idx - 1) * sizeof(_fs_path));
      memset(&_fs.base_path[_fs.count - 1].buf, 0, FS_MAX_PATH);
      _fs.count--;
      return true;
    }
  }
  return false;
}

bool fs_exists(const char* filename) {
  FS_ASSERT(filename);
  char buf[FS_MAX_PATH];
  _fs_path* dir = &_fs.base_path[_fs.count - 1];
  for (; dir >= _fs.base_path; dir--) {
    if (_fs_concat_path(buf, dir, filename) && _fs_get_file_info(buf, NULL)) {
      return true;
    }
  }
  return false;
}

void* fs_read(const char* name, size_t* size) {
  FS_ASSERT(name && size);
  char buf[FS_MAX_PATH];
  _fs_path* dir = &_fs.base_path[_fs.count - 1];
  for (; dir >= _fs.base_path; dir--) {
    if (!_fs_concat_path(buf, dir, name)) {
      continue;
    }
    FILE* fp = _fs_native_open(buf, _FS_MREAD);
    return _fs_native_read(fp, size);
  }
  return NULL;
}

bool fs_write(const char* name, const fs_data* data) {
  FS_ASSERT(name && data);
  if (_fs_strempty(&_fs.write_dir)) {
    return false;
  }
  char buf[FS_MAX_PATH];
  if (!_fs_concat_path(buf, &_fs.write_dir, name)) {
    return false;
  }
  FILE* fp = _fs_native_open(buf, _FS_MWRITE);
  return _fs_native_write(fp, data);
}

bool fs_append(const char* name, const fs_data* data) {
  FS_ASSERT(name && data);
  if (_fs_strempty(&_fs.write_dir)) {
    return false;
  }
  char buf[FS_MAX_PATH];
  if (!_fs_concat_path(buf, &_fs.write_dir, name)) {
    return false;
  }
  FILE* fp = _fs_native_open(buf, _FS_MAPPEND);
  return _fs_native_write(fp, data);
}

bool fs_get_info(const char* path, fs_info* info) {
  FS_ASSERT(path && info);
  char buf[FS_MAX_PATH];
  _fs_path* dir = &_fs.base_path[_fs.count - 1];
  for (; dir >= _fs.base_path; dir--) {
    if (_fs_concat_path(buf, dir, path) && _fs_get_file_info(buf, info)) {
      return true;
    }
  }
  return false;
}

const char* fs_get_cwd() {
  if (_fs.cwd[0] == 0 && getcwd(_fs.cwd, FS_MAX_PATH) == 0) {
    return NULL;
  }
  return _fs.cwd;
}

bool fs_mkdir(const char* path) {
  FS_ASSERT(path);
  if (_fs_strempty(&_fs.write_dir)) {
    return false;
  }
  char buf[FS_MAX_PATH];
  if (!_fs_concat_path(buf, &_fs.write_dir, path)) {
    return false;
  }
  return _fs_native_mkdir(buf);
}

bool fs_delete(const char* name) {
  FS_ASSERT(name);
  if (_fs_strempty(&_fs.write_dir)) {
    return false;
  }
  char buf[FS_MAX_PATH];
  if (!_fs_concat_path(buf, &_fs.write_dir, name)) {
    return false;
  }
  return _fs_native_delete(buf);
}

inline void fs_free(void* p) {
  FS_FREE(p);
}

#endif /* FS_IMPLEMENTATION */