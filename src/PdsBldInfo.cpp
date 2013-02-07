//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class PdsBldInfo...
//
// Author List:
//      Andy Salnikov
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "psana_python/PdsBldInfo.h"

//-----------------
// C/C++ Headers --
//-----------------
#include <algorithm>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "psana_python/PdsSrc.h"
#include "pytools/EnumType.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

namespace {

  pytools::EnumType typeEnum("Type");

  // standard Python stuff
  PyObject* PdsBldInfo_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds);

  // type-specific methods
  PyObject* PdsBldInfo_processId(PyObject* self, PyObject*);
  PyObject* PdsBldInfo_type(PyObject* self, PyObject*);
  PyObject* PdsBldInfo_typeName(PyObject* self, PyObject*);

  PyMethodDef methods[] = {
    { "processId",   PdsBldInfo_processId,  METH_NOARGS, "self.processId() -> int\n\nReturns process ID number." },
    { "type",        PdsBldInfo_type,       METH_NOARGS, "self.type() -> int\n\nReturns detector type." },
    { "typeName",    PdsBldInfo_typeName,   METH_NOARGS, "self.typeName() -> string\n\nReturns detector name." },
    {0, 0, 0, 0}
   };

  char typedoc[] = "Class which defines addresses for BLD devices.\n"
      "Constructor can take either one or two arguments, both of them "
      "should be integer numbers. With two arguments they are "
      "(process_id, type), one argument is just type.";

}

//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------

void
psana_python::PdsBldInfo::initType(PyObject* module)
{
  PyTypeObject* type = BaseType::typeObject() ;
  type->tp_doc = ::typedoc;
  type->tp_methods = ::methods;
  type->tp_new = ::PdsBldInfo_new;
  type->tp_base = psana_python::PdsSrc::typeObject();
  Py_INCREF(type->tp_base);

  // Generate constants for C++ enum values.
  // Note that names of the constants are not the same as
  // names of corresponding C++ enums.
  for (int i = 0; i <= Pds::BldInfo::NumberOf; ++ i) {
    std::string name = i == Pds::BldInfo::NumberOf ? "NumberOf" : Pds::BldInfo::name(Pds::BldInfo(0, Pds::BldInfo::Type(i)));
    // replace special characters with underscores
    std::replace(name.begin(), name.end(), '-', '_');

    ::typeEnum.addEnum(name, i);
  }

  type->tp_dict = PyDict_New();
  PyDict_SetItemString(type->tp_dict, ::typeEnum.typeName(), ::typeEnum.type());

  BaseType::initType("BldInfo", module, "psana");
}

namespace {

PyObject*
PdsBldInfo_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds)
{
  // parse arguments
  unsigned processId=0, type;
  if (PyTuple_GET_SIZE(args) == 1) {
    if ( not PyArg_ParseTuple( args, "I:BldInfo", &type ) ) {
      return 0;
    }
  } else if (PyTuple_GET_SIZE(args) == 2) {
    if ( not PyArg_ParseTuple( args, "II:BldInfo", &processId, &type ) ) {
      return 0;
    }
  } else {
    PyErr_SetString(PyExc_TypeError, "BldInfo(): one or two arguments are required");
    return 0;
  }
  if ( type >= Pds::BldInfo::NumberOf ) {
    PyErr_SetString(PyExc_ValueError, "Error: detector type out of range");
    return 0;
  }

  PyObject* self = subtype->tp_alloc(subtype, 1);
  psana_python::PdsBldInfo* py_this = (psana_python::PdsBldInfo*) self;

  new(&py_this->m_obj) Pds::BldInfo(processId, Pds::BldInfo::Type(type));

  return self;
}

PyObject*
PdsBldInfo_processId(PyObject* self, PyObject* )
{
  Pds::BldInfo& cself = psana_python::PdsBldInfo::cppObject(self);
  return PyInt_FromLong(cself.processId());
}

PyObject*
PdsBldInfo_type(PyObject* self, PyObject* )
{
  Pds::BldInfo& cself = psana_python::PdsBldInfo::cppObject(self);
  return PyInt_FromLong(cself.type());
}

PyObject*
PdsBldInfo_typeName(PyObject* self, PyObject* )
{
  Pds::BldInfo& cself = psana_python::PdsBldInfo::cppObject(self);
  return PyString_FromString(Pds::BldInfo::name(cself));
}

}
