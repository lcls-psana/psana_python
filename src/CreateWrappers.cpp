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
#include <psana_python/EpicsPvHeaderWrapper.h>

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

using boost::shared_ptr;
using boost::python::api::object;
using boost::python::class_;
using boost::python::copy_const_reference;
using boost::python::return_by_value;
using boost::python::init;
using boost::python::no_init;
using boost::python::numeric::array;
using boost::python::reference_existing_object;
using boost::python::return_value_policy;

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

namespace psana_python {

object EventWrapperClass;
object EnvWrapperClass;
static bool createWrappersDone = false;

EpicsPvHeaderWrapper
value_EpicsStore(EpicsStore& epicsStore, const std::string& name, int index)
{
  EpicsStore::EpicsValue value(epicsStore.value(name, index));
  PSEnv::EpicsStoreImpl* m_impl = value.m_impl;
  return EpicsPvHeaderWrapper(m_impl->getAny(name));
}

EpicsPvHeaderWrapper
value_EpicsStore0(EpicsStore& epicsStore, const std::string& name)
{
  return value_EpicsStore(epicsStore, name, 0);
}

void
createWrappers()
{

  if (createWrappersDone) {
    return;
  }

  // Required initialization of numpy array support
  _import_array();
  array::set_module_and_type("numpy", "ndarray");

  class_<PSEnv::EnvObjectStore::GetResultProxy>("PSEnv::EnvObjectStore::GetResultProxy", no_init)
    ;

  class_<PSEnv::EpicsStore::EpicsValue>("PSEnv::EpicsStore::EpicsValue", no_init)
    ;

  class_<PSEnv::EpicsStore, boost::noncopyable>("PSEnv::EpicsStore", no_init)
    .def("value", &value_EpicsStore, return_value_policy<return_by_value>())
    .def("value", &value_EpicsStore0, return_value_policy<return_by_value>())
    ;

  class_<EpicsPvHeaderWrapper>("Psana::Epics::EpicsPvHeader", no_init)
    .def("pvId", &EpicsPvHeaderWrapper::pvId)
    .def("dbrType", &EpicsPvHeaderWrapper::dbrType)
    .add_property("iPvId", &EpicsPvHeaderWrapper::pvId)
    .add_property("iDbrType", &EpicsPvHeaderWrapper::dbrType)
    .def("numElements", &EpicsPvHeaderWrapper::numElements)
    .def("print", &EpicsPvHeaderWrapper::print)
    .def("isCtrl", &EpicsPvHeaderWrapper::isCtrl)
    .def("isTime", &EpicsPvHeaderWrapper::isTime)
    .add_property("status", &EpicsPvHeaderWrapper::status)
    .add_property("severity", &EpicsPvHeaderWrapper::severity)
    // The following are not actually methods/fields of EpicsPvHeader, but only of EpicsPvHeaderWrapper
    .add_property("value", &EpicsPvHeaderWrapper::value)
    .add_property("value", &EpicsPvHeaderWrapper::value0)
    .add_property("precision", &EpicsPvHeaderWrapper::precision)
    .add_property("units", &EpicsPvHeaderWrapper::units)
    .add_property("lower_ctrl_limit", &EpicsPvHeaderWrapper::lower_ctrl_limit)
    .add_property("upper_ctrl_limit", &EpicsPvHeaderWrapper::upper_ctrl_limit)
    .add_property("lower_disp_limit", &EpicsPvHeaderWrapper::lower_disp_limit)
    .add_property("upper_disp_limit", &EpicsPvHeaderWrapper::upper_disp_limit)
    .add_property("lower_warning_limit", &EpicsPvHeaderWrapper::lower_warning_limit)
    .add_property("upper_warning_limit", &EpicsPvHeaderWrapper::upper_warning_limit)
    .add_property("lower_alarm_limit", &EpicsPvHeaderWrapper::lower_alarm_limit)
    .add_property("upper_alarm_limit", &EpicsPvHeaderWrapper::upper_alarm_limit)
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
    .def("keys", &EventWrapper::keys, return_value_policy<return_by_value>())
    .def("put", &EventWrapper::putBoolean)
    .def("put", &EventWrapper::putList)
    .def("run", &EventWrapper::run)
    ;

  class_<EnvObjectStoreWrapper>("PSEnv::EnvObjectStore", init<EnvObjectStoreWrapper&>())
    .def("get", &EnvObjectStoreWrapper::get)
    .def("keys", &EnvObjectStoreWrapper::keys)
    ;

  class_<Pds::Src>("Pds::Src", no_init)
    .def("log", &Pds::Src::log)
    .def("phy", &Pds::Src::phy)
    ;

  EnvWrapperClass = class_<EnvWrapper>("PSEnv::Env", init<EnvWrapper&>())
    .def("jobName", &EnvWrapper::jobName, return_value_policy<copy_const_reference>())
    .def("instrument", &EnvWrapper::instrument, return_value_policy<copy_const_reference>())
    .def("experiment", &EnvWrapper::experiment, return_value_policy<copy_const_reference>())
    .def("expNum", &EnvWrapper::expNum)
    .def("calibDir", &EnvWrapper::calibDir, return_value_policy<copy_const_reference>())
    .def("configStore", &EnvWrapper::configStore)
    .def("getConfig", &EnvWrapper::getConfig)
    .def("calibStore", &EnvWrapper::calibStore, return_value_policy<reference_existing_object>())
    .def("epicsStore", &EnvWrapper::epicsStore, return_value_policy<reference_existing_object>())
    .def("rhmgr", &EnvWrapper::rhmgr, return_value_policy<reference_existing_object>())
    .def("hmgr", &EnvWrapper::hmgr, return_value_policy<reference_existing_object>())
    .def("configStr", &EnvWrapper::configStr)
    .def("keys", &EnvWrapper::keys)
    .def("assert_psana", &EnvWrapper::assert_psana)
    .def("subprocess", &EnvWrapper::subprocess)
    ;

  createDeviceWrappers();
  createWrappersDone = true;
}

}
