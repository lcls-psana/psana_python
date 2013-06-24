//--------------------------------------------------------------------------
// File and Version Information:
//  $Id$
//
// Description:
//  Class StepIter...
//
// Author List:
//      Andrei Salnikov
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "StepIter.h"

//-----------------
// C/C++ Headers --
//-----------------
#include <exception>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "Step.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

namespace {

  // type-specific methods
  PyObject* StepIter_iter(PyObject* self);
  PyObject* StepIter_iternext(PyObject* self);

  char typedoc[] = "Class which supports iteration over steps (calib cycles) contained in a "
      "particular :py:class:`DataSource` or :py:class:`Run` instance. Iterator returns instances "
      "of :py:class:`Step` class.";

}

//    ----------------------------------------
//    -- Public Function Member Definitions --
//    ----------------------------------------

void
psana_python::pyext::StepIter::initType(PyObject* module)
{
  PyTypeObject* type = BaseType::typeObject() ;
  type->tp_doc = ::typedoc;
  type->tp_iter = StepIter_iter;
  type->tp_iternext = StepIter_iternext;

  BaseType::initType("StepIter", module, "psana");
}

namespace {

PyObject*
StepIter_iter(PyObject* self)
{
  Py_XINCREF(self);
  return self;
}

PyObject*
StepIter_iternext(PyObject* self)
try {
  psana_python::pyext::StepIter* py_this = static_cast<psana_python::pyext::StepIter*>(self);
  psana::Step step = py_this->m_obj.next();
  if (step) {
    return psana_python::pyext::Step::PyObject_FromCpp(step);
  } else {
    // stop iteration
    PyErr_SetNone( PyExc_StopIteration );
    return 0;
  }
} catch (const std::exception& ex) {
  PyErr_SetString(PyExc_RuntimeError, ex.what());
  return 0;
}

}
