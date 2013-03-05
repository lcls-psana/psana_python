//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class EpicsStore...
//
// Author List:
//      Andy Salnikov
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "psana_python/EpicsStore.h"

//-----------------
// C/C++ Headers --
//-----------------
#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "psddl_psana/epics.ddl.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

using boost::dynamic_pointer_cast;
using boost::python::object;


namespace {

  // convert vector of strings to python list of strings
  PyObject* nameList(const std::vector<std::string>& names);

  // type-specific methods
  PyObject* EpicsStore_names(PyObject* self, PyObject*);
  PyObject* EpicsStore_pvNames(PyObject* self, PyObject*);
  PyObject* EpicsStore_aliases(PyObject* self, PyObject*);
  PyObject* EpicsStore_alias(PyObject* self, PyObject* args);
  PyObject* EpicsStore_pvName(PyObject* self, PyObject* args);
  PyObject* EpicsStore_value(PyObject* self, PyObject* args);
  PyObject* EpicsStore_status(PyObject* self, PyObject* args);
  PyObject* EpicsStore_getPV(PyObject* self, PyObject* args);

  PyMethodDef methods[] = {
    { "names",     EpicsStore_names,       METH_NOARGS,
        "self.names() -> list of strings\n\nGet the full list of PV names and aliases, "
        "returned list includes the names of all PVs and aliases." },
    { "pvNames",   EpicsStore_pvNames,     METH_NOARGS,
        "self.pvNames() -> list of strings\n\nGet the list of PV names, "
        "returned list includes the names of all PVs but no alias names." },
    { "aliases",   EpicsStore_aliases,     METH_NOARGS,
        "self.aliases() -> list of strings\n\nGet the list of PV aliases, "
        "returned list includes the names of all alias names but no PV names." },
    { "alias",     EpicsStore_alias,       METH_VARARGS,
        "self.alias(pvName:string) -> string\n\nGet alias name for specified PV name. "
        "If specified PV is not found or does not have an alias an empty string is returned." },
    { "pvName",    EpicsStore_pvName,      METH_VARARGS,
        "self.pvName(alias:string) -> string\n\nGet PV name for specified alias name. "
        "If specified alias is not found an empty string is returned." },
    { "value",    EpicsStore_value,        METH_VARARGS,
        "self.value(name:string[, index:int]) -> number or string\n\nGet current value of PV. "
        "Returned type depends on PV type, for numeric or enum types integer or float is returned, "
        "for string types string is returned. Optional index specifies array index for array PVs, "
        "if index is out of range ``None`` is returned. Missing index means the same as index 0. "
        "If name is not found then ``None`` is returned." },
    { "status",    EpicsStore_status,      METH_VARARGS,
        "self.status(name:string) -> tuple\n\nGet status information for a given PV or alias name. "
        "Returns triplet (status, severity, time) corresponding to the last stored measurement. "
        "Time is returned as a floating number specifying seconds since UNIX Epoch. "
        "If name is not found ``None`` is returned." },
    { "getPV",    EpicsStore_getPV,        METH_VARARGS,
        "self.getPV(name:string) -> object\n\nFind EPICS PV given its PV or alias name. "
        "Returns an instance of one of the PV types (one of subclasses of :py:class:`Epics.EpicsPvHeader`). "
        "If PV name cannot be found ``None`` is returned." },
    {0, 0, 0, 0}
  };

  char typedoc[] = "Wrapper for C++ class PSEnv::EpicsStore. This class stores "
      "state of all EPICS corresponding to current event.";

}

//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------

void
psana_python::EpicsStore::initType(PyObject* module)
{
  PyTypeObject* type = BaseType::typeObject() ;
  type->tp_doc = ::typedoc;
  type->tp_methods = ::methods;

  BaseType::initType("EpicsStore", module, "psana");
}

namespace {

PyObject*
nameList(const std::vector<std::string>& names)
{
  PyObject* list = PyList_New(names.size());
  for (unsigned i = 0; i != names.size(); ++ i) {
    PyList_SET_ITEM(list, i, PyString_FromString(names[i].c_str()));
  }
  return list;
}

PyObject*
EpicsStore_names(PyObject* self, PyObject* )
{
  const boost::shared_ptr<PSEnv::EpicsStore>& cself = psana_python::EpicsStore::cppObject(self);
  return nameList(cself->names());
}

PyObject*
EpicsStore_pvNames(PyObject* self, PyObject* )
{
  const boost::shared_ptr<PSEnv::EpicsStore>& cself = psana_python::EpicsStore::cppObject(self);
  return nameList(cself->pvNames());
}

PyObject*
EpicsStore_aliases(PyObject* self, PyObject* )
{
  const boost::shared_ptr<PSEnv::EpicsStore>& cself = psana_python::EpicsStore::cppObject(self);
  return nameList(cself->aliases());
}

PyObject*
EpicsStore_alias(PyObject* self, PyObject* args)
{
  const boost::shared_ptr<PSEnv::EpicsStore>& cself = psana_python::EpicsStore::cppObject(self);

  const char* arg;
  if (not PyArg_ParseTuple( args, "s:EpicsStore.alias", &arg)) return 0;

  const std::string& res = cself->alias(arg);
  return PyString_FromString(res.c_str());
}

PyObject*
EpicsStore_pvName(PyObject* self, PyObject* args)
{
  const boost::shared_ptr<PSEnv::EpicsStore>& cself = psana_python::EpicsStore::cppObject(self);

  const char* arg;
  if (not PyArg_ParseTuple( args, "s:EpicsStore.pvName", &arg)) return 0;

  const std::string& res = cself->pvName(arg);
  return PyString_FromString(res.c_str());
}

PyObject*
EpicsStore_value(PyObject* self, PyObject* args)
{
  const boost::shared_ptr<PSEnv::EpicsStore>& cself = psana_python::EpicsStore::cppObject(self);

  const char* name;
  unsigned index = 0;
  if (not PyArg_ParseTuple( args, "s|I:EpicsStore.status", &name, &index)) return 0;

  // need to access internal details
  const PSEnv::EpicsStoreImpl impl = cself->internal_implementation();

  // find the channel object
  const boost::shared_ptr<Psana::Epics::EpicsPvHeader>& hdr = impl.getAny(name);
  if (not hdr) {
    Py_RETURN_NONE;
  }

  // check index
  if (index >= unsigned(hdr->numElements())) {
    Py_RETURN_NONE;
  }

  switch (hdr->dbrType()) {
  case Psana::Epics::DBR_TIME_STRING:
    return PyString_FromString((dynamic_pointer_cast<Psana::Epics::EpicsPvTimeString>(hdr))->value(index));
  case Psana::Epics::DBR_TIME_SHORT:
    return PyInt_FromLong((dynamic_pointer_cast<Psana::Epics::EpicsPvTimeShort>(hdr))->value(index));
  case Psana::Epics::DBR_TIME_FLOAT:
    return PyFloat_FromDouble((dynamic_pointer_cast<Psana::Epics::EpicsPvTimeFloat>(hdr))->value(index));
  case Psana::Epics::DBR_TIME_ENUM:
    return PyInt_FromLong((dynamic_pointer_cast<Psana::Epics::EpicsPvTimeEnum>(hdr))->value(index));
  case Psana::Epics::DBR_TIME_CHAR:
    return PyInt_FromLong((dynamic_pointer_cast<Psana::Epics::EpicsPvTimeChar>(hdr))->value(index));
  case Psana::Epics::DBR_TIME_LONG:
    return PyInt_FromLong((dynamic_pointer_cast<Psana::Epics::EpicsPvTimeLong>(hdr))->value(index));
  case Psana::Epics::DBR_TIME_DOUBLE:
    return PyFloat_FromDouble((dynamic_pointer_cast<Psana::Epics::EpicsPvTimeDouble>(hdr))->value(index));
  case Psana::Epics::DBR_CTRL_STRING:
    return PyString_FromString((dynamic_pointer_cast<Psana::Epics::EpicsPvCtrlString>(hdr))->value(index));
  case Psana::Epics::DBR_CTRL_SHORT:
    return PyInt_FromLong((dynamic_pointer_cast<Psana::Epics::EpicsPvCtrlShort>(hdr))->value(index));
  case Psana::Epics::DBR_CTRL_FLOAT:
    return PyFloat_FromDouble((dynamic_pointer_cast<Psana::Epics::EpicsPvCtrlFloat>(hdr))->value(index));
  case Psana::Epics::DBR_CTRL_ENUM:
    return PyInt_FromLong((dynamic_pointer_cast<Psana::Epics::EpicsPvCtrlEnum>(hdr))->value(index));
  case Psana::Epics::DBR_CTRL_CHAR:
    return PyInt_FromLong((dynamic_pointer_cast<Psana::Epics::EpicsPvCtrlChar>(hdr))->value(index));
  case Psana::Epics::DBR_CTRL_LONG:
    return PyInt_FromLong((dynamic_pointer_cast<Psana::Epics::EpicsPvCtrlLong>(hdr))->value(index));
  case Psana::Epics::DBR_CTRL_DOUBLE:
    return PyFloat_FromDouble((dynamic_pointer_cast<Psana::Epics::EpicsPvCtrlDouble>(hdr))->value(index));
  default:
    Py_RETURN_NONE;
  }
}

PyObject*
EpicsStore_status(PyObject* self, PyObject* args)
{
  const boost::shared_ptr<PSEnv::EpicsStore>& cself = psana_python::EpicsStore::cppObject(self);

  const char* arg;
  if (not PyArg_ParseTuple( args, "s:EpicsStore.status", &arg)) return 0;

  int status;
  int severity;
  PSTime::Time time;
  try {
    cself->status(arg, status, severity, time);
  } catch (const PSEnv::ExceptionEpicsName& ex) {
    Py_RETURN_NONE;
  }

  PyObject* tuple = PyTuple_New(3);
  PyTuple_SET_ITEM(tuple, 0, PyInt_FromLong(status));
  PyTuple_SET_ITEM(tuple, 1, PyInt_FromLong(severity));
  PyTuple_SET_ITEM(tuple, 2, PyFloat_FromDouble(time.sec() + time.nsec()*1e-9));

  return tuple;
}

PyObject*
EpicsStore_getPV(PyObject* self, PyObject* args)
{
  const boost::shared_ptr<PSEnv::EpicsStore>& cself = psana_python::EpicsStore::cppObject(self);

  const char* name;
  if (not PyArg_ParseTuple( args, "s:EpicsStore.status", &name)) return 0;

  // need to access internal details
  const PSEnv::EpicsStoreImpl impl = cself->internal_implementation();

  // find the channel object
  const boost::shared_ptr<Psana::Epics::EpicsPvHeader>& hdr = impl.getAny(name);
  if (not hdr) {
    Py_RETURN_NONE;
  }

  object res;
  switch (hdr->dbrType()) {
  case Psana::Epics::DBR_TIME_STRING:
    res = object(dynamic_pointer_cast<Psana::Epics::EpicsPvTimeString>(hdr));
    break;
  case Psana::Epics::DBR_TIME_SHORT:
    res = object(dynamic_pointer_cast<Psana::Epics::EpicsPvTimeShort>(hdr));
    break;
  case Psana::Epics::DBR_TIME_FLOAT:
    res = object(dynamic_pointer_cast<Psana::Epics::EpicsPvTimeFloat>(hdr));
    break;
  case Psana::Epics::DBR_TIME_ENUM:
    res = object(dynamic_pointer_cast<Psana::Epics::EpicsPvTimeEnum>(hdr));
    break;
  case Psana::Epics::DBR_TIME_CHAR:
    res = object(dynamic_pointer_cast<Psana::Epics::EpicsPvTimeChar>(hdr));
    break;
  case Psana::Epics::DBR_TIME_LONG:
    res = object(dynamic_pointer_cast<Psana::Epics::EpicsPvTimeLong>(hdr));
    break;
  case Psana::Epics::DBR_TIME_DOUBLE:
    res = object(dynamic_pointer_cast<Psana::Epics::EpicsPvTimeDouble>(hdr));
    break;
  case Psana::Epics::DBR_CTRL_STRING:
    res = object(dynamic_pointer_cast<Psana::Epics::EpicsPvCtrlString>(hdr));
    break;
  case Psana::Epics::DBR_CTRL_SHORT:
    res = object(dynamic_pointer_cast<Psana::Epics::EpicsPvCtrlShort>(hdr));
    break;
  case Psana::Epics::DBR_CTRL_FLOAT:
    res = object(dynamic_pointer_cast<Psana::Epics::EpicsPvCtrlFloat>(hdr));
    break;
  case Psana::Epics::DBR_CTRL_ENUM:
    res = object(dynamic_pointer_cast<Psana::Epics::EpicsPvCtrlEnum>(hdr));
    break;
  case Psana::Epics::DBR_CTRL_CHAR:
    res = object(dynamic_pointer_cast<Psana::Epics::EpicsPvCtrlChar>(hdr));
    break;
  case Psana::Epics::DBR_CTRL_LONG:
    res = object(dynamic_pointer_cast<Psana::Epics::EpicsPvCtrlLong>(hdr));
    break;
  case Psana::Epics::DBR_CTRL_DOUBLE:
    res = object(dynamic_pointer_cast<Psana::Epics::EpicsPvCtrlDouble>(hdr));
    break;
  default:
    break;
  }

  Py_INCREF(res.ptr());
  return res.ptr();
}

}
