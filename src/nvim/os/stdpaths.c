#include "nvim/os/os.h"
#include "nvim/strings.h"
#include "nvim/path.h"
#include "nvim/garray.h"

typedef enum {
  kConfigHome,
  kDataHome,
  kCacheHome,
  kRuntimeDir,
  kConfigDirs,
  kDataDirs,
} XDGDirType;

static const char *xdg_env_vars[] = {
  [kConfigHome] = "XDG_CONFIG_HOME",
  [kDataHome] = "XDG_DATA_HOME",
  [kCacheHome] = "XDG_CACHE_HOME",
  [kRuntimeDir] = "XDG_RUNTIME_DIR",
  [kConfigDirs] = "XDG_CONFIG_DIRS",
  [kDataDirs] = "XDG_DATA_DIRS",
};

static const char *const xdg_defaults[] = {
  // Windows, Apple stuff are just shims right now
#ifdef WIN32
  // Windows
#elif APPLE
  // Apple (this includes iOS, which we might need to handle differently)
  [kConfigHome] = "~/Library/Preferences",
  [kDataHome] = "~/Library/Application Support",
  [kCacheHome] = "~/Library/Caches",
  [kRuntimeDir] = "~/Library/Application Support",
  [kConfigDirs] = "/Library/Application Support",
  [kDataDirs] = "/Library/Application Support",
#else
  // Linux, BSD, CYGWIN
  [kConfigHome] = "~/.config",
  [kDataHome] = "~/.local/share",
  [kCacheHome] = "~/.cache",
  [kRuntimeDir] = "",
  [kConfigDirs] = "/etc/xdg/",
  [kDataDirs] = "/usr/local/share/:/usr/share/",
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
  return get_xdg_home(kConfigHome);
}

const char *get_from_user_conf(const char * fname)
{
  return (const char *)concat_fnames(get_user_conf_dir(), fname, true);
}

const char *get_user_data_dir(void)
{
  return get_xdg_home(kDataHome);
}

const char *get_from_user_data(const char * fname)
{
  const char *dir = (const char *)concat_fnames(get_user_data_dir(), fname, true);
  if (!os_isdir((char_u *)dir)) {
    create_dir(dir, 0755, fname);
  }
  return dir;
}
