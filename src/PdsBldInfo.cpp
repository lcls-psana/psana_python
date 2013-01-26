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

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "psana_python/PdsSrc.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

namespace {

  // standard Python stuff
  int PdsBldInfo_init( PyObject* self, PyObject* args, PyObject* kwds );

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
  type->tp_init = ::PdsBldInfo_init;
  type->tp_base = psana_python::PdsSrc::typeObject();
  Py_INCREF(type->tp_base);

  BaseType::initType("BldInfo", module);
}

namespace {

int
PdsBldInfo_init( PyObject* self, PyObject* args, PyObject* kwds )
{
  // parse arguments
  unsigned processId=0, type;
  if (PyTuple_GET_SIZE(args) == 1) {
    if ( not PyArg_ParseTuple( args, "I:BldInfo", &type ) ) {
      return -1;
    }
  } else {
    if ( not PyArg_ParseTuple( args, "II:BldInfo", &processId, &type ) ) {
      return -1;
    }
  }
  if ( type >= Pds::BldInfo::NumberOf ) {
    PyErr_SetString(PyExc_ValueError, "Error: detector type out of range");
    return -1;
  }

  psana_python::PdsBldInfo* py_this = (psana_python::PdsBldInfo*) self;
  if ( not py_this ) {
    PyErr_SetString(PyExc_RuntimeError, "Error: self is NULL");
    return -1;
  }

  new(&py_this->m_obj) Pds::BldInfo(processId, Pds::BldInfo::Type(type));

  return 0;
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
