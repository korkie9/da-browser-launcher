#include <filesystem>
#include <string>
#include <unistd.h>

using namespace std;
using namespace filesystem;

string get_resource_path() {
  const char *xdg_data = getenv("XDG_DATA_HOME");
  string path = xdg_data ? xdg_data : string(getenv("HOME")) + "/.local/share";
  return path + "/da-browser-launcher/";
}

string find_resource(const string &fileName) {
  string system_path = "/usr/share/da-browser-launcher/" + fileName;
  if (exists(system_path))
    return system_path;

  // User path
  string user_path = get_resource_path() + fileName;
  if (exists(user_path))
    return user_path;

  throw std::runtime_error("Resource not found: " + fileName);
}
