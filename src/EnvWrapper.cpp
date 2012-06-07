#include <psana_python/EnvWrapper.h>
#include <PSEvt/EventId.h>
#include <ConfigSvc/ConfigSvc.h>
#include <psddl_python/EnvGetter.h>
#include <psddl_python/EnvGetMethod.h>

using PSEnv::EnvObjectStore;
using PSEvt::EventKey;
using PSEvt::Source;
using boost::python::api::object;

namespace Psana {

  void EnvWrapper::printAllKeys() {
    EnvObjectStore& store = _env.configStore();
    list<EventKey> keys = store.keys(); // , Source());
    list<EventKey>::iterator it;
    for (it = keys.begin(); it != keys.end(); it++) {
      cout << "THIS is an ENV key: " << *it << endl;
    }
  }

  void EnvWrapper::printConfigKeys() {
    ConfigSvc::ConfigSvc cfg;
    list<string> keys = cfg.getKeys(_name);
    list<string>::iterator it;
    cout << "!!! keys.size() = " << keys.size() << " for " << _name << endl;
    for (it = keys.begin(); it != keys.end(); it++) {
      cout << "THIS is an ConfigSvc key: " << *it << endl;
    }
  }

  string EnvWrapper::configStr(const string& parameter) {
    ConfigSvc::ConfigSvc cfg;
    try {
      return cfg.getStr(_name, parameter).c_str();
    } catch (const ConfigSvc::ExceptionMissing& ex) {
#if 1
      return cfg.getStr(_className, parameter);
#else
      try {
        return cfg.getStr(_className, parameter);
      } catch (const ConfigSvc::ExceptionMissing& ex) {
        return 0; // XXX raise python exception?
      }
#endif
    }
  }

  string EnvWrapper::configStr2(const string& parameter, const string& _default) {
    ConfigSvc::ConfigSvc cfg;
    try {
      return cfg.getStr(_name, parameter);
    } catch (const ConfigSvc::ExceptionMissing& ex) {
      return cfg.getStr(_className, parameter, _default);
    }
  }

  object EnvWrapper::getConfigByType2(const char* typeName, const char* detectorSourceName) {
    Pds::Src m_foundSrc;
    const Source detectorSource(detectorSourceName);
    EnvGetMethod method(_env.configStore(), detectorSource, &m_foundSrc);
    string typeName2(typeName);
    return GenericGetter::get(typeName2, &method);
  }

  object EnvWrapper::getConfigByType1(const char* typeName) {
    return getConfigByType2(typeName, "");
  }

  object EnvWrapper::getConfig2(int typeId, const char* detectorSourceName) {
    Pds::Src m_foundSrc;
    const Source detectorSource(detectorSourceName);
    EnvGetMethod method(_env.configStore(), detectorSource, &m_foundSrc);
    return GenericGetter::get(typeId, &method);
  }

  object EnvWrapper::getConfig1(int typeId) {
    printf("-> getConfig1(%d)\n", typeId);
    return getConfig2(typeId, "ProcInfo()");
  }
}
