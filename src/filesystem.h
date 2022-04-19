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

    FS_MALLOC(s)     - your own malloc function (default: malloc(s))
    FS_FREE(p)       - your own free function (default: free(p))
    FS_MAX_PATH      - maximum length of a path in bytes (default: 256)


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

    fs_append(const char *name, void *data, size_t *size);
    fs_delete(const char *name);
    fs_exists(const char *path);
    fs_free(void *p);
    fs_get_cwd();
    fs_get_info(const char *path, fs_info *info);
    fs_get_search_path()
    fs_get_write_dir()
    fs_mkdir(const char *path);
    fs_read(const char *name, size_t *size);
    fs_set_search_path(const char *path);
    fs_set_write_dir(const char *path);
    fs_strerror(int err);
    fs_write(const char *name, void *data, size_t size);


    STEP BY STEP:
    =============

    --- Add a search path. A search path is a list of directories where
        to search for files. The search path is a template where the 
        interrogation point `?` is replaced by a given filename.
        The templates in a path are separated by semicolons `;`.

        `./?;c:/windows/?;/usr/local/?`

    --- Add a write directory. A write directory indicates the directory
        where we can write to. Much like the search path, the write directory
        is a template where the interrogation point `?` is replaced by a
        given filename. However, only one template is allowed.


    READING FROM A FILE:
    ====================

    --- When reading from a file, the file is opened, memory is allocated,
        copied, and the file is closed before the function returns a pointer
        to the data read.

        Ownership of the pointer is that of the user, and the pointer must
        freed after use to avoid memory leaks.


        size_t size;
        const char *data = (char *) fs_read("example.txt", &size);

        fs_free(data);


    WRITTING TO A FILE:
    ===================

    --- When writting, or appending, to a file, the file is closed after
        writting has completed, or failed.

        If the file already exists, it will be completely replaced by the new contents.
        However, if the file doesn't exist a new one will be created.


        const char *text = "the quick brown fox jumps over the lazy dog";
        int err = fs_write("example.txt", text, strlen(text));

        if (err != FS_ESUCCESS) {
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
#define FS_INCLUDED

#include <string.h>

#if !defined (FS_API_DECL)
#define FS_API_DECL extern
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* errors */
enum {
  FS_ESUCCESS      =  0,
  FS_EFAILURE      = -1,
  FS_ETOOLONG      = -2,
  FS_ENOWRITEDIR   = -3,
  FS_EWRITEFAIL    = -4,
  FS_EMKDIRFAIL    = -5,
  FS_ENOSEARCHPATH = -6,
  FS_EREMOVE       = -7,
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

typedef struct fs_write_desc {
  const void *data;
  size_t size;
} fs_write_desc;

/* converts error to a human readable string */
FS_API_DECL const char *fs_strerror(int err);
/* gets the search path */
FS_API_DECL const char *fs_get_search_path();
/* sets the search path */
FS_API_DECL int fs_set_search_path(const char *path);
/* gets the write directory */
FS_API_DECL const char *fs_get_write_dir();
/* sets the write directory */
FS_API_DECL int fs_set_write_dir(const char *path);
/* return true if a file or a directory exists */
FS_API_DECL int fs_exists(const char *path);
/* reads the contents of a file */
FS_API_DECL void *fs_read(const char *name, size_t *size);
/* writes data to a file */
FS_API_DECL int fs_write(const char *name, const fs_write_desc *desc);
/* writes data to the end of a file */
FS_API_DECL int fs_append(const char *name, const fs_write_desc *desc);
/* gets information about the specified file or directory */
FS_API_DECL int fs_get_info(const char *path, fs_info *info);
/* gets the current working directory */
FS_API_DECL char *fs_get_cwd();
/* creates a directory */
FS_API_DECL int fs_mkdir(const char *path);
/* deletes a file or directory */
FS_API_DECL int fs_delete(const char *name);
/* frees allocated memory */
FS_API_DECL inline void fs_free(void *p);

#ifdef __cplusplus
}

/* reference-based equivalents for c++ */
inline int fs_get_info(const char *filename, const fs_info &info) { return fs_get_info(filename, &info); }
inline int fs_write(const char *name, const fs_write_desc &desc) { return fs_write(filename, &desc); }
inline int fs_append(const char *name, const fs_write_desc &desc) { return fs_append(filename, &desc); }

#endif

#endif /* FS_INCLUDED */

#ifdef FS_IMPLEMENTATION
#define FS_IMPL_INCLUDED (1)

#include <stdio.h>
#include <time.h>
#include <sys/stat.h>

#if defined (_WIN32)
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

/* configuration */
#if !defined (FS_MAX_PATH)
#define FS_MAX_PATH 256
#endif
#if !defined (FS_PATH_SEP)
#define FS_PATH_SEP ";"
#endif
#if !defined (FS_PATH_MARK)
#define FS_PATH_MARK "?"
#endif

#if !defined (FS_MALLOC)
  #include <stdlib.h>
  #define FS_MALLOC(s) malloc(s)
  #define FS_FREE(p) free(p)
#endif

#if defined (_WIN32)
#define FS_DIR_SEP   '\\'
#else
#define FS_DIR_SEP   '/'
#endif

#define _FS_MREAD   "rb"
#define _FS_MWRITE  "w"
#define _FS_MAPPEND "a"

#define _FS_SANITY_WRITE_DIR() \
  if (!*_fs_write_dir) {       \
    return FS_ENOWRITEDIR;     \
  }

#define _FS_SANITY_SEARCH_PATH() \
  if (!*_fs_search_path) {       \
    return FS_ENOSEARCHPATH;     \
  } 

_FS_PRIVATE char _fs_search_path[FS_MAX_PATH];
_FS_PRIVATE char _fs_write_dir[FS_MAX_PATH];

_FS_PRIVATE const char *_fs_get_template(const char *path, char *buf) {
  const char *t;

  /* skip any separators */
  while (*path == *FS_PATH_SEP) {
    path++;
  }

  /* no more templates */
  if (*path == '\0') {
    return NULL;
  }

  t = strchr(path, *FS_PATH_SEP);
  if (t == NULL) {
    t = path + strlen(path);
  }

  strncpy(buf, path, t - path);
  buf[t - path] = '\0';
  return t;
}

#define _FS_CNCT_PATH(b, p, n)    \
  err = _fs_concat_path(b, p, n); \
  if (err) return err;            \

_FS_PRIVATE int _fs_concat_path(char *buf, const char *path, const char *filename) {
  const char *s = path;
  const char *p = FS_PATH_MARK;
  const char *r = filename;

  unsigned int l = strlen(p);
  unsigned int n = 0;

  #define CONCAT(b, n, s, l)    \
    do {                        \
      if (l <= 0) break;        \
      memcpy((void*)&b[n],s,l); \
      n+=l;                     \
    } while(0)

  /* gsub */
  const char *pch = strstr(s, p);
  while (pch != NULL) {
    CONCAT(buf, n, s, pch - s);
    CONCAT(buf, n, r, strlen(r));
    if (n >= FS_MAX_PATH) {
      return FS_ETOOLONG;
    }
    s = pch + l;
    pch = strstr(s, p);
  }

  #undef CONCAT

  /* null-terminator */
  buf[n] = '\0';

  return FS_ESUCCESS;
}

_FS_PRIVATE int _fs_check_search_path(const char *filename, char *buf, struct stat *fstat) {
  _FS_SANITY_SEARCH_PATH();

  int err;
  char str[FS_MAX_PATH];

  /* create template */
  const char *path = _fs_search_path;
  while ((path = _fs_get_template(path, &str[0])) != NULL) {
    _FS_CNCT_PATH(buf, &str[0], filename);
    if (stat(buf, fstat) == 0) {
      return FS_ESUCCESS;
    }
  }

  return FS_EFAILURE;
}

_FS_PRIVATE int _fs_get_type(const char *filename) {
  char path[FS_MAX_PATH];
  struct stat fstat;
  if (_fs_check_search_path(filename, &path[0], &fstat) != FS_ESUCCESS) {
    return FS_FILETYPE_NONE;
  }
  if (S_ISREG(fstat.st_mode)) return FS_FILETYPE_REG;
  if (S_ISDIR(fstat.st_mode)) return FS_FILETYPE_DIR;
  if (S_ISLNK(fstat.st_mode)) return FS_FILETYPE_SYM;
  return FS_FILETYPE_NONE;
}

#define _FS_CREATE_DIR_TREE()     \
  do {                            \
    if (strrchr(buf, '/')) {      \
      char tree[FS_MAX_PATH];     \
      strcpy(tree, buf);          \
      *strrchr(tree, '/') = '\0'; \
      _fs_make_dirs(tree);        \
    }                             \
  } while (0)

_FS_PRIVATE int _fs_make_dirs(char *path) {
  char *s = strrchr(path, '/');
  if (s != NULL) {
    *s = 0;
    _fs_make_dirs(path);
    *s = '/';
  }
  return (mkdir(path, S_IRWXU) == -1)
    ? FS_EMKDIRFAIL
    : FS_ESUCCESS;
}

const char *fs_strerror(int err) {
  switch (err) {
    case FS_ESUCCESS:      return "success";
    case FS_EFAILURE:      return "failure";
    case FS_ETOOLONG:      return "path too long";
    case FS_ENOWRITEDIR:   return "no write directory";
    case FS_EWRITEFAIL:    return "could not write to file";
    case FS_EMKDIRFAIL:    return "could not make directory";
    case FS_ENOSEARCHPATH: return "no search path";
    case FS_EREMOVE:       return "could not delete file or directory";
  }
  return "unknown error";
}

const char *fs_get_search_path() {
  return _fs_search_path;
}

int fs_set_search_path(const char *path) {
  if (strlen(path) >= FS_MAX_PATH) {
    return FS_ETOOLONG;
  }
  strcpy(_fs_search_path, path);
  return FS_ESUCCESS;
}

const char *fs_get_write_dir() {
  return _fs_write_dir;
}

int fs_set_write_dir(const char *path) {
  if (strlen(path) >= FS_MAX_PATH) {
    return FS_ETOOLONG;
  }
  strcpy(_fs_write_dir, path);
  return FS_ESUCCESS;
}

int fs_exists(const char *path) {
  _FS_SANITY_SEARCH_PATH();
  return _fs_get_type(path) != FS_FILETYPE_NONE
    ? FS_ESUCCESS
    : FS_EFAILURE;
}

void *fs_read(const char *name, size_t *size) {
  char path[FS_MAX_PATH];
  struct stat fstat;
  if (_fs_check_search_path(name, &path[0], &fstat) != FS_ESUCCESS) {
    return NULL;
  }
  FILE *fp = fopen(path, _FS_MREAD);
  if (!fp) {
    return NULL;
  }
  *size = fstat.st_size;
  void *b = FS_MALLOC(*size);
  if (!b) {
    return NULL;
  }
  fread(b, 1, *size, fp);
  fclose(fp);
  return b;
}

_FS_PRIVATE int _fs_write_to_file(const char *path, const char *mode, const void *data, const size_t size) {
  if (strstr(path, "..")) {
    return FS_EWRITEFAIL;
  }
  FILE *fp = fopen(path, mode);
  if (!fp) {
    return FS_EWRITEFAIL;
  }
  int wsize = fwrite(data, sizeof(char), size, fp);
  fclose(fp);
  return wsize == size
    ? FS_ESUCCESS
    : FS_EWRITEFAIL;
}

_FS_PRIVATE int _fs_write_to_file_ext(const char *path, const char *mode, const fs_write_desc *desc) {
  if (strstr(path, "..")) {
    return FS_EWRITEFAIL;
  }
  FILE *fp = fopen(path, mode);
  if (!fp) {
    return FS_EWRITEFAIL;
  }
  int wsize = fwrite(desc->data, sizeof(char), desc->size, fp);
  fclose(fp);
  return wsize == desc->size
    ? FS_ESUCCESS
    : FS_EWRITEFAIL;
}

int fs_write(const char *name, const fs_write_desc *desc) {
  _FS_SANITY_WRITE_DIR();
  int err;
  char buf[FS_MAX_PATH];
  _FS_CNCT_PATH(buf, _fs_write_dir, name);
  _FS_CREATE_DIR_TREE();
  return _fs_write_to_file_ext(buf, _FS_MWRITE, desc);
}

int fs_append(const char *name, const fs_write_desc *desc) {
  _FS_SANITY_WRITE_DIR();
  int err;
  char buf[FS_MAX_PATH];
  _FS_CNCT_PATH(buf, _fs_write_dir, name);
  _FS_CREATE_DIR_TREE();
  return _fs_write_to_file_ext(buf, _FS_MAPPEND, desc);
}

FS_API_DECL int fs_get_info(const char *path, fs_info *info) {
  _FS_SANITY_SEARCH_PATH();
  char buf[FS_MAX_PATH];
  struct stat fstat;
  int err = _fs_check_search_path(path, &buf[0], &fstat);
  if (err) {
    return err;
  }
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
  return FS_ESUCCESS;
}

FS_API_DECL char *fs_get_cwd() {
  char *cwd = FS_MALLOC(FS_MAX_PATH);
  return getcwd(cwd, FS_MAX_PATH);
}

FS_API_DECL int fs_mkdir(const char *path) {
  _FS_SANITY_WRITE_DIR();
  int err;
  char buf[FS_MAX_PATH];
  _FS_CNCT_PATH(buf, _fs_write_dir, path);
  return _fs_make_dirs(buf);
}

FS_API_DECL int fs_delete(const char *name) {
  _FS_SANITY_WRITE_DIR();
  int err;
  char buf[FS_MAX_PATH];
  _FS_CNCT_PATH(buf, _fs_write_dir, name);
  return (remove(buf) != 0)
    ? FS_EREMOVE
    : FS_ESUCCESS;
}

FS_API_DECL inline void fs_free(void *p) {
  FS_FREE(p);
}

#endif /* FS_IMPLEMENTATION */