//--------------------------------------------------------------------------
// File and Version Information:
//  $Id$
//
// Description:
//  Class DgramList.
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "psana_python/DgramList.h"

//-----------------
// C/C++ Headers --
//-----------------
#include <iostream>
#include <vector>
#include <typeinfo>
#include <utility>
#include <boost/python.hpp>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "psana_python/PdsSrc.h"
#include "pytools/make_pyshared.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

using namespace psana_python;

namespace {

  // standard Python stuff
  PyObject* DgramList_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds);

  // type-specific methods
  PyObject* DgramList_getFileNames(PyObject* self, PyObject* args);
  PyObject* DgramList_getOffsets(PyObject* self, PyObject* args);

  PyMethodDef methods[] = {
    { "getFileNames",    DgramList_getFileNames,    METH_NOARGS,  "self.getFileNames()\n\nGet list of xtc filenames that are in this DgramList for a given event.\nNote .inprogress extensions typically indicate reading from live data."},
    { "getOffsets",  DgramList_getOffsets,  METH_NOARGS,   "self.getOffsets()\n\nGet offsets into the xtc filenames of datagrams in this DgramList."},
    {0, 0, 0, 0}
   };

  char typedoc[] = "\
Python wrapper for psana DgramList class. Usually the instances of this class are \
created by the framework itself and returned to the client from framework methods. \
This is a list of datagrams comprising an event. Exposed on the Python side are the \
list of filenames, and offsets into those files. \
Clients can instantiate objects of this type directly, this may not be very useful \
in general but could be helpful during testing.\n\
";

}

//    ----------------------------------------
//    -- Public Function Member Definitions --
//    ----------------------------------------

void
psana_python::DgramList::initType(PyObject* module)
{
  PyTypeObject* type = BaseType::typeObject() ;
  type->tp_doc = ::typedoc;
  type->tp_methods = ::methods;
  type->tp_new = ::DgramList_new;

  BaseType::initType("DgramList", module, "psana");
}

// Dump object info to a stream
void 
psana_python::DgramList::print(std::ostream& out) const
{
  out << "psana.DgramList()" ;
}

namespace {

PyObject*
DgramList_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds)
{
  // no arguments expected
  if (PyTuple_GET_SIZE(args) != 0) {
    PyErr_SetString(PyExc_TypeError, "DgramList() does not need any arguments");
    return 0;
  }

  PyObject* self = subtype->tp_alloc(subtype, 1);
  DgramList* py_this = static_cast<DgramList*>(self);

  // construct in place, may not throw
  new(&py_this->m_obj) boost::shared_ptr<XtcInput::DgramList>(new XtcInput::DgramList());

  return self;
}

PyObject*
DgramList_getFileNames(PyObject* self, PyObject*)
{
  boost::shared_ptr<XtcInput::DgramList>& cself = DgramList::cppObject(self);
  PyObject * pyFileNames = PyTuple_New(0);
  if (not cself) {
    return pyFileNames;
  }
  XtcInput::DgramList::FileListImpl filenames = cself->getFileNames();
  _PyTuple_Resize(&pyFileNames, filenames.size());
  XtcInput::DgramList::FileListImpl::iterator pos = filenames.begin();
  Py_ssize_t pyPos = 0;
  while (pos != filenames.end()) {
    const std::string &path = pos->path();
    PyTuple_SetItem(pyFileNames, pyPos++, Py_BuildValue("s", path.c_str()));
    ++pos;
  }
  return pyFileNames;
}

PyObject*
DgramList_getOffsets(PyObject* self, PyObject*)
{
  boost::shared_ptr<XtcInput::DgramList>& cself = DgramList::cppObject(self);
  PyObject * pyOffsets = PyTuple_New(0);
  if (not cself) {
    return pyOffsets;
  }
  XtcInput::DgramList::OffsetImpl offsets = cself->getOffsets();
  _PyTuple_Resize(&pyOffsets, offsets.size());
  XtcInput::DgramList::OffsetImpl::iterator pos = offsets.begin();
  Py_ssize_t pyPos = 0;
  while (pos != offsets.end()) {
    PyTuple_SetItem(pyOffsets, pyPos++, Py_BuildValue("l", *pos++));
  }
  return pyOffsets;
}


}
