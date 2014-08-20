//--------------------------------------------------------------------------
// File and Version Information:
//  $Id$
//
// Description:
//  Class Run...
//
// Author List:
//      Andrei Salnikov
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "Run.h"

//-----------------
// C/C++ Headers --
//-----------------
#include <vector>
#include <boost/cstdint.hpp>
#include <boost/python/object.hpp>
#include "psddl_python/psddl_python_numpy.h"
#include "psana_python/Event.h"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "EventIter.h"
#include "StepIter.h"
#include "psana_python/Env.h"
#include "EventTime.h"
#include "psana/Index.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

namespace {

  // type-specific methods
  PyObject* Run_steps(PyObject* self, PyObject*);
  PyObject* Run_events(PyObject* self, PyObject*);
  PyObject* Run_end(PyObject* self, PyObject*);
  PyObject* Run_nonzero(PyObject* self, PyObject*);
  PyObject* Run_env(PyObject* self, PyObject*);
  PyObject* Run_run(PyObject* self, PyObject*);
  PyObject* Run_times(PyObject* self, PyObject*);
  PyObject* Run_nsteps(PyObject* self, PyObject*);
  PyObject* Run_event(PyObject* self, PyObject*);

  PyMethodDef methods[] = {
    { "steps",       Run_steps,     METH_NOARGS, "self.Steps() -> iterator\n\nReturns iterator for contained steps (:py:class:`StepIter`)" },
    { "events",      Run_events,    METH_NOARGS, "self.events() -> iterator\n\nReturns iterator for contained events (:py:class:`EventIter`)" },
    { "end",         Run_end,       METH_NOARGS, "self.end() -> forces endrun (for use with indexing)" },
    { "env",         Run_env,       METH_NOARGS, "self.env() -> object\n\nReturns environment object" },
    { "run",         Run_run,       METH_NOARGS, "self.run() -> int\n\nReturns run number, -1 if unknown" },
    { "times",       Run_times,     METH_VARARGS,"self.times() -> array\n\nReturns tuple of event timestamps for a run for random access with indexing" },
    { "nsteps",      Run_nsteps,    METH_NOARGS, "self.nsteps() -> int\n\nReturns number of steps (a.k.a. calib-cycles) for a run.  works only with random access (indexing)." },
    { "event",       Run_event,     METH_VARARGS,"self.event() -> array\n\nReturns a randomly accessed event using timestamp argument" },
    { "__nonzero__", Run_nonzero,   METH_NOARGS, "self.__nonzero__() -> bool\n\nReturns true for non-null object" },
    {0, 0, 0, 0}
   };

  char typedoc[] = "Python wrapper for psana Run type. Run type represents data originating "
      "from a single run and it contains one or more steps (calib cycles) which in turn contain "
      "events. This class provides ways to iterate over individual steps in a run or over all "
      "events contained in all steps of this run. Actual iteration is implemented in "
      ":py:class:`StepIter` and :py:class:`EventIter` classes, this class serves as a factory "
      "for iterator instances.";

}

//    ----------------------------------------
//    -- Public Function Member Definitions --
//    ----------------------------------------

void
psana_python::pyext::Run::initType(PyObject* module)
{
  PyTypeObject* type = BaseType::typeObject() ;
  type->tp_doc = ::typedoc;
  type->tp_methods = ::methods;

  BaseType::initType("Run", module, "psana");
}

namespace {

PyObject*
Run_steps(PyObject* self, PyObject* )
{
  psana_python::pyext::Run* py_this = static_cast<psana_python::pyext::Run*>(self);
  return psana_python::pyext::StepIter::PyObject_FromCpp(py_this->m_obj.steps());
}

PyObject*
Run_events(PyObject* self, PyObject* )
{
  psana_python::pyext::Run* py_this = static_cast<psana_python::pyext::Run*>(self);
  return psana_python::pyext::EventIter::PyObject_FromCpp(py_this->m_obj.events());
}

PyObject*
Run_end(PyObject* self, PyObject* )
{
  // tell the index object to give us the EndRun datagram
  psana_python::pyext::Run* py_this = static_cast<psana_python::pyext::Run*>(self);
  py_this->m_obj.index().end();
  
  // use the event iterator to fetch the endrun and send it to the modules
  psana::EventIter evt_iter = py_this->m_obj.events();
  boost::shared_ptr<PSEvt::Event> evt = evt_iter.next();
  Py_RETURN_NONE;
}

PyObject*
Run_env(PyObject* self, PyObject* )
{
  psana_python::pyext::Run* py_this = static_cast<psana_python::pyext::Run*>(self);
  PSEnv::Env& env = py_this->m_obj.env();
  return psana_python::Env::PyObject_FromCpp(env.shared_from_this());
}

PyObject*
Run_nonzero(PyObject* self, PyObject* )
{
  psana_python::pyext::Run* py_this = static_cast<psana_python::pyext::Run*>(self);
  return PyBool_FromLong(long(bool(py_this->m_obj)));
}

PyObject*
Run_run(PyObject* self, PyObject* )
{
  psana_python::pyext::Run* py_this = static_cast<psana_python::pyext::Run*>(self);
  return PyInt_FromLong(py_this->m_obj.run());
}

PyObject*
Run_nsteps(PyObject* self, PyObject* args)
{
  psana_python::pyext::Run* py_this = static_cast<psana_python::pyext::Run*>(self);
  return Py_BuildValue("i",py_this->m_obj.index().nsteps());
}

PyObject*
Run_times(PyObject* self, PyObject* args)
{
  psana_python::pyext::Run* py_this = static_cast<psana_python::pyext::Run*>(self);
  unsigned nstep;
  if (!PyArg_ParseTuple(args, "|I", &nstep)) return NULL;
  unsigned getsteptimes = PyTuple_Size(args);
  psana::Index::EventTimeIter begin;
  psana::Index::EventTimeIter end;

  if (getsteptimes)
    py_this->m_obj.index().times(nstep,begin,end);
  else
    py_this->m_obj.index().times(begin,end);

  // the old way that worked when we used NPY_COMPLEX128 numpy arrays
  // npy_intp length=idxtimes.size();
  // PyObject* times = PyArray_SimpleNewFromData(1, &length, NPY_COMPLEX128, const_cast<psana::EventTime *> (&idxtimes[0]));
  // return Py_BuildValue("O", times);

  PyObject *pTuple = PyTuple_New(end-begin); // new reference
  unsigned i=0;
  for(psana::Index::EventTimeIter it=begin; it!=end; ++it) {
    PyTuple_SetItem(pTuple, i, psana_python::pyext::EventTime::PyObject_FromCpp(*it));
    i++;
  }

  return pTuple;
}

PyObject *
Run_event(PyObject* self, PyObject* args)
{
  int status;
  psana_python::pyext::EventTime *pyEventTime=NULL;
  if (!PyArg_ParseTuple(args, "O", &pyEventTime)) return NULL;

  // the old way that worked when we used NPY_COMPLEX128 numpy arrays
  // psana::EventTime time;
  // PyArray_ScalarAsCtype(pyEventTime,&time);

  psana_python::pyext::Run* py_this = static_cast<psana_python::pyext::Run*>(self);
  psana::Index& index = py_this->m_obj.index();
  status = index.jump(pyEventTime->m_obj);
  if (status) Py_RETURN_NONE;

  psana::EventIter evt_iter = py_this->m_obj.events();
  boost::shared_ptr<PSEvt::Event> evt = evt_iter.next();
  if (evt) {
    return psana_python::Event::PyObject_FromCpp(evt);
  } else {
    Py_RETURN_NONE;
  }
}

}
