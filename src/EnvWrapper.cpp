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

  string EnvWrapper::configStr(const string& parameter, const string& _default) {
    ConfigSvc::ConfigSvc cfg;
    try {
      return cfg.getStr(_name, parameter);
    } catch (const ConfigSvc::ExceptionMissing& ex) {
      return cfg.getStr(_className, parameter, _default);
    }
  }

  object EnvWrapper::get(const char* typeName, const char* detectorSourceName) {
    Pds::Src m_foundSrc;
    const Source detectorSource(detectorSourceName);
    EnvGetMethod method(_env.configStore(), detectorSource, &m_foundSrc);
    string typeName2(typeName);
    return GenericGetter::get(typeName2, &method);
  }

  object EnvWrapper::getConfig(int typeId, const char* detectorSourceName) {
    printf("*** getConfig(%d, '%s')\n", typeId, detectorSourceName);
    Pds::Src m_foundSrc;
    const Source detectorSource(detectorSourceName);
    EnvGetMethod method(_env.configStore(), detectorSource, &m_foundSrc);
    string typeName = GenericGetter::getTypeNameForId(typeId);
    if (typeName == "") {
      printf("*** getConfig(%d, '%s'): could not find type name for type id %d\n", typeId, detectorSourceName, typeId);
      return object();
    }
    return GenericGetter::get(typeName, &method);
  }
}
