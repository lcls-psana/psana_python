////////////////////////////////////////////////////////////////////////////////
//
// XXX TO DO:
//
// Python wrappers should use attributes instead of functions
// e.g. ConfigV1.pvControls[i] instead of ConfigV1.pvControls()[i]
//
//
// http://www.boost.org/doc/libs/1_42_0/libs/python/doc/tutorial/doc/html/python/exposing.html
//
// "However, in Python attribute access is fine; it doesn't
//  neccessarily break encapsulation to let users handle
//  attributes directly, because the attributes can just be
//  a different syntax for a method call. Wrapping our
//  Num class using Boost.Python:"
//
// class_<Num>("Num")
//    .add_property("rovalue", &Num::get)
//    .add_property("value", &Num::get, &Num::set);
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
#include "PSEnv/EpicsStoreImpl.h"
#include <ConfigSvc/ConfigSvc.h>
#include <psana_python/CreateDeviceWrappers.h>
#include <psana_python/EnvWrapper.h>
#include <psana_python/EventWrapper.h>

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
using Psana::Epics::EpicsPvHeader;

#define std_vector_class_(T)\
  class_<vector<T> >("std::vector<" #T ">")\
    .def(vector_indexing_suite_nocopy<vector<T> >())

namespace Psana {
  object EventWrapperClass;
  object EnvWrapperClass;
  static bool createWrappersDone = false;

  class EpicsPvHeaderWrapper {
  private:
    shared_ptr<EpicsPvHeader> _header;
  public:
    EpicsPvHeaderWrapper(shared_ptr<EpicsPvHeader> header) : _header(header) {}
    int pvId() { return _header->pvId(); }
    int dbrType() { return _header->dbrType(); }
    int numElements() { return _header->numElements(); }
    void print() { _header->print(); }
    int isCtrl() { return _header->isCtrl(); }
    int isTime() { return _header->isTime(); }
    int severity() { return _header->severity(); }
    int __zero() { return 0; }
  };

  object fail_EpicsStore(const char* typeName, int typeId) {
    fprintf(stderr, "Unrecognized type %s (%d)\n", typeName, typeId);
    return object();
  }

  EpicsPvHeaderWrapper value_EpicsStore(EpicsStore& epicsStore, const std::string& name, int index) {
    EpicsStore::EpicsValue value(epicsStore.value(name, index));
    PSEnv::EpicsStoreImpl* m_impl = value.m_impl;
    return EpicsPvHeaderWrapper(m_impl->getAny(name));
  }

  EpicsPvHeaderWrapper value_EpicsStore0(EpicsStore& epicsStore, const std::string& name) {
    return value_EpicsStore(epicsStore, name, 0);
  }

  object value_EpicsStore_KEEP(EpicsStore& epicsStore, const std::string& name, int index = 0) {
    EpicsStore::EpicsValue value(epicsStore.value(name, index));
    PSEnv::EpicsStoreImpl* m_impl = value.m_impl;
    shared_ptr<EpicsPvHeader> pv = m_impl->getAny(name);
    if (not pv.get()) {
      return object();
    }
    const EpicsPvHeader* p = pv.get();
    int type = p->dbrType();
    switch (type) {
      case Epics::DBR_STRING:
        return fail_EpicsStore("DBR_STRING", type);
      case Epics::DBR_SHORT:
        return fail_EpicsStore("DBR_SHORT", type);
      case Epics::DBR_FLOAT:
        return fail_EpicsStore("DBR_FLOAT", type);
      case Epics::DBR_ENUM:
        return fail_EpicsStore("DBR_ENUM", type);
      case Epics::DBR_CHAR:
        return fail_EpicsStore("DBR_CHAR", type);
      case Epics::DBR_LONG:
        return fail_EpicsStore("DBR_LONG", type);
      case Epics::DBR_DOUBLE:
        return fail_EpicsStore("DBR_DOUBLE", type);
      case Epics::DBR_STS_STRING:
        return fail_EpicsStore("DBR_STS_STRING", type);
      case Epics::DBR_STS_SHORT:
        return fail_EpicsStore("DBR_STS_SHORT", type);
      case Epics::DBR_STS_FLOAT:
        return fail_EpicsStore("DBR_STS_FLOAT", type);
      case Epics::DBR_STS_ENUM:
        return fail_EpicsStore("DBR_STS_ENUM", type);
      case Epics::DBR_STS_CHAR:
        return fail_EpicsStore("DBR_STS_CHAR", type);
      case Epics::DBR_STS_LONG:
        return fail_EpicsStore("DBR_STS_LONG", type);
      case Epics::DBR_STS_DOUBLE:
        return fail_EpicsStore("DBR_STS_DOUBLE", type);
      case Epics::DBR_TIME_STRING:
        return object(((Epics::EpicsPvTimeString *) p)->value(index));
      case Epics::DBR_TIME_SHORT:
        return object(((Epics::EpicsPvTimeShort *) p)->value(index));
      case Epics::DBR_TIME_FLOAT:
        return object(((Epics::EpicsPvTimeFloat *) p)->value(index));
      case Epics::DBR_TIME_ENUM:
        return object(((Epics::EpicsPvTimeEnum *) p)->value(index));
      case Epics::DBR_TIME_CHAR:
        return object(((Epics::EpicsPvTimeChar *) p)->value(index));
      case Epics::DBR_TIME_LONG:
        return object(((Epics::EpicsPvTimeLong *) p)->value(index));
      case Epics::DBR_TIME_DOUBLE:
        return object(((Epics::EpicsPvTimeDouble *) p)->value(index));
      case Epics::DBR_GR_STRING:
        return fail_EpicsStore("DBR_GR_STRING", type);
      case Epics::DBR_GR_SHORT:
        return fail_EpicsStore("DBR_GR_SHORT", type);
      case Epics::DBR_GR_FLOAT:
        return fail_EpicsStore("DBR_GR_FLOAT", type);
      case Epics::DBR_GR_ENUM:
        return fail_EpicsStore("DBR_GR_ENUM", type);
      case Epics::DBR_GR_CHAR:
        return fail_EpicsStore("DBR_GR_CHAR", type);
      case Epics::DBR_GR_LONG:
        return fail_EpicsStore("DBR_GR_LONG", type);
      case Epics::DBR_GR_DOUBLE:
        return fail_EpicsStore("DBR_GR_DOUBLE", type);
      case Epics::DBR_CTRL_STRING:
        return object(((Epics::EpicsPvCtrlString *) p)->value(index));
      case Epics::DBR_CTRL_SHORT:
        return object(((Epics::EpicsPvCtrlShort *) p)->value(index));
      case Epics::DBR_CTRL_FLOAT:
        return object(((Epics::EpicsPvCtrlFloat *) p)->value(index));
      case Epics::DBR_CTRL_ENUM:
        return object(((Epics::EpicsPvCtrlEnum *) p)->value(index));
      case Epics::DBR_CTRL_CHAR:
        return object(((Epics::EpicsPvCtrlChar *) p)->value(index));
      case Epics::DBR_CTRL_LONG:
        return object(((Epics::EpicsPvCtrlLong *) p)->value(index));
      case Epics::DBR_CTRL_DOUBLE:
        return object(((Epics::EpicsPvCtrlDouble *) p)->value(index));
      default:
        return fail_EpicsStore("???", type);
    }
  }

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
      .def("value", &value_EpicsStore, return_value_policy<return_by_value>())
      .def("value", &value_EpicsStore0, return_value_policy<return_by_value>())
      //      .def("value", &PSEnv::EpicsStore::value)
      ;

    class_<PSEnv::EpicsStore::EpicsValue>("PSEnv::EpicsStore::EpicsValue", no_init)
      ;

    class_<Psana::EpicsPvHeaderWrapper>("Psana::Epics::EpicsPvHeader", no_init)
      .def("pvId", &EpicsPvHeaderWrapper::pvId)
      .def("dbrType", &EpicsPvHeaderWrapper::dbrType)
      .def("numElements", &EpicsPvHeaderWrapper::numElements)
      .def("print", &EpicsPvHeaderWrapper::print)
      .def("isCtrl", &EpicsPvHeaderWrapper::isCtrl)
      .def("isTime", &EpicsPvHeaderWrapper::isTime)
      .def("severity", &EpicsPvHeaderWrapper::severity)
      .def("status", &EpicsPvHeaderWrapper::__zero)
      .def("precision", &EpicsPvHeaderWrapper::__zero)
      .def("units", &EpicsPvHeaderWrapper::__zero)
      .add_property("lower_ctrl_limit", &EpicsPvHeaderWrapper::__zero)
      .add_property("upper_ctrl_limit", &EpicsPvHeaderWrapper::__zero)
      .add_property("lower_disp_limit", &EpicsPvHeaderWrapper::__zero)
      .add_property("upper_disp_limit", &EpicsPvHeaderWrapper::__zero)
      .add_property("lower_warning_limit", &EpicsPvHeaderWrapper::__zero)
      .add_property("upper_warning_limit", &EpicsPvHeaderWrapper::__zero)
      .add_property("lower_alarm_limit", &EpicsPvHeaderWrapper::__zero)
      .add_property("upper_alarm_limit", &EpicsPvHeaderWrapper::__zero)
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

