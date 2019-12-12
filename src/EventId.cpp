//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class EventId...
//
// Author List:
//      Andy Salnikov
//
//------------------------------------------------------------------------

#if PY_MAJOR_VERSION >= 3
#define IS_PY3K
#endif

//-----------------------
// This Class's Header --
//-----------------------
#include "psana_python/EventId.h"

//-----------------
// C/C++ Headers --
//-----------------

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "PSTime/Time.h"
#include "pyext/EventTime.h"
#include "psana/EventTime.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

namespace {

  // type-specific methods
  PyObject* EventId_time(PyObject* self, PyObject*);
  PyObject* EventId_idxtime(PyObject* self, PyObject*);
  PyObject* EventId_run(PyObject* self, PyObject*);
  PyObject* EventId_fiducials(PyObject* self, PyObject*);
  PyObject* EventId_ticks(PyObject* self, PyObject*);
  PyObject* EventId_vector(PyObject* self, PyObject*);

  PyMethodDef methods[] = {
    { "time",       EventId_time,     METH_NOARGS,
        "self.time() -> tuple\n\nReturn the time for event, which is tuple containing seconds and nanoseconds." },
    { "idxtime",    EventId_idxtime,  METH_NOARGS,
        "self.idxtime() -> EventTime\n\nReturn the time for event, in a class suitable for use with indexing." },
    { "run",        EventId_run,      METH_NOARGS,
        "self.run() -> int\n\nReturn the run number for event. If run number is not known -1 will be returned." },
    { "fiducials",  EventId_fiducials,METH_NOARGS,
        "self.fiducials() -> int\n\nReturns fiducials counter for the event.\n"
        "Note that MCC sends fiducials as 17-bit number which overflows "
        "frequently (fiducials clock runs at 360Hz) so this number is "
        "not unique. In some cases (e.g. when reading from old HDF5 "
        "files) fiducials is not know, 0 will be returned in this case." },
    { "ticks",        EventId_ticks,      METH_NOARGS,
        "self.ticks() -> int\n\nReturns 119MHz counter within the fiducial.\n"
        "Returns the value of 119MHz counter within the fiducial for the event "
        "code which initiated the readout. In some cases (e.g. when reading "
        "from old HDF5 files) ticks are not know, 0 will be returned in this case." },
    { "vector",     EventId_vector,   METH_NOARGS, "self.vector() -> int\n\nReturns event counter since Configure.\n"
        "Note that counter is saved as 15-bits integer and will overflow "
        "frequently. In some cases (e.g. when reading from old HDF5 "
        "files) counter is not know, 0 will be returned  in this case." },
    {0, 0, 0, 0}
   };

  char typedoc[] = "Event ID should include enough information to uniquely identify "
      "an event (and possibly a location of the event in data file). "
      "Currently we include event timestamp and run number into Event ID.";

}

//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------

void
psana_python::EventId::initType(PyObject* module)
{
  PyTypeObject* type = BaseType::typeObject() ;
  type->tp_doc = ::typedoc;
  type->tp_methods = ::methods;

  BaseType::initType("EventId", module, "psana");
}

namespace {

PyObject*
EventId_time(PyObject* self, PyObject* )
{
  boost::shared_ptr<PSEvt::EventId> cself = psana_python::EventId::cppObject(self);
  const PSTime::Time& time = cself->time();

  PyObject* tuple = PyTuple_New(2);
#ifdef IS_PY3K
  PyTuple_SET_ITEM(tuple, 0, PyLong_FromLong(time.sec()));
  PyTuple_SET_ITEM(tuple, 1, PyLong_FromLong(time.nsec()));
#else
  PyTuple_SET_ITEM(tuple, 0, PyInt_FromLong(time.sec()));
  PyTuple_SET_ITEM(tuple, 1, PyInt_FromLong(time.nsec()));
#endif

  return tuple;
}

PyObject*
EventId_idxtime(PyObject* self, PyObject* )
{
  boost::shared_ptr<PSEvt::EventId> cself = psana_python::EventId::cppObject(self);
  const PSTime::Time& time = cself->time();

  uint64_t t = (uint64_t)(time.sec())<<32 | time.nsec();
  psana::EventTime et(t,cself->fiducials());
  return psana_python::pyext::EventTime::PyObject_FromCpp(et);
}

PyObject*
EventId_run(PyObject* self, PyObject* )
{
  boost::shared_ptr<PSEvt::EventId> cself = psana_python::EventId::cppObject(self);
#ifdef IS_PY3K
  return PyLong_FromLong(cself->run());
#else
  return PyInt_FromLong(cself->run());
#endif
}

PyObject*
EventId_fiducials(PyObject* self, PyObject* )
{
  boost::shared_ptr<PSEvt::EventId> cself = psana_python::EventId::cppObject(self);
#ifdef IS_PY3K
  return PyLong_FromLong(cself->fiducials());
#else
  return PyInt_FromLong(cself->fiducials());
#endif
}

PyObject*
EventId_ticks(PyObject* self, PyObject* )
{
  boost::shared_ptr<PSEvt::EventId> cself = psana_python::EventId::cppObject(self);
#ifdef IS_PY3K
  return PyLong_FromLong(cself->ticks());
#else
  return PyInt_FromLong(cself->ticks());
#endif
}

PyObject*
EventId_vector(PyObject* self, PyObject* )
{
  boost::shared_ptr<PSEvt::EventId> cself = psana_python::EventId::cppObject(self);
#ifdef IS_PY3K
  return PyLong_FromLong(cself->vector());
#else
  return PyInt_FromLong(cself->vector());
#endif
}

}
