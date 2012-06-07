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

  Source EnvWrapper::convertToSource(const string& value) {
    return Source(value);
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

  void EnvWrapper::assert_psana() {
  }

  using boost::python::class_;
  using boost::python::copy_const_reference;
  using boost::python::init;
  using boost::python::no_init;
  using boost::python::numeric::array;
  using boost::python::reference_existing_object;
  using boost::python::return_value_policy;

  object EnvWrapper::getBoostPythonClass() {
    return class_<EnvWrapper>("PSEnv::Env", init<EnvWrapper&>())
      .def("jobName", &EnvWrapper::jobName, return_value_policy<copy_const_reference>())
      .def("instrument", &EnvWrapper::instrument, return_value_policy<copy_const_reference>())
      .def("experiment", &EnvWrapper::experiment, return_value_policy<copy_const_reference>())
      .def("expNum", &EnvWrapper::expNum)
      .def("calibDir", &EnvWrapper::calibDir, return_value_policy<copy_const_reference>())
      .def("configStore", &EnvWrapper::configStore)
      .def("calibStore", &EnvWrapper::calibStore, return_value_policy<reference_existing_object>())
      .def("epicsStore", &EnvWrapper::epicsStore, return_value_policy<reference_existing_object>())
      .def("rhmgr", &EnvWrapper::rhmgr, return_value_policy<reference_existing_object>())
      .def("hmgr", &EnvWrapper::hmgr, return_value_policy<reference_existing_object>())
      .def("configStr", &EnvWrapper::configStr)
      .def("configStr", &EnvWrapper::configStr2)
      .def("Source", &EnvWrapper::convertToSource)
      .def("printAllKeys", &EnvWrapper::printAllKeys)
      .def("printConfigKeys", &EnvWrapper::printConfigKeys)
      .def("get", &EnvWrapper::getConfigByType1)
      .def("get", &EnvWrapper::getConfigByType2)
      .def("getConfig", &EnvWrapper::getConfig1)
      .def("getConfig", &EnvWrapper::getConfig2)
      .def("assert_psana", &EnvWrapper::assert_psana)
      ;
  }
}
