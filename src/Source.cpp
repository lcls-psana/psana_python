//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class Source...
//
// Author List:
//      Andy Salnikov
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "psana_python/Source.h"

//-----------------
// C/C++ Headers --
//-----------------
#include <exception>
#include <boost/make_shared.hpp>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "psana_python/AliasMap.h"
#include "psana_python/PdsSrc.h"
#include "psana_python/SrcMatch.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

namespace {

  // standard Python stuff
  PyObject* Source_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds);

  // type-specific methods
  PyObject* Source_srcMatch(PyObject* self, PyObject*);

  PyMethodDef methods[] = {
    { "srcMatch",     Source_srcMatch,   METH_VARARGS,
        "self.srcMatch([aliases:AliasMap]) -> :py:class:`SrcMatch`\n\n"
        "Returns object which can be used to match Src instances. If Source instance was constructed "
        "from a string then this method tries to resolve string as an alias. If alias is not found "
        "then it tries to parse the string according to the definitions above. If parsing fails then "
        "exception is thrown.\n"
        "Optional argument provides alias map instance which can be obtained from environment object"
        " (:py:class:`Env`). If alias map is not provided then alias names are not resolved (aliases "
        "cannot be used in this case)."
        "" },
    {0, 0, 0, 0}
   };

  char typedoc[] = "\
Class which defines matching criteria for source addresses \
returned from ``Event.get()`` method. This class provides several constructors:\n\
 * ``Source()`` without arguments matches data from any source address\n\
 * ``Source(None)`` will match data that do not have associated address (such as EventId data)\n\
 * ``Source(Src)`` with argument of type :py:class:`psana.Src` will match data that have the same address\n\
 * ``Source(int)`` where argument should be one of the :py:class:`BldInfo.Type` enum constants, will match\
 data coming from that particular BLD source\n\
 * ``Source(int, int, int, int)`` where argument should be (:py:class:`DetInfo.Detector`, int, :py:class:`DetInfo.Device`, int)\
 will match data coming from that particular DAQ (DetInfo) source\n\
 * ``Source(string)`` where string provides matching criteria, e.g. 'DetInfo(CxiDg1.*:Cspad2x2.0)'\n\n\
Main utility of this class consists entirely of being passed down to ``Event.get()`` method.\
";

}

//              ----------------------------------------
//              -- Public Function Member Definitions --
//              ----------------------------------------

void
psana_python::Source::initType(PyObject* module)
{
  PyTypeObject* type = BaseType::typeObject() ;
  type->tp_doc = ::typedoc;
  type->tp_methods = ::methods;
  type->tp_new = ::Source_new;

  BaseType::initType("Source", module, "psana");
}

namespace {

PyObject*
Source_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds)
try {

  // parse arguments
  PSEvt::Source source;

  // don't support keywords at the moment
  if (kwds!=0) Py_RETURN_NONE;

  if (PyTuple_GET_SIZE(args) == 1) {
    PyObject* arg0 = PyTuple_GET_ITEM(args, 0);
    if (arg0 == Py_None) {
      // None means no-source
      source = PSEvt::Source(PSEvt::Source::null);
    } else if (PyInt_Check(arg0)) {
      // integer argument means BldInfo type, check the reange
      int val = PyInt_AS_LONG(arg0);
      if (val < 0 or val >= Pds::BldInfo::NumberOf) {
        PyErr_SetString(PyExc_ValueError, "Error: BLD type out of range");
        return 0;
      }
      source = PSEvt::Source(Pds::BldInfo::Type(val));
    } else if (PyString_Check(arg0)) {
      // string is passed to Source ctor, which can throw
      source = PSEvt::Source(PyString_AsString(arg0));
    } else if (psana_python::PdsSrc::Object_TypeCheck(arg0)) {
      // Pds::Src wrapped into Python object
      const Pds::Src& src = psana_python::PdsSrc::cppObject(arg0);
      source = PSEvt::Source(src);
    } else {
      // anything else is an error
      PyErr_SetString(PyExc_TypeError, "Source() received argument of unsupported type");
      return 0;
    }
  } else if (PyTuple_GET_SIZE(args) == 4) {
    // 4 arguments need to be integers
    unsigned det, detId, dev, devId;
    if ( not PyArg_ParseTuple( args, "IIII:Source", &det, &detId, &dev, &devId ) ) {
      return 0;
    }
    // check the ranges
    if ( det >= Pds::DetInfo::NumDetector ) {
      PyErr_SetString(PyExc_ValueError, "Error: detector type out of range");
      return 0;
    }
    if ( dev >= Pds::DetInfo::NumDevice ) {
      PyErr_SetString(PyExc_ValueError, "Error: device type out of range");
      return 0;
    }
    source = PSEvt::Source(Pds::DetInfo::Detector(det), detId,
        Pds::DetInfo::Device(dev), devId);
  } else if (PyTuple_GET_SIZE(args) > 0) {
    // zero arguments ok, anything else is not
    PyErr_SetString(PyExc_TypeError, "Source(): zero, one, or four arguments are required");
    return 0;
  }

  PyObject* self = subtype->tp_alloc(subtype, 1);
  psana_python::Source* py_this = (psana_python::Source*) self;

  // construct in place, cannot throw
  new(&py_this->m_obj) PSEvt::Source(source);
  
  return self;

} catch (const std::exception& ex) {
  PyErr_SetString(PyExc_ValueError, ex.what());
  return 0;
}

PyObject*
Source_srcMatch(PyObject* self, PyObject* args)
{
  PSEvt::Source& cself = psana_python::Source::cppObject(self);

  // optional argument
  PyObject* amapObj = 0;
  if (not PyArg_ParseTuple(args, "|O!:Source.srcMatch", psana_python::AliasMap::typeObject(), &amapObj)) return 0;

  // get or make alias map
  boost::shared_ptr<PSEvt::AliasMap> amap;
  if (amapObj) {
    amap = psana_python::AliasMap::cppObject(amapObj);
  } else {
    amap = boost::make_shared<PSEvt::AliasMap>();
  }

  // forward to C++
  try {
    const PSEvt::Source::SrcMatch& srcMatch = cself.srcMatch(*amap);
    return psana_python::SrcMatch::PyObject_FromCpp(srcMatch);
  } catch (const std::exception& ex) {
    PyErr_SetString(PyExc_ValueError, ex.what());
    return 0;
  }
}

}
