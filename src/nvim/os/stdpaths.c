#include "nvim/os/os.h"
#include "nvim/strings.h"
#include "nvim/path.h"
#include "nvim/garray.h"

static const char *std_paths_spec[][2] = {
  {"XDG_CONFIG_HOME", "~/.config"},
  {"XDG_DATA_HOME", "~/.local/share"},
  {"XDG_CACHE_HOME", "~/.cache"},
  {"XDG_RUNTIME_DIR", "/tmp/${USER}-runtime-dir"},
  {"XDG_CONFIG_DIRS", "/etc/xdg"},
  {"XDG_DATA_DIRS", "/usr/local/share:/usr/share"}
};


enum spec_order {
  CONFIG_HOME,
  DATA_HOME,
  CACHE_HOME,
  RUNTIME_DIR,
  CONFIG_DIRS,
  DATA_DIRS
};


StdPaths std_paths;

void init_std_paths(void) 
{
  std_paths.config_home = get_xdg_home(CONFIG_HOME);
  std_paths.data_home = get_xdg_home(DATA_HOME);
  std_paths.cache_home = get_xdg_home(CACHE_HOME);

  std_paths.runtime_dir = get_xdg_home(RUNTIME_DIR);
  if (!os_isdir(std_paths.runtime_dir)) {
    char *failed_dir;
    int ret = os_mkdir_recurse(std_paths.runtime_dir, 0700, &failed_dir);
  }

  std_paths.config_dirs = get_xdg(CONFIG_DIRS);
  std_paths.data_dirs = get_xdg(DATA_DIRS);
}

const char *get_xdg(int idx)
{
  const char *env = std_paths_spec[idx][0];
  const char *fallback = std_paths_spec[idx][1];

  const char *ret = os_getenv(env);
  if (!ret && fallback) {
    ret = (const char *)expand_env_save((char_u *)fallback);
  }

  return ret;
}

const char *get_xdg_home(int idx)
{
  const char *dir = get_xdg(idx);
  if (dir) {
    dir = (const char *)concat_fnames(dir, "nvim", true);
  }
  return dir;
}
