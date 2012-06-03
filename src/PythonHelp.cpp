////////////////////////////////////////////////////////////////////////////////
//
// XXX TO DO:
//
// Python wrappers should use attributes instead of functions
// e.g. ConfigV1.pvControls[i] instead of ConfigV1.pvControls()[i]
//
//
////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class PythonHelp
//
// Author List:
//   Joseph S. Barrera III
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include <psana_python/PythonHelp.h>

//-----------------
// C/C++ Headers --
//-----------------
#include <boost/python.hpp>
#include <psddl_python/vector_indexing_suite_nocopy.hpp>
#include <boost/utility.hpp>
#include <numpy/arrayobject.h>
#include <string>
#include <set>
#include <cxxabi.h>
#include <python/Python.h>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include <MsgLogger/MsgLogger.h>
#include <PSEnv/Env.h>
#include <PSEnv/EpicsStore.h>
#include <PSEvt/Event.h>
#include <PSEvt/EventId.h>
#include <ConfigSvc/ConfigSvc.h>
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
using boost::python::vector_indexing_suite_nocopy;

using std::map;
using std::set;
using std::string;
using std::vector;
using std::list;

using PSEnv::Env;
using PSEnv::EnvObjectStore;
using PSEnv::EpicsStore;
using PSEvt::Event;
using PSEvt::EventKey;
using PSEvt::Source;
using Pds::Src;

typedef boost::shared_ptr<PyObject> PyObjPtr;





namespace Psana {

  // This turns out not to be very useful because
  // e.g. xtc.TypeId.Type.Id_AcqConfig -> "AcqConfig" instead of "Psana::Acqiris::ConfigV1"
  string getCppTypeNameForPythonTypeId_Event(Event& evt, int typeId) {
    return string(Pds::TypeId::name(Pds::TypeId::Type(typeId)));
  }

  // XXX get rid of this
  static EvtGetter* getEvtGetterByType(const string& typeName) {
    return (EvtGetter*) GenericGetter::getGetterByType(typeName.c_str());
  }

  namespace CreateWrappers {
    extern void createWrappers();
  }

  struct PyRefDelete {
    void operator()(PyObject* obj) { Py_CLEAR(obj); }
  };

  static boost::shared_ptr<char> none((char *)0);
  static object Event_Class;
  static object EnvWrapper_Class;



  object getEnvWrapper(Env& env, const string& name, const string& className) {
    EnvWrapper _envWrapper(env, name, className);
    object envWrapper(EnvWrapper_Class(_envWrapper));
    return envWrapper;
  }

  object getEvtWrapper(Event& evt) {
    return object(Event_Class(evt));
  }

  boost::shared_ptr<string> get_Event(Event& evt, const string& key) {
    return boost::shared_ptr<string>(evt.get(key));
  }

  list<string> getAllKeys_Event(Event& evt) {
    Event::GetResultProxy proxy = evt.get();
    list<EventKey> keys;
    proxy.m_dict->keys(keys, Source());

    list<string> keyNames;
    list<EventKey>::iterator it;
    for (it = keys.begin(); it != keys.end(); it++) {
      EventKey& key = *it;
      cout << "THIS is a key: " << key << endl;

      //cout << "THIS is a key typeid: " << key << endl;

      int status;
      char* keyName = abi::__cxa_demangle(key.typeinfo()->name(), 0, 0, &status);

      cout << "THIS is a keyName: " << keyName << endl;
      keyNames.push_back(string(keyName));
    }
    return keyNames;
  }

  int run_Event(Event& evt) {
    const boost::shared_ptr<PSEvt::EventId> eventId = evt.get();
    return eventId->run();
  }

  object getByType_Event(Event& evt, const string& typeName, const string& detectorSourceName) {
    if (typeName == "PSEvt::EventId") {
      const boost::shared_ptr<PSEvt::EventId> eventId = evt.get();
      return object(eventId);
    }

    //printAllKeys_Event(evt);
    Source detectorSource;
    if (detectorSourceName == "") {
      detectorSource = Source();
    } else {
      detectorSource = Source(detectorSourceName);
    }
    EvtGetter *getter = getEvtGetterByType(typeName);
    if (getter) {
      return getter->get(evt, detectorSource, string());
    }
    return object(none);
  }

  static bool createWrappersDone = false;

#define std_vector_class_(T)\
  class_<vector<T> >("std::vector<" #T ">")\
    .def(vector_indexing_suite_nocopy<vector<T> >())

  void createWrappers()
  {
    if (createWrappersDone) {
      return;
    }

    // Required initialization of numpy array support
    _import_array();
    array::set_module_and_type("numpy", "ndarray");

    printf("std_vector_class_(int)...\n");
    std_vector_class_(int);
    std_vector_class_(short);
    std_vector_class_(unsigned);
    std_vector_class_(unsigned short);
    printf("std_vector_class_(EventKey)...\n");
    std_vector_class_(EventKey);
    std_vector_class_(string);

    class_<PSEnv::EnvObjectStore::GetResultProxy>("PSEnv::EnvObjectStore::GetResultProxy", no_init)
      ;

    class_<PSEnv::EpicsStore, boost::noncopyable>("PSEnv::EpicsStore", no_init)
      .def("value", &EpicsStore::value)
      ;

    class_<PSEvt::Source>("PSEvt::Source", no_init)
      .def("match", &Source::match)
      .def("isNoSource", &Source::isNoSource)
      .def("isExact", &Source::isExact)
      .def("src", &Source::src, return_value_policy<reference_existing_object>())
      ;

    Event_Class =
      class_<PSEvt::Event>("PSEvt::Event", init<Event&>())
      .def("get", &get_Event)
      .def("getByType", &getByType_Event)
      .def("getAllKeys", &getAllKeys_Event)
      .def("run", &run_Event)
      .def("getCppTypeNameForPythonTypeId", &getCppTypeNameForPythonTypeId_Event);
      ;

    class_<EnvObjectStoreWrapper>("PSEnv::EnvObjectStore", init<EnvObjectStore&>())
      .def("getBySrc", &EnvObjectStoreWrapper::getBySrc)
      .def("getBySource", &EnvObjectStoreWrapper::getBySource)
      .def("getByType", &EnvObjectStoreWrapper::getByType)
      .def("keys", &EnvObjectStoreWrapper::keys)
      ;

    EnvWrapper_Class = EnvWrapper::getBoostPythonClass();

    CreateWrappers::createWrappers();

    createWrappersDone = true;
  }

  // call specific method
  boost::shared_ptr<PyObject> call(PyObject* method, Event& evt, Env& env, const string& name, const string& className)
  {
    object envWrapper = Psana::getEnvWrapper(env, name, className);
    object evtWrapper = Psana::getEvtWrapper(evt);

    PyObjPtr args(PyTuple_New(2), PyRefDelete());
    PyTuple_SET_ITEM(args.get(), 0, evtWrapper.ptr());
    PyTuple_SET_ITEM(args.get(), 1, envWrapper.ptr());
    PyObjPtr res(PyObject_Call(method, args.get(), NULL), PyRefDelete());
    return res;
  }


} // namespace Psana
