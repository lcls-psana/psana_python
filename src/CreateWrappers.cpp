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

//-----------------
// C/C++ Headers --
//-----------------
#include <boost/python.hpp>
#include <boost/utility.hpp>
#include <string>
#include <python/Python.h>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include <MsgLogger/MsgLogger.h>
#include <PSEnv/Env.h>
#include <PSEnv/EpicsStore.h>
#include <PSEvt/Event.h>
#include <ConfigSvc/ConfigSvc.h>
#include <psana_python/Env.h>
#include <psana_python/EnvObjectStore.h>
#include <psana_python/EventId.h>
#include <psana_python/EventKey.h>
#include <psana_python/Event.h>
#include <psana_python/EpicsPvHeaderWrapper.h>
#include <psana_python/PdsBldInfo.h>
#include <psana_python/PdsDetInfo.h>
#include <psana_python/PdsProcInfo.h>
#include <psana_python/PdsSrc.h>
#include <psana_python/Source.h>
#include <psddl_python/CreateDeviceWrappers.h>

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

using boost::shared_ptr;
using boost::python::api::object;
using boost::python::arg;
using boost::python::args;
using boost::python::borrowed;
using boost::python::class_;
using boost::python::copy_const_reference;
using boost::python::return_by_value;
using boost::python::handle;
using boost::python::init;
using boost::python::no_init;
using boost::python::numeric::array;
using boost::python::optional;
using boost::python::return_internal_reference;
using boost::python::return_value_policy;
using boost::python::scope;
using boost::python::self;
using boost::python::str;

using std::string;

using PSEnv::EpicsStore;

namespace psana_python {

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
createWrappers(PyObject* module)
{

  if (createWrappersDone) {
    return;
  }

  Env::initType(module);
  EnvObjectStore::initType(module);
  Event::initType(module);
  EventId::initType(module);
  EventKey::initType(module);
  PdsBldInfo::initType(module);
  PdsDetInfo::initType(module);
  PdsProcInfo::initType(module);
  PdsSrc::initType(module);
  Source::initType(module);

  scope mod = object(handle<>(borrowed(module)));

  class_<PSEnv::EpicsStore, boost::noncopyable>("EpicsStore", no_init)
    .def("value", &value_EpicsStore, return_value_policy<return_by_value>())
    .def("value", &value_EpicsStore0, return_value_policy<return_by_value>())
    ;

  class_<EpicsPvHeaderWrapper>("EpicsPvHeader", no_init)
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

  psddl_python::createDeviceWrappers(module);
  createWrappersDone = true;
}

}
