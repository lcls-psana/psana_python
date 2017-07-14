//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class EventOffset...
//
// Author List:
//      Elliott Slaughter
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "psana_python/EventOffset.h"

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

  // type-specific methods
  PyObject* EventOffset_offsets(PyObject* self, PyObject*);
  PyObject* EventOffset_filenames(PyObject* self, PyObject*);
  PyObject* EventOffset_configureOffset(PyObject* self, PyObject*);
  PyObject* EventOffset_beginRunOffset(PyObject* self, PyObject*);
  PyObject* EventOffset_lastBeginCalibCycleOffset(PyObject* self, PyObject*);
  PyObject* EventOffset_lastBeginCalibCycleFilename(PyObject* self, PyObject*);

  PyMethodDef methods[] = {
    { "filenames",                   EventOffset_filenames,                   METH_NOARGS, "self.filenames() -> list\n\nReturns list of datagram filenames."},
    { "offsets",                     EventOffset_offsets,                     METH_NOARGS, "self.offsets() -> list\n\nReturns list of event offsets in datagram."},
    { "configureOffset",             EventOffset_configureOffset,             METH_NOARGS, "self.configureOffset() -> int\n\nReturns offset of configure transition in datagram."},
    { "beginRunOffset",              EventOffset_beginRunOffset,              METH_NOARGS, "self.beginRunOffset() -> int\n\nReturns offset of beginRun transition in datagram."},
    { "lastBeginCalibCycleFilename", EventOffset_lastBeginCalibCycleFilename, METH_NOARGS, "self.lastBeginCalibCycleFilename() -> int\n\nReturns filename containing beginCalibCycle transition."},
    { "lastBeginCalibCycleOffset",   EventOffset_lastBeginCalibCycleOffset,   METH_NOARGS, "self.lastBeginCalibCycleOffset() -> int\n\nReturns offset of beginCalibCycle transition in datagram."},
    {0, 0, 0, 0}
   };

  char typedoc[] = "Event Offset should include enough information to locate "
      "an event in data file. "
      "Currently we include event offsets and filenames in the Event Offset.";

}

//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------

void
psana_python::EventOffset::initType(PyObject* module)
{
  PyTypeObject* type = BaseType::typeObject() ;
  type->tp_doc = ::typedoc;
  type->tp_methods = ::methods;

  BaseType::initType("EventOffset", module, "psana");
}

namespace {

PyObject*
EventOffset_offsets(PyObject* self, PyObject* )
{
  boost::shared_ptr<PSEvt::EventOffset> cself = psana_python::EventOffset::cppObject(self);
  const std::vector<int64_t> &offsets = cself->offsets();
  PyObject *result = PyList_New(offsets.size());
  for (size_t i = 0; i < offsets.size(); i++) {
    PyList_SetItem(result, i, PyLong_FromLongLong(offsets[i]));
  }
  return result;
}

PyObject*
EventOffset_filenames(PyObject* self, PyObject* )
{
  boost::shared_ptr<PSEvt::EventOffset> cself = psana_python::EventOffset::cppObject(self);
  const std::vector<std::string> &filenames = cself->filenames();
  PyObject *result = PyList_New(filenames.size());
  for (size_t i = 0; i < filenames.size(); i++) {
    PyList_SetItem(result, i, PyString_FromString(filenames[i].c_str()));
  }
  return result;
}

PyObject*
EventOffset_configureOffset(PyObject* self, PyObject* )
{
  boost::shared_ptr<PSEvt::EventOffset> cself = psana_python::EventOffset::cppObject(self);
  return PyLong_FromLongLong(cself->configureOffset());
}

PyObject*
EventOffset_beginRunOffset(PyObject* self, PyObject* )
{
  boost::shared_ptr<PSEvt::EventOffset> cself = psana_python::EventOffset::cppObject(self);
  return PyLong_FromLongLong(cself->beginRunOffset());
}

PyObject*
EventOffset_lastBeginCalibCycleOffset(PyObject* self, PyObject* )
{
  boost::shared_ptr<PSEvt::EventOffset> cself = psana_python::EventOffset::cppObject(self);
  return PyLong_FromLongLong(cself->lastBeginCalibCycleOffset());
}

PyObject*
EventOffset_lastBeginCalibCycleFilename(PyObject* self, PyObject* )
{
  boost::shared_ptr<PSEvt::EventOffset> cself = psana_python::EventOffset::cppObject(self);
  return PyString_FromString(cself->lastBeginCalibCycleFilename().c_str());
}

}
