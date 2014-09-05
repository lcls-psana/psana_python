//--------------------------------------------------------------------------
// File and Version Information:
//  $Id$
//
// Description:
//  Class EventIter...
//
// Author List:
//      Andrei Salnikov
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "EventIter.h"

//-----------------
// C/C++ Headers --
//-----------------
#include <exception>
#include <boost/python/object.hpp>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "psana_python/Event.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

namespace {

  // type-specific methods
  PyObject* EventIter_iter(PyObject* self);
  PyObject* EventIter_iternext(PyObject* self);

  char typedoc[] = "Class which supports iteration over events contained in a "
      "particular :py:class:`DataSource`, :py:class:`Run`, or :py:class:`Step` "
      "instance. Iterator returns event (:py:class:`Event`) objects which contain "
      "all experimental data for particular event.";

}

//    ----------------------------------------
//    -- Public Function Member Definitions --
//    ----------------------------------------

void
psana_python::pyext::EventIter::initType(PyObject* module)
{
  PyTypeObject* type = BaseType::typeObject() ;
  type->tp_doc = ::typedoc;
  type->tp_iter = EventIter_iter;
  type->tp_iternext = EventIter_iternext;

  BaseType::initType("EventIter", module, "psana");
}

namespace {

PyObject*
EventIter_iter(PyObject* self)
{
  Py_XINCREF(self);
  return self;
}

/// class for releasing and restoring the the Python GIL lock
class GILReleaser {
public:
  inline GILReleaser()
  {
    m_thread_state = PyEval_SaveThread();
  }
  
  inline ~GILReleaser()
  {
    PyEval_RestoreThread(m_thread_state);
    m_thread_state = NULL;
  }

private:
  PyThreadState * m_thread_state;
};

PyObject*
EventIter_iternext(PyObject* self)
try {
  psana_python::pyext::EventIter* py_this = static_cast<psana_python::pyext::EventIter*>(self);
  boost::shared_ptr<PSEvt::Event> evt;
  {
    // Release GIL lock during processing of all Psana Modules. 
    // psana will ensure the GIL is restored/released for Psana Python Modules.
    // effectively the GIL will be released for only C++ modules.
    GILReleaser releaseGIL;
    evt = py_this->m_obj.next();
  }
  if (evt) {
    return psana_python::Event::PyObject_FromCpp(evt);
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
