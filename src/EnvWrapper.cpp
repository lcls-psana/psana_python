#include <psana_python/EnvWrapper.h>
#include <PSEvt/EventId.h>
#include <ConfigSvc/ConfigSvc.h>
#include <psddl_python/EnvGetter.h>

using PSEnv::EnvObjectStore;
using PSEvt::EventKey;
using PSEvt::Source;
using boost::python::api::object;

#if 0

//-----------------
// C/C++ Headers --
//-----------------
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/utility.hpp>
#include <numpy/arrayobject.h>
#include <string>
#include <set>
#include <python/Python.h>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include <MsgLogger/MsgLogger.h>
#include <PSEnv/Env.h>
#include <PSEnv/EpicsStore.h>
#include <PSEvt/Event.h>
#include <psddl_python/GenericGetter.h>
#include <psddl_python/EvtGetter.h>
#include <psana_python/EnvWrapper.h>

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

using boost::python::api::object;
using boost::python::class_;
using boost::python::copy_const_reference;
using boost::python::init;
using boost::python::no_init;
using boost::python::numeric::array;
using boost::python::reference_existing_object;
using boost::python::return_value_policy;
using boost::python::vector_indexing_suite;

using std::map;
using std::set;
using std::string;
using std::vector;
using std::list;

using PSEnv::Env;
using PSEnv::EpicsStore;
using PSEvt::Event;
using Pds::Src;

typedef boost::shared_ptr<PyObject> PyObjPtr;


#endif

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

  const char* EnvWrapper::configStr(const string& parameter) {
    ConfigSvc::ConfigSvc cfg;
    try {
      return cfg.getStr(_name, parameter).c_str();
    } catch (const ConfigSvc::ExceptionMissing& ex) {
      try {
        return cfg.getStr(_className, parameter).c_str();
      } catch (const ConfigSvc::ExceptionMissing& ex) {
        return 0;
      }
    }
  }

  string EnvWrapper::configStr2(const string& parameter, const char* _default) {
    if (_default == 0) {
      return configStr(parameter);
    }
    ConfigSvc::ConfigSvc cfg;
    try {
      return cfg.getStr(_name, parameter);
    } catch (const ConfigSvc::ExceptionMissing& ex) {
      return cfg.getStr(_className, parameter, _default);
    }
  }

  Source EnvWrapper::configSource(const string& _default) {
    const char* value = configStr("source");
    if (value) {
      return Source(value);
    } else {
      return Source(_default);
    }
  }

  // XXX get rid of this
  static EnvGetter* getEnvGetterByType(const string& typeName) {
    printf("~~~ getEnvGetterByType('%s')\n", typeName.c_str());
    EnvGetter* result = (EnvGetter*) GenericGetter::getGetterByType(typeName.c_str());
    printf("~~~ getEnvGetterByType('%s') returns %p\n", typeName.c_str(), result);
    return result;
  }

  object EnvWrapper::getConfigByType2(const char* typeName, const char* detectorSourceName) {
    printf("~~~ getConfigByType('%s', '%s')\n", typeName, detectorSourceName);
    const Source detectorSource(detectorSourceName);
    EnvGetter *getter = getEnvGetterByType(typeName);
    if (getter) {
      return getter->get(_env.configStore(), detectorSource);
    }
    return object((void*) 0);
  }

  object EnvWrapper::getConfigByType1(const char* typeName) {
    return getConfigByType2(typeName, "");
  }

  object EnvWrapper::getConfig2(int typeId, const char* detectorSourceName) {
    printf("-> getConfig2(%d, '%s')\n", typeId, detectorSourceName);
    const char* name0 = Pds::TypeId::name(Pds::TypeId::Type(typeId));
    printf("-> name0='%s'\n", name0);
    char name[64];
    sprintf(name, "@EnvType_%d_", typeId);
    return getConfigByType2(name, detectorSourceName);
  }

  object EnvWrapper::getConfig1(int typeId) {
    printf("-> getConfig1(%d)\n", typeId);
    return getConfig2(typeId, "ProcInfo()");
  }
}
