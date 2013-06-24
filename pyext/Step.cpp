//--------------------------------------------------------------------------
// File and Version Information:
//  $Id$
//
// Description:
//  Class Step...
//
// Author List:
//      Andrei Salnikov
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "Step.h"

//-----------------
// C/C++ Headers --
//-----------------

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "EventIter.h"
#include "psana_python/Env.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

namespace {

  // type-specific methods
  PyObject* Step_events(PyObject* self, PyObject*);
  PyObject* Step_env(PyObject* self, PyObject*);
  PyObject* Step_nonzero(PyObject* self, PyObject*);

  PyMethodDef methods[] = {
    { "events",      Step_events,    METH_NOARGS, "self.events() -> iterator\n\nReturns iterator for contained events (:py:class:`EventIter`)" },
    { "env",         Step_env,       METH_NOARGS, "self.env() -> object\n\nReturns environment object" },
    { "__nonzero__", Step_nonzero,   METH_NOARGS, "self.__nonzero__() -> bool\n\nReturns true for non-null object" },
    {0, 0, 0, 0}
   };

  char typedoc[] = "Python wrapper for psana Step type. Step type represents data originating "
      "from a single step (calib cycle) which contains events. This class provides way to "
      "iterate over individual  events contained in a Step. Actual iteration is implemented in "
      ":py:class:`EventIter` class, this class serves as a factory for iterator instances.";

}

//    ----------------------------------------
//    -- Public Function Member Definitions --
//    ----------------------------------------

void
psana_python::pyext::Step::initType(PyObject* module)
{
  PyTypeObject* type = BaseType::typeObject() ;
  type->tp_doc = ::typedoc;
  type->tp_methods = ::methods;

  BaseType::initType("Step", module, "psana");
}

namespace {

PyObject*
Step_events(PyObject* self, PyObject* )
{
  psana_python::pyext::Step* py_this = static_cast<psana_python::pyext::Step*>(self);
  return psana_python::pyext::EventIter::PyObject_FromCpp(py_this->m_obj.events());
}

PyObject*
Step_env(PyObject* self, PyObject* )
{
  psana_python::pyext::Step* py_this = static_cast<psana_python::pyext::Step*>(self);
  PSEnv::Env& env = py_this->m_obj.env();
  return psana_python::Env::PyObject_FromCpp(env.shared_from_this());
}

PyObject*
Step_nonzero(PyObject* self, PyObject* )
{
  psana_python::pyext::Step* py_this = static_cast<psana_python::pyext::Step*>(self);
  return PyBool_FromLong(long(bool(py_this->m_obj)));
}

}
