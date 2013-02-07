//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class PdsProcInfo...
//
// Author List:
//      Andy Salnikov
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "psana_python/PdsProcInfo.h"

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
PyObject* PdsProcInfo_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds);

  // type-specific methods
  PyObject* PdsProcInfo_processId(PyObject* self, PyObject*);
  PyObject* PdsProcInfo_ipAddr(PyObject* self, PyObject*);

  PyMethodDef methods[] = {
    { "processId",   PdsProcInfo_processId,  METH_NOARGS, "self.processId() -> int\n\nReturns process ID number." },
    { "ipAddr",      PdsProcInfo_ipAddr,     METH_NOARGS, "self.ipAddr() -> int\n\nReturns IP address as integer number." },
    {0, 0, 0, 0}
   };

  char typedoc[] = "Class which defines detector-level addresses.\n"
      "Constructor takes three arguments, all of them "
      "should be integer numbers: level, process ID, and IP address.";

}

//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------

void
psana_python::PdsProcInfo::initType(PyObject* module)
{
  PyTypeObject* type = BaseType::typeObject() ;
  type->tp_doc = ::typedoc;
  type->tp_methods = ::methods;
  type->tp_new = ::PdsProcInfo_new;
  type->tp_base = psana_python::PdsSrc::typeObject();
  Py_INCREF(type->tp_base);

  BaseType::initType("ProcInfo", module, "psana");
}

namespace {

PyObject*
PdsProcInfo_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds)
{
  // parse arguments
  unsigned level, processId, ipAddr;
  if ( not PyArg_ParseTuple( args, "III:ProcInfo", &level, &processId, &ipAddr ) ) {
    return 0;
  }
  if ( level >= Pds::Level::NumberOfLevels ) {
    PyErr_SetString(PyExc_ValueError, "Error: level number out of range");
    return 0;
  }

  PyObject* self = subtype->tp_alloc(subtype, 1);
  psana_python::PdsProcInfo* py_this = (psana_python::PdsProcInfo*) self;

  new(&py_this->m_obj) Pds::ProcInfo(Pds::Level::Type(level), processId, ipAddr);

  return 0;
}

PyObject*
PdsProcInfo_processId(PyObject* self, PyObject* )
{
  Pds::ProcInfo& cself = psana_python::PdsProcInfo::cppObject(self);
  return PyInt_FromLong(cself.processId());
}

PyObject*
PdsProcInfo_ipAddr(PyObject* self, PyObject* )
{
  Pds::ProcInfo& cself = psana_python::PdsProcInfo::cppObject(self);
  return PyInt_FromLong(cself.ipAddr());
}

}
