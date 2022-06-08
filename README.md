# filesystem.h

a *tiny* library for interfacing with a filesystem.

- searches for files within a search path
- does not write outside the write directory
- supported platforms: Win32, macOS, Linux
- written in C99

## Usage
**[filesystem.h](src/filesystem.h?raw=1)** should be dropped
into an existing project and compiled along with it. The library provides 7 functions for interfacing with a filesystem.

```c
fs_append(const char* name, const fs_data* data)
fs_delete(const char* name)
fs_exists(const char* path)
fs_get_info(const char* path, fs_info* info)
fs_mkdir(const char* path)
fs_read(const char* name, size_t* size)
fs_write(const char* name, const fs_data* data)
```

```c
#include <stdio.h>

#define FS_IMPL
#include "filesystem.h"

int main(int argc, char* argv[]) {
  char* cwd = (char*) fs_get_cwd();
  fs_setup(&(fs_desc) {
    .write_dir = cwd,
    .base_paths = { cwd },
  });

  fs_info info;
  if (!fs_get_info("coffee.txt", &fs_info)) {
    printf("[error could not read file]\n");
    return -1;
  }

  printf("coffee.txt:\n  type: %d\n  size: %d\n  modtime: %ld\n"
    , info.type
    , info.size
    , info.modtime);

  return 0;
}
```

See the **[filesystem.h](src/filesystem.h?raw=1)** header for a more complete documentation.

## License
This library is free software; you can redistribute it and/or modify it under
the terms of the MIT license. See [LICENSE](LICENSE) for details.