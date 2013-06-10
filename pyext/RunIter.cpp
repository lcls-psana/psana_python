//--------------------------------------------------------------------------
// File and Version Information:
//  $Id$
//
// Description:
//  Class RunIter...
//
// Author List:
//      Andrei Salnikov
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "RunIter.h"

//-----------------
// C/C++ Headers --
//-----------------
#include <exception>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "Run.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

namespace {

  // type-specific methods
  PyObject* RunIter_iter(PyObject* self);
  PyObject* RunIter_iternext(PyObject* self);

  char typedoc[] = "Class which supports iteration over runs contained in a "
      "particular :py:class:`DataSource` instance. Iterator returns instances "
      "of :py:class:`Run` class.";

}

//    ----------------------------------------
//    -- Public Function Member Definitions --
//    ----------------------------------------

void
psana_python::pyext::RunIter::initType(PyObject* module)
{
  PyTypeObject* type = BaseType::typeObject() ;
  type->tp_doc = ::typedoc;
  type->tp_iter = RunIter_iter;
  type->tp_iternext = RunIter_iternext;

  BaseType::initType("RunIter", module, "psana");
}

namespace {

PyObject*
RunIter_iter(PyObject* self)
{
  Py_XINCREF(self);
  return self;
}

PyObject*
RunIter_iternext(PyObject* self)
try {
  psana_python::pyext::RunIter* py_this = static_cast<psana_python::pyext::RunIter*>(self);
  psana::Run run = py_this->m_obj.next();
  if (run) {
    return psana_python::pyext::Run::PyObject_FromCpp(run);
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
