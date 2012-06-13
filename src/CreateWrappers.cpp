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
#include <psana_python/CreateDeviceWrappers.h>
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
  object EventWrapperClass;
  object EnvWrapperClass;

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

    std_vector_class_(int);
    std_vector_class_(short);
    std_vector_class_(unsigned);
    std_vector_class_(unsigned short);
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

    EventWrapperClass =
      class_<EventWrapper>("PSEvt::Event", init<EventWrapper&>())
      .def("get", &EventWrapper::get)
      .def("get", &EventWrapper::getByType)
      .def("get", &EventWrapper::getByTypeId)
      .def("getAllKeys", &EventWrapper::getAllKeys, return_value_policy<return_by_value>())
      .def("put", &EventWrapper::putBoolean)
      .def("put", &EventWrapper::putList)
      .def("run", &EventWrapper::run)
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

    EnvWrapperClass = class_<EnvWrapper>("PSEnv::Env", init<EnvWrapper&>())
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
      .def("configStr", &EnvWrapper::configStr1)
      .def("printAllKeys", &EnvWrapper::printAllKeys)
      .def("printConfigKeys", &EnvWrapper::printConfigKeys)
      .def("get", &EnvWrapper::get)
      .def("get", &EnvWrapper::get1)
      .def("getConfig", &EnvWrapper::getConfig)
      .def("getConfig", &EnvWrapper::getConfig1)
      .def("assert_psana", &EnvWrapper::assert_psana)
      .def("Source", &EnvWrapper::convertToSource)
      .def("Type", &EnvWrapper::getTypeNameForId)
      .def("subprocess", &EnvWrapper::subprocess)
      ;

    createDeviceWrappers();
    createWrappersDone = true;
  }
}

