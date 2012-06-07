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
#include <psana_python/CreateWrappers.h>

//-----------------
// C/C++ Headers --
//-----------------
#include <boost/python.hpp>
#include <psddl_python/vector_indexing_suite_nocopy.hpp>
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
#include <ConfigSvc/ConfigSvc.h>
#include <psana_python/EnvWrapper.h>
#include <psana_python/EventWrapper.h>

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

using boost::python::api::object;
using boost::python::class_;
using boost::python::copy_const_reference;
using boost::python::return_by_value;
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

#define std_vector_class_(T)\
  class_<vector<T> >("std::vector<" #T ">")\
    .def(vector_indexing_suite_nocopy<vector<T> >())

namespace Psana {
  namespace CreateWrappers {
    extern void createDeviceWrappers();
  }



  extern object EventWrapper_Class;
  extern object Event_Class;
  extern object EnvWrapper_Class;

  string stringValue_EpicsValue(EpicsStore::EpicsValue epicsValue) {
    return string(epicsValue);
  }

  double doubleValue_EpicsValue(EpicsStore::EpicsValue epicsValue) {
    return double(epicsValue);
  }

  static bool createWrappersDone = false;

  void createWrappers() {
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
    std_vector_class_(std::string);

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

#if 0
    Event_Class =
      class_<PSEvt::Event>("PSEvt::Event", init<Event&>())
      .def("get", &get_Event)
      .def("getByType", &getByType_Event)
      .def("getAllKeys", &getAllKeys_Event, return_value_policy<return_by_value>())
      .def("run", &run_Event)
      ;
#endif

    EventWrapper_Class =
      class_<EventWrapper>("PSEvt::Event", init<EventWrapper&>())
      .def("get", &EventWrapper::get_Event)
      .def("getByType", &EventWrapper::getByType_Event)
      .def("getAllKeys", &EventWrapper::getAllKeys_Event, return_value_policy<return_by_value>())
      .def("run", &EventWrapper::run_Event)
      ;

    class_<EnvObjectStoreWrapper>("PSEnv::EnvObjectStore", init<EnvObjectStore&>())
      .def("get", &EnvObjectStoreWrapper::getBySrc)
      .def("get", &EnvObjectStoreWrapper::getBySource)
      .def("get", &EnvObjectStoreWrapper::getByType1)
      .def("get", &EnvObjectStoreWrapper::getByType2)
      .def("keys", &EnvObjectStoreWrapper::keys)
      ;

    class_<Pds::Src>("Pds::Src", no_init)
      .def("log", &Pds::Src::log)
      .def("phy", &Pds::Src::phy)
      ;

    class_<EpicsStore::EpicsValue>("PSEnv::EpicsValue", no_init)
      .def("stringValue", stringValue_EpicsValue)
      .def("doubleValue", doubleValue_EpicsValue)
      ;

    EnvWrapper_Class = EnvWrapper::getBoostPythonClass();

    CreateWrappers::createDeviceWrappers();

    createWrappersDone = true;
  }
}

