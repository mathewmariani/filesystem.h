#include "acutest.h"

#define FS_IMPL
#include "filesystem.h"

/* internal functions */

void test__fs_native_delete(void) {
  /* create a file */
  FILE *fp; const char* str = "The quick brown fox jumps over the lazy dog.";
  fp = fopen("is_a_file.txt" , "wb");
  fwrite(str, 1, strlen(str), fp);

  TEST_CASE("delete a file that doesn't exist");
  TEST_CHECK(_fs_native_delete("not_a_file.txt") == false);

  TEST_CASE("delete a file that does exist");
  TEST_CHECK(_fs_native_delete("is_a_file.txt") == true);

  /* cleanup */
  fclose(fp);
}

void test__fs_native_mkdir(void) {
  TEST_CASE("create a directory");
  TEST_CHECK(_fs_native_mkdir("foo") == true);

  TEST_CASE("create a directory tree");
  TEST_CHECK(_fs_native_mkdir("foo/bar/fizz") == true);

  /* cleanup */
  remove("foo/bar/fizz");
  remove("foo/bar");
  remove("foo");
}

void test__fs_native_open(void) {
  /* create a file */
  FILE *fp; const char* str = "The quick brown fox jumps over the lazy dog.";
  fp = fopen("is_a_file.txt" , "wb");
  fwrite(str, 1, strlen(str), fp);

  TEST_CASE("open file (for read) that doesn't exist");
  fp = _fs_native_open("not_a_file.txt", _FS_MREAD);
  TEST_CHECK(fp == NULL);

  TEST_CASE("open file (for read) that does exist");
  fp = _fs_native_open("is_a_file.txt", _FS_MREAD);
  TEST_CHECK(fp != NULL);

  /* cleanup */
  fclose(fp);
  remove("is_a_file.txt");
}

void test__fs_native_read(void) {
  /* create a file */
  FILE *fp; const char* str = "The quick brown fox jumps over the lazy dog.";
  fp = fopen("is_a_file.txt" , "wb");
  fwrite(str, 1, strlen(str), fp);
  size_t size;

  TEST_CASE("read from null file");
  TEST_CHECK(_fs_native_read(NULL, &size) == NULL);

  fp = fopen("not_a_file.txt", "rb");
  TEST_CASE("read file that doesn't exist");
  TEST_CHECK(_fs_native_read(fp, &size) == NULL);

  fp = fopen("is_a_file.txt", "rb");
  TEST_CASE("read file that does exist");
  TEST_CHECK(_fs_native_read(fp, &size) != NULL);

  /* cleanup */
  fclose(fp);
  remove("is_a_file.txt");
}

void test__fs_native_write(void) {
  FILE *fp = NULL;
  const char* str = "The quick brown fox jumps over the lazy dog.";
  fs_data desc = { .data = str, .size = strlen(str) };

  TEST_CASE("write to null file");
  TEST_CHECK(_fs_native_write(NULL, &desc) == false);

  fp = fopen("test_write.txt", "w");
  TEST_CASE("read file that doesn't exist");
  TEST_CHECK(_fs_native_write(fp, &desc) == true);

  fp = fopen("test_write.txt", "w");
  TEST_CASE("read file that does exist");
  TEST_CHECK(_fs_native_write(fp, &desc) == true);

  /* cleanup */
  fclose(fp);
  remove("test_write.txt");
}

/* public functions */

void test_fs_setup(void) {
  TEST_CHECK(fs_is_valid() == false);
  fs_setup(&(fs_desc) {
    .write_dir = "buzz",
    .base_paths = {
      "foo", "bar", "fizz",
    },
  });

  TEST_CHECK(fs_is_valid());

  fs_shutdown();
  TEST_CHECK(fs_is_valid() == false);
}

void test_fs_append(void) {
  /* setup filesystem */
  char* cwd = (char*) fs_get_cwd();
  fs_setup(&(fs_desc) { .write_dir = cwd, .base_paths = { cwd } });

  TEST_CASE("append to file that doesn't exist");
  if (TEST_CHECK(!fs_exists("is_a_file.txt"))) {
    const char* str = "The quick brown fox";
    TEST_CHECK(fs_append("is_a_file.txt", FS_DATA_STR_REF(str)) == true);
    TEST_CHECK(fs_exists("is_a_file.txt") == true);

    size_t size;
    char *data = fs_read("is_a_file.txt", &size);
    TEST_CHECK(data != NULL);
    TEST_CHECK(strcmp(data, str) == 0);
  }

  TEST_CASE("append to file that does exist");
  if (TEST_CHECK(fs_exists("is_a_file.txt"))) {
    const char* full = "The quick brown fox jumps over the lazy dog.";
    const char* str = " jumps over the lazy dog.";
    TEST_CHECK(fs_append("is_a_file.txt", FS_DATA_STR_REF(str)) == true);

    size_t size;
    char *data = fs_read("is_a_file.txt", &size);
    TEST_CHECK(data != NULL);
    TEST_CHECK(strcmp(data, full) == 0);
  }

  /* cleanup */
  fs_delete("is_a_file.txt");
}

void test_fs_delete(void) {
  /* setup filesystem */
  char* cwd = (char*) fs_get_cwd();
  fs_setup(&(fs_desc) { .write_dir = cwd, .base_paths = { cwd } });

  TEST_CASE("delete file that doesn't exist");
  if (TEST_CHECK(!fs_exists("is_a_file.txt"))) {
    TEST_CHECK(fs_delete("is_a_file.txt") == false);
  }


  const char* str = "The quick brown fox jumps over the lazy dog.";
  fs_write("is_a_file.txt", FS_DATA_STR_REF(str));

  TEST_CASE("delete file that does exist");
  if (TEST_CHECK(fs_exists("is_a_file.txt") == true)) {
    TEST_CHECK(fs_delete("is_a_file.txt") == true);
  }


  
  // TEST_CHECK(fs_delete("delete_me.txt") == true);

  TEST_CASE("delete directory that doesn't exist");
  if (TEST_CHECK(!fs_exists("is_a_dir"))) {
    TEST_CHECK(fs_delete("is_a_dir") == false);
  }
}

void test_fs_exists(void) {
  /* setup filesystem */
  char* cwd = (char*) fs_get_cwd();
  fs_setup(&(fs_desc) { .write_dir = cwd, .base_paths = { cwd } });

  TEST_CASE("check for file that doesn't exist");
  TEST_CHECK(fs_exists("is_a_file.txt") == false);

  TEST_CASE("check for directory that doesn't exist");
  TEST_CHECK(fs_exists("is_a_dir") == false);

  TEST_CASE("check for file that does exist");
  const char* str = "The quick brown fox jumps over the lazy dog.";
  fs_write("is_a_file.txt", &(fs_data) { .data = str, .size = strlen(str) });
  TEST_CHECK(fs_exists("is_a_file.txt") == true);

  TEST_CASE("check for directory that does exist");
  fs_mkdir("is_a_dir");
  TEST_CHECK(fs_exists("is_a_dir") == true);

  /* cleanup */
  fs_delete("is_a_file.txt");
  fs_delete("is_a_dir");
}

void test_fs_get_cwd(void) {
  const char* cwd = fs_get_cwd();
  TEST_CHECK(cwd != NULL);
}

void test_fs_get_info(void) {
  /* setup filesystem */
  char* cwd = (char*) fs_get_cwd();
  fs_setup(&(fs_desc) { .write_dir = cwd, .base_paths = { cwd } });

  fs_info info;

  TEST_CASE("get info for file that doesn't exist");
  if (TEST_CHECK(!fs_exists("is_a_file.txt"))) {
    TEST_CHECK(fs_get_info("is_a_file.txt", &info) == false);
  }

  TEST_CASE("get info for directory that doesn't exist");
  if (TEST_CHECK(!fs_exists("is_a_dir"))) {
    TEST_CHECK(fs_get_info("is_a_dir", &info) == false);
  }

  const char* str = "The quick brown fox jumps over the lazy dog.";
  fs_write("is_a_file.txt", &(fs_data) { .data = str, .size = strlen(str) });

  if (TEST_CHECK(fs_exists("is_a_file.txt"))) {
    TEST_CASE("get info for file that does exist");
    TEST_CHECK(fs_get_info("is_a_file.txt", &info) == true);
    TEST_CHECK(info.type == FS_FILETYPE_REG);
  }

  fs_mkdir("is_a_dir");

  if (TEST_CHECK(fs_exists("is_a_dir"))) {
    TEST_CASE("get info for directory that does exist");
    TEST_CHECK(fs_get_info("is_a_dir", &info) == true);
    TEST_CHECK(info.type == FS_FILETYPE_DIR);
  }

  /* cleanup */
  fs_delete("is_a_dir");
  fs_delete("is_a_file.txt");
}

void test_fs_mkdir(void) {
  /* body */
}

void test_fs_read(void) {
  /* setup filesystem */
  char* cwd = (char*) fs_get_cwd();
  fs_setup(&(fs_desc) { .write_dir = cwd, .base_paths = { cwd } });

  /* create a file */
  const char* str = "The quick brown fox jumps over the lazy dog.";
  fs_write("is_a_file.txt", FS_DATA_STR_REF(str));

  TEST_CASE("read file that doesn't exist");
  if (TEST_CHECK(!fs_exists("not_a_file.txt"))) {
    size_t size;
    char* data = fs_read("not_a_file.txt", &size);
    
    TEST_CHECK(data == NULL);
  }

  TEST_CASE("read file that does exist");
  if (TEST_CHECK(fs_exists("is_a_file.txt"))) {
    size_t size;
    char* data = fs_read("is_a_file.txt", &size);
    
    TEST_CHECK(data != NULL);
    TEST_CHECK(strcmp(data, str) == 0);
  }

  /* cleanup */
  fs_delete("is_a_file.txt");
}

void test_fs_write(void) {
  /* setup filesystem */
  char* cwd = (char*) fs_get_cwd();
  fs_setup(&(fs_desc) { .write_dir = cwd, .base_paths = { cwd } });

  TEST_CASE("write to file that doesn't exist");
  if (TEST_CHECK(!fs_exists("is_a_file.txt"))) {
    const char* str = "The quick brown fox jumps over the lazy dog.";
    TEST_CHECK(fs_write("is_a_file.txt", FS_DATA_STR_REF(str)) == true);
    TEST_CHECK(fs_exists("is_a_file.txt") == true);

    size_t size;
    char *data = fs_read("is_a_file.txt", &size);
    TEST_CHECK(data != NULL);
    TEST_CHECK(strcmp(data, str) == 0);
  }

  TEST_CASE("write to file that does exist");
  if (TEST_CHECK(fs_exists("is_a_file.txt"))) {
    const char* str = "The five boxing wizards jump quickly.";
    TEST_CHECK(fs_write("is_a_file.txt", FS_DATA_STR_REF(str)) == true);

    size_t size;
    char *data = fs_read("is_a_file.txt", &size);
    TEST_CHECK(data != NULL);
    TEST_CHECK(strcmp(data, str) == 0);
  }

  /* cleanup */
  fs_delete("is_a_file.txt");
}

void test_fs_insert_basepath(void) {
  /* setup filesystem */
  fs_setup(&(fs_desc) {  });

  TEST_CASE("will add a unique base path");
  TEST_CHECK(fs_insert_basepath("foo") == true);
  TEST_CHECK(fs_insert_basepath("foo") == false);

  TEST_CHECK(strcmp("foo", _fs.base_path[0].buf) == 0);

  TEST_CASE("will add multiple unique base paths");
  TEST_CHECK(fs_insert_basepath("foo") == false);
  TEST_CHECK(fs_insert_basepath("foo/bar") == true);
  TEST_CHECK(fs_insert_basepath("foo/bar/fizz") == true);
  TEST_CHECK(fs_insert_basepath("foo/bar/fizz/buzz") == false);

  TEST_CHECK(strcmp("foo", _fs.base_path[0].buf) == 0);
  TEST_CHECK(strcmp("foo/bar", _fs.base_path[1].buf) == 0);
  TEST_CHECK(strcmp("foo/bar/fizz", _fs.base_path[2].buf) == 0);
}

void test_fs_remove_basepath(void) {
  /* setup filesystem */
  fs_setup(&(fs_desc) {  });

  fs_insert_basepath("foo");
  fs_insert_basepath("foo/bar");
  fs_insert_basepath("foo/bar/fizz");

  TEST_CASE("removing non-existant basepath should return false");
  TEST_CHECK(fs_remove_basepath("fizz/buzz") == false);

  TEST_CASE("removing existing basepath should return true");
  TEST_CHECK(fs_remove_basepath("foo") == true);

  TEST_CASE("basepaths should be reodered when one is removed");
  TEST_CHECK(strcmp("foo/bar", _fs.base_path[0].buf) == 0);
  TEST_CHECK(strcmp("foo/bar/fizz", _fs.base_path[1].buf) == 0);
  TEST_CHECK(_fs.base_path[2].buf[0] == 0);

  TEST_CHECK(fs_remove_basepath("foo/bar/fizz") == true);
  TEST_CHECK(strcmp("foo/bar", _fs.base_path[0].buf) == 0);
  TEST_CHECK(_fs.base_path[1].buf[0] == 0);
  TEST_CHECK(_fs.base_path[2].buf[0] == 0);

  TEST_CHECK(fs_remove_basepath("foo/bar") == true);
  TEST_CHECK(_fs.base_path[1].buf[0] == 0);
  TEST_CHECK(_fs.base_path[2].buf[0] == 0);
  TEST_CHECK(_fs.base_path[3].buf[0] == 0);
}

TEST_LIST = {
  /* internal functions */
  { "_fs_native_delete", test__fs_native_delete },
  { "_fs_native_mkdir", test__fs_native_mkdir },
  { "_fs_native_open", test__fs_native_open },
  { "_fs_native_read", test__fs_native_read },
  { "_fs_native_write", test__fs_native_write },

  /* public functions */
  { "fs_setup", test_fs_setup },
  { "fs_append", test_fs_append },
  { "fs_delete", test_fs_delete },
  { "fs_exists", test_fs_exists },
  { "fs_get_cwd", test_fs_get_cwd },
  { "fs_get_info", test_fs_get_info },
  { "fs_mkdir", test_fs_mkdir },
  { "fs_read", test_fs_read },
  { "fs_write", test_fs_write },

  { "fs_insert_basepath", test_fs_insert_basepath },
  { "fs_remove_basepath", test_fs_remove_basepath },

  /* always last. */
  { NULL, NULL }
};