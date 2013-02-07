//--------------------------------------------------------------------------
// File and Version Information:
//  $Id: Env.cpp 4455 2012-09-12 00:22:58Z salnikov@SLAC.STANFORD.EDU $
//
// Description:
//  Class Env...
//
// Author List:
//      Andrei Salnikov
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "psana_python/Env.h"

//-----------------
// C/C++ Headers --
//-----------------
#include <boost/python.hpp>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "psana_python/EnvObjectStore.h"
#include "pytools/make_pyshared.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

using namespace psana_python;

namespace {

  // type-specific methods
  PyObject* Env_jobName(PyObject* self, PyObject*);
  PyObject* Env_instrument(PyObject* self, PyObject*);
  PyObject* Env_experiment(PyObject* self, PyObject*);
  PyObject* Env_expNum(PyObject* self, PyObject*);
  PyObject* Env_calibDir(PyObject* self, PyObject*);
  PyObject* Env_configStore(PyObject* self, PyObject*);
  PyObject* Env_calibStore(PyObject* self, PyObject*);
  PyObject* Env_epicsStore(PyObject* self, PyObject*);
  PyObject* Env_hmgr(PyObject* self, PyObject*);
  PyObject* Env_assert_psana(PyObject* self, PyObject*);
  PyObject* Env_subprocess(PyObject* self, PyObject*);
  PyObject* Env_getConfig(PyObject* self, PyObject* args);

  PyMethodDef methods[] = {
    { "jobName",       Env_jobName,       METH_NOARGS, "self.jobName() -> str\n\nReturns job name."},
    { "instrument",    Env_instrument,    METH_NOARGS, "self.instrument() -> str\n\nReturns instrument name"},
    { "experiment",    Env_experiment,    METH_NOARGS, "self.experiment() -> str\n\nReturns experiment name"},
    { "expNum",        Env_expNum,        METH_NOARGS, "self.expNum() -> int\n\nReturns experiment number or 0"},
    { "calibDir",      Env_calibDir,      METH_NOARGS, 
        "self.calibDir() -> str\n\nReturns that name of the calibration directory for current instrument/experiment"},
    { "configStore",   Env_configStore,   METH_NOARGS, "self.configStore() -> object\n\nAccess to Configuration Store object."},
    { "calibStore",    Env_calibStore,    METH_NOARGS, "self.calibStore() -> object\n\nAccess to Calibration Store object."},
    { "epicsStore",    Env_epicsStore,    METH_NOARGS, "self.epicsStore() -> object\n\nAccess to EPICS Store object."},
    { "hmgr",          Env_hmgr,          METH_NOARGS, "self.hmgr() -> object\n\nAccess to histogram manager."},
    { "assert_psana",  Env_assert_psana,  METH_NOARGS, "self.assert_psana() -> int\n\nReturns 1 if running inside psana."},
    { "subprocess",    Env_subprocess,    METH_NOARGS, "self.subprocess() -> int\n\nReturns subprocess number or 0 if running inside main process."},
    { "getConfig",     Env_getConfig,     METH_VARARGS, 
        "self.getConfig(...) -> object\n\nPyana compatibility method, shortcut for self.configStore().get(), deprecated."},
    {0, 0, 0, 0}
   };

  char typedoc[] = "\
Python wrapper for psana Env class. Instances of this class are \
created by the framework itself and returned to the client from framework methods.\
";

}

//    ----------------------------------------
//    -- Public Function Member Definitions --
//    ----------------------------------------

void
psana_python::Env::initType(PyObject* module)
{
  PyTypeObject* type = BaseType::typeObject() ;
  type->tp_doc = ::typedoc;
  type->tp_methods = ::methods;

  BaseType::initType("Env", module, "psana");
}

// Dump object info to a stream
void 
psana_python::Env::print(std::ostream& out) const
{
  out << "psana.Env()" ;
}

namespace {

PyObject*
Env_jobName(PyObject* self, PyObject*)
{
  boost::shared_ptr<PSEnv::Env>& cself = Env::cppObject(self);
  return PyString_FromString(cself->jobName().c_str());
}

PyObject*
Env_instrument(PyObject* self, PyObject*)
{
  boost::shared_ptr<PSEnv::Env>& cself = Env::cppObject(self);
  return PyString_FromString(cself->instrument().c_str());
}

PyObject*
Env_experiment(PyObject* self, PyObject*)
{
  boost::shared_ptr<PSEnv::Env>& cself = Env::cppObject(self);
  return PyString_FromString(cself->experiment().c_str());
}

PyObject*
Env_expNum(PyObject* self, PyObject*)
{
  boost::shared_ptr<PSEnv::Env>& cself = Env::cppObject(self);
  return PyInt_FromLong(cself->expNum());
}

PyObject*
Env_calibDir(PyObject* self, PyObject*)
{
  boost::shared_ptr<PSEnv::Env>& cself = Env::cppObject(self);
  return PyString_FromString(cself->calibDir().c_str());
}

PyObject*
Env_configStore(PyObject* self, PyObject*)
{
  boost::shared_ptr<PSEnv::Env>& cself = Env::cppObject(self);
  return EnvObjectStore::PyObject_FromCpp(cself->configStore().shared_from_this());
}

PyObject*
Env_calibStore(PyObject* self, PyObject*)
{
  boost::shared_ptr<PSEnv::Env>& cself = Env::cppObject(self);
  return EnvObjectStore::PyObject_FromCpp(cself->calibStore().shared_from_this());
}

PyObject*
Env_epicsStore(PyObject* self, PyObject*)
{
  boost::shared_ptr<PSEnv::Env>& cself = Env::cppObject(self);
  boost::python::object boo(cself->epicsStore());
  Py_INCREF(boo.ptr());
  return boo.ptr();
}

PyObject*
Env_hmgr(PyObject* self, PyObject*)
{
  boost::shared_ptr<PSEnv::Env>& cself = Env::cppObject(self);
  boost::python::object boo(cself->hmgr());
  Py_INCREF(boo.ptr());
  return boo.ptr();
}

PyObject*
Env_assert_psana(PyObject* self, PyObject*)
{
  return PyInt_FromLong(1);
}

PyObject*
Env_subprocess(PyObject* self, PyObject*)
{
  return PyInt_FromLong(0);
}

PyObject* 
Env_getConfig(PyObject* self, PyObject* args)
{
  // forward call to the config store method
  pytools::pyshared_ptr cfgStore = pytools::make_pyshared(Env_configStore(self, 0));
  pytools::pyshared_ptr method = pytools::make_pyshared(PyObject_GetAttrString(cfgStore.get(), "get"));
  return PyObject_CallObject(method.get(), args);
}

}
