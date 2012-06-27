#include <psana_python/EnvWrapper.h>

namespace Psana {
  boost::python::list EnvWrapper::keys() {
    boost::python::list l;
    ConfigSvc::ConfigSvc cfg;
    list<string> keys = cfg.getKeys(_name);
    list<string>::iterator it;
    for (it = keys.begin(); it != keys.end(); it++) {
      string& key = *it;
      l.append(key);
    }
    return l;
  }

  string EnvWrapper::configStr(const string& parameter, const string& _default) {
    ConfigSvc::ConfigSvc cfg;
    try {
      return cfg.getStr(_name, parameter);
    } catch (const ConfigSvc::ExceptionMissing& ex) {
      return cfg.getStr(_className, parameter, _default);
    }
  }
}
