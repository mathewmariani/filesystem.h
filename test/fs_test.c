#include "acutest.h"

#define FS_IMPL
#include "filesystem.h"

#define _WRITE_DIR   "./?"
#define _SEARCH_PATH "./?"
#define _FILE_DATA   "quick brown fox"

#define CREATE_FILE(name)             \
  do {                                \
    const char *data = _FILE_DATA;    \
    fs_write(name, &(fs_write_desc) { \
      .data = data,                   \
      .size = strlen(data),           \
    });                               \
  } while (0)

#define CREATE_DIR(path) \
  do {                   \
    fs_mkdir(path);      \
  } while (0)

void test_fs_append(void) {
  int err;
  fs_info info;

  /* no write directory */
  err = fs_append("example.txt", &(fs_write_desc) {
    .data = _FILE_DATA,
    .size = strlen(_FILE_DATA),
  });

  TEST_CHECK(err == FS_ENOWRITEDIR);

  /* set search path */
  fs_set_search_path(_SEARCH_PATH);

  /* set write directory */
  fs_set_write_dir(_WRITE_DIR);

  /* file should not already exist */
  err = fs_exists("example.txt");
  TEST_CHECK(err == FS_EFAILURE);

  /* create a new file */
  err = fs_append("example.txt", &(fs_write_desc) {
    .data = _FILE_DATA,
    .size = strlen(_FILE_DATA),
  });
  TEST_CHECK(err == FS_ESUCCESS);

  /* check the file we created */
  err = fs_get_info("example.txt", &info);
  TEST_CHECK(err == FS_ESUCCESS);
  TEST_CHECK(info.type == FS_FILETYPE_REG);
  TEST_CHECK(info.size == strlen(_FILE_DATA));
  TEST_CHECK(info.modtime > 0);

  /* compare contents of file */
  size_t size;
  char *txt = fs_read("example.txt", &size);
  TEST_CHECK(txt != NULL);
  TEST_CHECK(size == info.size);
  TEST_CHECK(strcmp(txt, _FILE_DATA) == 0);

  /* should not overwrite file */
  const char *new_text = " jumps over the lazy dog";
  err = fs_append("example.txt", &(fs_write_desc) {
    .data = new_text,
    .size = strlen(new_text),
  });
  TEST_CHECK(err == FS_ESUCCESS);

  /* compare contents of file */
  txt = fs_read("example.txt", &size);
  TEST_CHECK(txt != NULL);
  TEST_CHECK(size == strlen(_FILE_DATA) + strlen(new_text));

  /* should create directory tree */
  err = fs_append("foo/example.txt", &(fs_write_desc) {
    .data = _FILE_DATA,
    .size = strlen(_FILE_DATA),
  });
  TEST_CHECK(err == FS_ESUCCESS);

  /* check we created a directory */
  err = fs_get_info("foo", &info);
  TEST_CHECK(err == FS_ESUCCESS);
  TEST_CHECK(info.type == FS_FILETYPE_DIR);  

  /* cleanup after test */
  fs_delete("foo/example.txt");
  fs_delete("foo");
  fs_delete("example.txt");
}


void test_fs_delete(void) {
  int err;

  /* set search path */
  fs_set_search_path(_SEARCH_PATH);

  /* delete without write directory */
  err = fs_delete("example.txt");
  TEST_CHECK(err == FS_ENOWRITEDIR);

  fs_set_write_dir(_WRITE_DIR);

  /* delete non-existing file */
  err = fs_delete("not_a_file.txt");
  TEST_CHECK(err == FS_EREMOVE);

  /* delete non-existing directory */
  err = fs_delete("foo");
  TEST_CHECK(err == FS_EREMOVE);

  /* delete an existing file */
  CREATE_FILE("delete_me.txt");
  err = fs_delete("delete_me.txt");
  TEST_CHECK(err == FS_ESUCCESS);

  /* sanity, should not exist */
  err = fs_exists("delete_me.txt");
  TEST_CHECK(err == FS_EFAILURE);  

  /* delete an existing, not empty, directory */
  CREATE_DIR("foo/bar");
  err = fs_delete("foo");
  TEST_CHECK(err == FS_EREMOVE);

  /* sanity, should exist */
  err = fs_exists("foo");
  TEST_CHECK(err == FS_ESUCCESS);

  /* delete an existing, empty, directory */
  err = fs_delete("foo/bar");
  TEST_CHECK(err == FS_ESUCCESS);

  /* sanity, should not exist */
  err = fs_exists("foo/bar");
  TEST_CHECK(err == FS_EFAILURE); 

  /* cleanup after test */
  fs_delete("foo");
}

void test_fs_exists(void) {
  int err;

  /* without search path */
  err = fs_exists("example.txt");
  TEST_CHECK(err == FS_ENOSEARCHPATH);

  /* set search path */
  fs_set_search_path(_SEARCH_PATH);

  /* non-existing file */
  err = fs_exists("example.txt");
  TEST_CHECK(err == FS_EFAILURE);

  /* non-existing directory */
  err = fs_exists("foo");
  TEST_CHECK(err == FS_EFAILURE);

  /* set write directory */
  fs_set_write_dir(_WRITE_DIR);

  /* existing file */
  CREATE_FILE("example.txt");
  err = fs_exists("example.txt");
  TEST_CHECK(err == FS_ESUCCESS);

  /* existing directory */
  CREATE_DIR("foo");
  err = fs_exists("foo");
  TEST_CHECK(err == FS_ESUCCESS);

  /* cleanup after test */
  fs_delete("example.txt");
  fs_delete("foo");
}

void test_fs_get_cwd(void) {
  char *cwd = fs_get_cwd();
  TEST_CHECK(cwd != NULL);

  fs_free(cwd);
  cwd = NULL;
  TEST_CHECK(cwd == NULL);
}

void test_fs_get_info(void) {
  int err;
  fs_info info;

  /* no search path */
  err = fs_get_info("example.txt", &info);
  TEST_CHECK(err == FS_ENOSEARCHPATH);

  /* set search path */
  fs_set_search_path(_SEARCH_PATH);

  /* non-existing file */
  err = fs_get_info("example.txt", &info);
  TEST_CHECK(err == FS_EFAILURE);

  /* non-existing diretory */
  err = fs_get_info("foo", &info);
  TEST_CHECK(err == FS_EFAILURE);

  /* set write directory */
  fs_set_write_dir(_WRITE_DIR);

  /* existing file */
  CREATE_FILE("example.txt");
  err = fs_get_info("example.txt", &info);
  TEST_CHECK(err == FS_ESUCCESS);
  TEST_CHECK(info.type == FS_FILETYPE_REG);
  TEST_CHECK(info.size > 0);
  TEST_CHECK(info.modtime > 0);

  /* existing directory */
  CREATE_DIR("foo");
  err = fs_get_info("foo", &info);
  TEST_CHECK(err == FS_ESUCCESS);
  TEST_CHECK(info.type == FS_FILETYPE_DIR);
  TEST_CHECK(info.size > 0);
  TEST_CHECK(info.modtime > 0);

  /* cleanup after test */
  fs_delete("example.txt");
  fs_delete("foo");
}

void test_fs_mkdir(void) {
  int err;
  fs_info info;

  /* set search path */
  fs_set_search_path(_SEARCH_PATH);

  /* no write dir */
  err = fs_mkdir("foo");
  TEST_CHECK(err == FS_ENOWRITEDIR);

  /* set write directory */
  fs_set_write_dir(_WRITE_DIR);

  /* with write dir */
  err = fs_mkdir("foo");
  TEST_CHECK(err == FS_ESUCCESS);

  /* check if we created a directory */
  err = fs_get_info("foo", &info);
  TEST_CHECK(err == FS_ESUCCESS);
  TEST_CHECK(info.type == FS_FILETYPE_DIR);

  /* create existing directory */
  err = fs_mkdir("foo");
  TEST_CHECK(err == FS_EMKDIRFAIL);

  /* create all directories in path */
  err = fs_mkdir("foo/bar");
  TEST_CHECK(err == FS_ESUCCESS);

  /* check if we created a directory */
  err = fs_get_info("foo/bar", &info);
  TEST_CHECK(err == FS_ESUCCESS);
  TEST_CHECK(info.type == FS_FILETYPE_DIR);

  /* cleanup after test */
  fs_delete("foo/bar");
  fs_delete("foo");
}

void test_fs_read(void) {
  size_t size;
  void *data;
  fs_info info;

  /* read doesn't return an error code */
  /* set search path */
  fs_set_search_path(_SEARCH_PATH);

  /* set write directory */
  fs_set_write_dir(_WRITE_DIR);

  /* non-existing file */
  data = fs_read("example.txt", &size);
  TEST_CHECK(data == NULL);

  CREATE_FILE("example.txt");
  fs_get_info("example.txt", &info);

  /* existing file */
  data = fs_read("example.txt", &size);
  TEST_CHECK(data != NULL);
  TEST_CHECK(size == info.size);

  fs_free(data);
  data = NULL;

  /* cleanup after test */
  fs_delete("example.txt");
}

void test_fs_write(void) {
  int err;
  fs_info info;

  /* no write directory */
  err = fs_write("example.txt", &(fs_write_desc) {
    .data = _FILE_DATA,
    .size = strlen(_FILE_DATA),
  });
  TEST_CHECK(err == FS_ENOWRITEDIR);

  /* set search path */
  fs_set_search_path(_SEARCH_PATH);

  /* set write directory */
  fs_set_write_dir(_WRITE_DIR);

  /* file should not already exist */
  err = fs_exists("example.txt");
  TEST_CHECK(err == FS_EFAILURE);

  /* create a new file */
  err = fs_write("example.txt", &(fs_write_desc) {
    .data = _FILE_DATA,
    .size = strlen(_FILE_DATA),
  });
  TEST_CHECK(err == FS_ESUCCESS);

  /* check the file we created */
  err = fs_get_info("example.txt", &info);
  TEST_CHECK(err == FS_ESUCCESS);
  TEST_CHECK(info.type == FS_FILETYPE_REG);
  TEST_CHECK(info.size == strlen(_FILE_DATA));
  TEST_CHECK(info.modtime > 0);

  /* compare contents of file */
  size_t size;
  char *txt = fs_read("example.txt", &size);
  TEST_CHECK(txt != NULL);
  TEST_CHECK(size == info.size);
  TEST_CHECK(strcmp(txt, _FILE_DATA) == 0);

  /* should overwrite file */
  const char *new_text = "all new data";
  err = fs_write("example.txt", &(fs_write_desc) {
    .data = new_text,
    .size = strlen(new_text),
  });
  TEST_CHECK(err == FS_ESUCCESS);

  /* compare contents of file */
  txt = fs_read("example.txt", &size);
  TEST_CHECK(txt != NULL);
  TEST_CHECK(size != info.size);
  TEST_CHECK(strcmp(txt, _FILE_DATA) != 0);
  TEST_CHECK(strcmp(txt, new_text) == 0);

  /* should create directory tree */
  err = fs_write("foo/example.txt", &(fs_write_desc) {
    .data = _FILE_DATA,
    .size = strlen(_FILE_DATA),
  });
  TEST_CHECK(err == FS_ESUCCESS);

  /* check we created a directory */
  err = fs_get_info("foo", &info);
  TEST_CHECK(err == FS_ESUCCESS);
  TEST_CHECK(info.type == FS_FILETYPE_DIR);  

  /* cleanup after test */
  fs_delete("foo/example.txt");
  fs_delete("foo");
  fs_delete("example.txt");
}

TEST_LIST = {
  { "fs_append", test_fs_append },
  { "fs_delete", test_fs_delete },
  { "fs_exists", test_fs_exists },
  { "fs_get_cwd", test_fs_get_cwd },
  { "fs_get_info", test_fs_get_info },
  { "fs_mkdir", test_fs_mkdir },
  { "fs_read", test_fs_read },
  { "fs_write", test_fs_write },

  /* always last. */
  { NULL, NULL }
};