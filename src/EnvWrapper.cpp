
#include <psana_python/EnvWrapper.h>

#include <list>

namespace psana_python {

boost::python::list
EnvWrapper::keys()
{
  boost::python::list l;
  ConfigSvc::ConfigSvc cfg;
  std::list<std::string> keys = cfg.getKeys(_name);
  for (std::list<std::string>::iterator it = keys.begin(); it != keys.end(); ++it) {
    std::string& key = *it;
    l.append(key);
  }
  return l;
}

std::string
EnvWrapper::configStr(const std::string& parameter, const std::string& _default)
{
  ConfigSvc::ConfigSvc cfg;
  try {
    return cfg.getStr(_name, parameter);
  } catch (const ConfigSvc::ExceptionMissing& ex) {
    return cfg.getStr(_className, parameter, _default);
  }
}

}
