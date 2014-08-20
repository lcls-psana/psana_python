//--------------------------------------------------------------------------
// File and Version Information:
//  $Id: EventTime.cpp 8179 2014-05-08 00:36:49Z cpo@SLAC.STANFORD.EDU $
//
// Description:
//  Class EventTime...
//
// Author List:
//      Christopher O'Grady
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "EventTime.h"

//-----------------
// C/C++ Headers --
//-----------------
#include <vector>
#include <boost/cstdint.hpp>
#include <boost/python/object.hpp>
#include "psddl_python/psddl_python_numpy.h"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "psana/Index.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

namespace {

  // type-specific methods
  int EventTime_init(PyObject* self, PyObject* args, PyObject* kwds);
  PyObject* EventTime_reduce(PyObject* self, PyObject*);
  PyObject* EventTime_fiducial(PyObject* self, PyObject*);
  PyObject* EventTime_time(PyObject* self, PyObject*);
  PyObject* EventTime_nanoseconds(PyObject* self, PyObject*);
  PyObject* EventTime_seconds(PyObject* self, PyObject*);

  PyMethodDef methods[] = {
    { "__reduce__",  EventTime_reduce,      METH_NOARGS, "self.__reduce__() -> Tuple\n\nReturns tuple relevent for EventTime pickling" },
    { "fiducial",    EventTime_fiducial,    METH_NOARGS, "self.fiducial() -> int\n\nReturns fiducial count for this timestamp" },
    { "time",        EventTime_time,        METH_NOARGS, "self.time() -> int\n\nReturns time count for this timestamp" },
    { "nanoseconds", EventTime_nanoseconds, METH_NOARGS, "self.nanoseconds() -> int\n\nReturns nanoseconds count for this timestamp" },
    { "seconds",     EventTime_seconds,     METH_NOARGS, "self.seconds() -> int\n\nReturns seconds count for this timestamp" },
    {0, 0, 0, 0}
   };

  char typedoc[] = "Python wrapper for psana EventTime type";
}

//    ----------------------------------------
//    -- Public Function Member Definitions --
//    ----------------------------------------

void
psana_python::pyext::EventTime::initType(PyObject* module)
{
  PyTypeObject* type = BaseType::typeObject() ;
  type->tp_doc = ::typedoc;
  type->tp_methods = ::methods;
  type->tp_new = PyType_GenericNew;
  type->tp_init = ::EventTime_init;

  BaseType::initType("EventTime", module, "psana");
}

namespace {

int EventTime_init(PyObject* self, PyObject* args, PyObject* kwds)
{
  // copy-construct the object
  psana_python::pyext::EventTime* py_this = static_cast<psana_python::pyext::EventTime*>(self);
  uint32_t fiducial;
  uint64_t time;
  PyArg_ParseTuple(args,"KI",&time,&fiducial);
  new(&(py_this->m_obj)) psana::EventTime(time,fiducial);
  return 0;
}

PyObject*
EventTime_fiducial(PyObject* self, PyObject* )
{
  psana_python::pyext::EventTime* py_this = static_cast<psana_python::pyext::EventTime*>(self);
  uint32_t fiducial = py_this->m_obj.fiducial();
  return Py_BuildValue("I",fiducial);
}

PyObject*
EventTime_reduce(PyObject* self, PyObject* )
{
  psana_python::pyext::EventTime* py_this = static_cast<psana_python::pyext::EventTime*>(self);
  uint32_t fiducial = py_this->m_obj.fiducial();
  uint64_t time = py_this->m_obj.time();

  PyObject *pTuple = PyTuple_New(2);
  PyTuple_SetItem(pTuple, 0, PyObject_Type(self));
  PyTuple_SetItem(pTuple, 1, Py_BuildValue("KI",time,fiducial));
  return pTuple;
}

PyObject*
EventTime_time(PyObject* self, PyObject* )
{
  psana_python::pyext::EventTime* py_this = static_cast<psana_python::pyext::EventTime*>(self);
  uint64_t time = py_this->m_obj.time();
  return Py_BuildValue("K",time);
}

PyObject*
EventTime_seconds(PyObject* self, PyObject* )
{
  psana_python::pyext::EventTime* py_this = static_cast<psana_python::pyext::EventTime*>(self);
  uint32_t seconds = py_this->m_obj.seconds();
  return Py_BuildValue("I",seconds);
}

PyObject*
EventTime_nanoseconds(PyObject* self, PyObject* )
{
  psana_python::pyext::EventTime* py_this = static_cast<psana_python::pyext::EventTime*>(self);
  uint32_t nanoseconds = py_this->m_obj.nanoseconds();
  return Py_BuildValue("I",nanoseconds);
}

}
