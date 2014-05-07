//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class PdsClockTime...
//
// Author List:
//      David Schneider
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "psana_python/PdsClockTime.h"

//-----------------
// C/C++ Headers --
//-----------------

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

namespace {

  // standard Python stuff
  PyObject* PdsClockTime_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds);

  // type-specific methods
  PyObject* PdsClockTime_seconds(PyObject* self, PyObject*);
  PyObject* PdsClockTime_nanoseconds(PyObject* self, PyObject*);
  PyObject* PdsClockTime_asDouble(PyObject* self, PyObject*);
  PyObject* PdsClockTime_isZero(PyObject* self, PyObject*);

  PyMethodDef methods[] = {
    { "seconds", PdsClockTime_seconds, METH_NOARGS, "self.seconds() -> int\n\nReturns seconds part of time (since Jan 1, 1970)" },
    { "nanoseconds", PdsClockTime_nanoseconds, METH_NOARGS, "self.nanoseconds() -> int\n\nReturns nanoseconds part of time." },
    { "asDouble",    PdsClockTime_asDouble, METH_NOARGS, "self.asDouble() -> float\n\nReturns time in seconds (since Jan 1, 1970)" },
    { "isZero",    PdsClockTime_isZero, METH_NOARGS, "self.isZero() -> string\n\nReturns true if time is 0." },
    {0, 0, 0, 0}
   };

  char typedoc[] = "Class which defines clock time.\n"
    "Uses two 32 bit unsigned integers for seconds since Jan 1, 1970 "
    "and nanoseconds.";

}

//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------

void
psana_python::PdsClockTime::initType(PyObject* module)
{
  PyTypeObject* type = BaseType::typeObject() ;
  type->tp_doc = ::typedoc;
  type->tp_methods = ::methods;

  BaseType::initType("ClockTime", module, "psana");
}

namespace {

PyObject*
PdsClockTime_seconds(PyObject* self, PyObject* )
{
  Pds::ClockTime& cself = psana_python::PdsClockTime::cppObject(self);
  return PyInt_FromLong(cself.seconds());
}

PyObject*
PdsClockTime_nanoseconds(PyObject* self, PyObject* )
{
  Pds::ClockTime& cself = psana_python::PdsClockTime::cppObject(self);
  return PyInt_FromLong(cself.nanoseconds());
}

PyObject*
PdsClockTime_asDouble(PyObject* self, PyObject* )
{
  Pds::ClockTime& cself = psana_python::PdsClockTime::cppObject(self);
  return PyFloat_FromDouble(cself.asDouble());
}

PyObject*
PdsClockTime_isZero(PyObject* self, PyObject* )
{
  Pds::ClockTime& cself = psana_python::PdsClockTime::cppObject(self);
  return PyBool_FromLong(cself.isZero());
}

}
