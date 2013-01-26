//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class PdsDetInfo...
//
// Author List:
//      Andy Salnikov
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "psana_python/PdsDetInfo.h"

//-----------------
// C/C++ Headers --
//-----------------

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "psana_python/PdsSrc.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

namespace {

  // standard Python stuff
  int PdsDetInfo_init( PyObject* self, PyObject* args, PyObject* kwds );

  // type-specific methods
  PyObject* PdsDetInfo_processId(PyObject* self, PyObject*);
  PyObject* PdsDetInfo_detector(PyObject* self, PyObject*);
  PyObject* PdsDetInfo_device(PyObject* self, PyObject*);
  PyObject* PdsDetInfo_detId(PyObject* self, PyObject*);
  PyObject* PdsDetInfo_devId(PyObject* self, PyObject*);
  PyObject* PdsDetInfo_detName(PyObject* self, PyObject*);
  PyObject* PdsDetInfo_devName(PyObject* self, PyObject*);

  PyMethodDef methods[] = {
    { "processId",   PdsDetInfo_processId,  METH_NOARGS, "self.processId() -> int\n\nReturns process ID number." },
    { "detector",    PdsDetInfo_detector,   METH_NOARGS, "self.detector() -> int\n\nReturns detector type." },
    { "device",      PdsDetInfo_device,     METH_NOARGS, "self.device() -> int\n\nReturns device type." },
    { "detId",       PdsDetInfo_detId,      METH_NOARGS, "self.detId() -> int\n\nReturns detector ID number." },
    { "devId",       PdsDetInfo_devId,      METH_NOARGS, "self.devId() -> int\n\nReturns device ID number." },
    { "detName",     PdsDetInfo_detName,    METH_NOARGS, "self.detName() -> string\n\nReturns detector name." },
    { "devName",     PdsDetInfo_devName,    METH_NOARGS, "self.devName() -> string\n\nReturns device name." },
    {0, 0, 0, 0}
   };

  char typedoc[] = "Class which defines detector-level addresses.\n"
      "Constructor can take either four or five arguments, all of them "
      "should be integer numbers. With four arguments they are "
      "(detector, detector_id, device, device_id), five arguments are "
      "(process_id, detector, detector_id, device, device_id).";

}

//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------

void
psana_python::PdsDetInfo::initType(PyObject* module)
{
  PyTypeObject* type = BaseType::typeObject() ;
  type->tp_doc = ::typedoc;
  type->tp_methods = ::methods;
  type->tp_init = ::PdsDetInfo_init;
  type->tp_base = psana_python::PdsSrc::typeObject();
  Py_INCREF(type->tp_base);

  BaseType::initType("DetInfo", module);
}

namespace {

int
PdsDetInfo_init( PyObject* self, PyObject* args, PyObject* kwds )
{
  // parse arguments
  unsigned processId=0, det, detId, dev, devId;
  if (PyTuple_GET_SIZE(args) == 4) {
    if ( not PyArg_ParseTuple( args, "IIII:DetInfo", &det, &detId, &dev, &devId ) ) {
      return -1;
    }
  } else {
    if ( not PyArg_ParseTuple( args, "IIIII:DetInfo", &processId, &det, &detId, &dev, &devId ) ) {
      return -1;
    }
  }
  if ( det >= Pds::DetInfo::NumDetector ) {
    PyErr_SetString(PyExc_ValueError, "Error: detector type out of range");
    return -1;
  }
  if ( dev >= Pds::DetInfo::NumDevice ) {
    PyErr_SetString(PyExc_ValueError, "Error: device type out of range");
    return -1;
  }

  psana_python::PdsDetInfo* py_this = (psana_python::PdsDetInfo*) self;
  if ( not py_this ) {
    PyErr_SetString(PyExc_RuntimeError, "Error: self is NULL");
    return -1;
  }

  new(&py_this->m_obj) Pds::DetInfo(processId, Pds::DetInfo::Detector(det), detId,
      Pds::DetInfo::Device(dev), devId);

  return 0;
}

PyObject*
PdsDetInfo_processId(PyObject* self, PyObject* )
{
  Pds::DetInfo& cself = psana_python::PdsDetInfo::cppObject(self);
  return PyInt_FromLong(cself.processId());
}

PyObject*
PdsDetInfo_detector(PyObject* self, PyObject* )
{
  Pds::DetInfo& cself = psana_python::PdsDetInfo::cppObject(self);
  return PyInt_FromLong(cself.detector());
}

PyObject*
PdsDetInfo_device(PyObject* self, PyObject* )
{
  Pds::DetInfo& cself = psana_python::PdsDetInfo::cppObject(self);
  return PyInt_FromLong(cself.device());
}

PyObject*
PdsDetInfo_detId(PyObject* self, PyObject* )
{
  Pds::DetInfo& cself = psana_python::PdsDetInfo::cppObject(self);
  return PyInt_FromLong(cself.detId());
}

PyObject*
PdsDetInfo_devId(PyObject* self, PyObject* )
{
  Pds::DetInfo& cself = psana_python::PdsDetInfo::cppObject(self);
  return PyInt_FromLong(cself.devId());
}

PyObject*
PdsDetInfo_detName(PyObject* self, PyObject* )
{
  Pds::DetInfo& cself = psana_python::PdsDetInfo::cppObject(self);
  return PyString_FromString(Pds::DetInfo::name(cself.detector()));
}

PyObject*
PdsDetInfo_devName(PyObject* self, PyObject* )
{
  Pds::DetInfo& cself = psana_python::PdsDetInfo::cppObject(self);
  return PyString_FromString(Pds::DetInfo::name(cself.device()));
}

}
