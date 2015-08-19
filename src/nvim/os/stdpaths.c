#include "nvim/os/os.h"
#include "nvim/strings.h"
#include "nvim/path.h"
#include "nvim/garray.h"

typedef enum {
  config_home,
  data_home,
  cache_home,
  runtime_dir,
  config_dirs,
  data_dirs,
} XDGDirType;

static const char *xdg_env_vars[] = {
  [config_home] = "XDG_CONFIG_HOME",
  [data_home] = "XDG_DATA_HOME",
  [cache_home] = "XDG_CACHE_HOME",
  [runtime_dir] = "XDG_RUNTIME_DIR",
  [config_dirs] = "XDG_CONFIG_DIRS",
  [data_dirs] = "XDG_DATA_DIRS",
};

#ifdef __linux__
static const char *const xdg_defaults[] = {
  [config_home] = "~/.config",
  [data_home] = "~/.local/share",
  [cache_home] = "~/.cache",
  [runtime_dir] = "",
  [config_dirs] = "/etc/xdg/",
  [data_dirs] = "/usr/local/share/:/usr/share/",
};
#endif

static const char *get_xdg(XDGDirType idx)
{
  const char *env = xdg_env_vars[idx];
  const char *fallback = xdg_defaults[idx];

  const char *ret = os_getenv(env);
  if (!ret && fallback) {
    ret = (const char *)expand_env_save((char_u *)fallback);
  }

  return ret;
}

static const char *get_xdg_home(XDGDirType idx)
{
  const char *dir = get_xdg(idx);
  if (dir) {
    dir = (const char *)concat_fnames(dir, "nvim", true);
  }
  return dir;
}

static void create_dir(const char *dir, int mode, const char *suffix)
{
  char *failed;
  if (!os_mkdir_recurse(dir, mode, &failed)) {
    // TODO: Create a folder in $TMPDIR instead
    DLOG("Create dir failed");
  }
}

const char *get_user_conf_dir(void)
{
  return get_xdg_home(config_home);
}

const char *get_from_user_conf(const char * fname)
{
  return (const char *)concat_fnames(get_user_conf_dir(), fname, true);
}

const char *get_user_data_dir(void)
{
  return get_xdg_home(data_home);
}

const char *get_from_user_data(const char * fname)
{
  const char *dir = (const char *)concat_fnames(get_user_data_dir(), fname, true);
  if (!os_isdir((char_u *)dir)) {
    create_dir(dir, 0755, fname);
  }
  return dir;
}
