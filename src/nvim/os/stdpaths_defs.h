#ifndef NVIM_OS_STDPATHS_DEFS_H
#define NVIM_OS_STDPATHS_DEFS_H

typedef struct {
  const char *config_home;
  const char *data_home;
  const char *cache_home;
  const char *runtime_dir;
  const char *config_dirs;
  const char *data_dirs;
} StdPaths;

#endif  // NVIM_OS_STDPATHS_DEFS_H
