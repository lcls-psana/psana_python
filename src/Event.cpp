//--------------------------------------------------------------------------
// File and Version Information:
//  $Id: Event.cpp 4455 2012-09-12 00:22:58Z salnikov@SLAC.STANFORD.EDU $
//
// Description:
//  Class Event...
//
// Author List:
//      Andrei Salnikov
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "psana_python/Event.h"

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
#include "PSEvt/EventId.h"
#include "PSEvt/Exceptions.h"
#include "PSEvt/ProxyDict.h"
#include "pdsdata/xtc/TypeId.hh"
#include "psana_python/EventKey.h"
#include "psana_python/PdsSrc.h"
#include "psana_python/Source.h"
#include "psana_python/ProxyDictMethods.h"
#include "pytools/make_pyshared.h"
//#include "psana_python/arg_get_put.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

using namespace psana_python;

namespace {

  // standard Python stuff
  PyObject* Event_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds);

  // type-specific methods
  PyObject* Event_keys(PyObject* self, PyObject* args);
  PyObject* Event_get(PyObject* self, PyObject* args);
  PyObject* Event_put(PyObject* self, PyObject* args);
  PyObject* Event_remove(PyObject* self, PyObject* args);
  PyObject* Event_run(PyObject* self, PyObject* args);

  PyMethodDef methods[] = {
    { "get",  Event_get,  METH_VARARGS, 
        "self.get(...) -> object\n\n"
        "Finds and retrieves objects from event. This is an overloaded method which "
        "can accept variable number of parameters:\n"
        " * ``get(type, src, key:string)``\n"
        " * ``get(type, src)`` - equivalent to ``get(type, src, \"\")``\n"
        " * ``get(type, key:string)`` - equivalent to ``get(type, Source(None), key)``\n"
        " * ``get(type)`` - equivalent to ``get(type, Source(None), \"\")``\n\n"
        "pyana compatibility methods (deprecated):\n"
        " * ``get(string)`` - gets any Python object stored with ``put(object, string)``\n"
        " * ``get(int, addr:Source)`` - equivalent to ``get(type, addr, \"\")`` where type is deduced"
        " from integer assumed to be ``Pds::TypeId::Type`` value\n"
        " * ``get(int, addr:string)`` - equivalent to ``get(type, Source(addr), \"\")`` where type is deduced"
        " from integer assumed to be ``Pds::TypeId::Type`` value\n"
        " * ``get(int)`` - equivalent to ``get(int, \"\")``\n\n"
        "In the first four methods type argument can be a type object or a list of type objects. "
        "If the list is given then object is returned whose type matches any one from the list. "
        "The src argument can be an instance of :py:class:`Source` or :py:class:`Src` types."},
    { "put",  Event_put,  METH_VARARGS, 
        "self.put(...) -> None\n\n"
        "Store new object in the event. This is an overloaded method which "
        "can accept variable number of parameters:\n"
        " * ``put(object, src, key:string)``\n"
        " * ``put(object, src)`` - equivalent to ``put(object, src, \"\")``\n"
        " * ``put(object, key:string)`` - equivalent to ``put(object, Source(None), key)``\n"
        " * ``put(object)`` - equivalent to ``put(type, Source(None), \"\")``\n\n"
        "The src argument can be an instance of :py:class:`Source` or :py:class:`Src` types. If Source instance is used "
        "for the src argument it must describe the source exactly (cannot contain wildcards)."},
    { "keys",  Event_keys,  METH_VARARGS, 
        "self.keys([src]) -> list\n\nGet the list of event keys (type :py:class:`EventKey`) for objects in the event. "
        "Optional argument can be either :py:class:`Source` instance or string. Without argument keys for all "
        "sources are returned."},
    { "remove",  Event_remove,  METH_VARARGS,
        "self.remove(...) -> bool\n\nRemove object of given type from the event. This is an overloaded method which "
        "can accept variable number of parameters:\n"
        " * ``remove(type, src, key:string)``\n"
        " * ``remove(type, src)`` - equivalent to ``remove(type, src, \"\")``\n"
        " * ``remove(type, key:string)`` - equivalent to ``remove(type, Source(None), key)``\n"
        " * ``remove(type)`` - equivalent to ``remove(type, Source(None), \"\")``\n\n"
        "Returns false if object did not exist before this call, true if object existed and was removed."},
    { "run",  Event_run,  METH_NOARGS,
        "self.run() -> int\n\nGet the run number form event. If run number is not known -1 is returned. "
        "This is a pyana compatibility method which is deprectated."},
    {0, 0, 0, 0}
   };

  char typedoc[] = "\
Python wrapper for psana Event class. Usually the instances of this class are \
created by the framework itself and returned to the client from framework methods. \
Clients can instantiate objects of this type directly, this may not be very useful \
in general but could be helpful during testing.\n\
The main methods of this class are get() and put() which allow you to retrieve data \
from the event or add new data to the event.\
";

}

//    ----------------------------------------
//    -- Public Function Member Definitions --
//    ----------------------------------------

void
psana_python::Event::initType(PyObject* module)
{
  PyTypeObject* type = BaseType::typeObject() ;
  type->tp_doc = ::typedoc;
  type->tp_methods = ::methods;
  type->tp_new = ::Event_new;

  BaseType::initType("Event", module, "psana");
}

// Dump object info to a stream
void 
psana_python::Event::print(std::ostream& out) const
{
  out << "psana.Event()" ;
}

namespace {

PyObject*
Event_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds)
{
  // no arguments expected
  if (PyTuple_GET_SIZE(args) != 0) {
    PyErr_SetString(PyExc_TypeError, "Event() does not need any arguments");
    return 0;
  }

  PyObject* self = subtype->tp_alloc(subtype, 1);
  Event* py_this = static_cast<Event*>(self);

  // construct in place, cannot throw
  boost::shared_ptr<PSEvt::ProxyDictI> dict = boost::make_shared<PSEvt::ProxyDict>(boost::shared_ptr<PSEvt::AliasMap>());
  new(&py_this->m_obj) boost::shared_ptr<PSEvt::Event>(new PSEvt::Event(dict));

  return self;
}

PyObject*
Event_keys(PyObject* self, PyObject* args)
{
  boost::shared_ptr<PSEvt::Event>& cself = Event::cppObject(self);
  return ProxyDictMethods::keys(*cself->proxyDict(), args);
}


PyObject* 
Event_get(PyObject* self, PyObject* args)
try {
  /*
   *  get(...) is very overloaded method, here is the list of possible argument combinations:
   *  get(type, src, key:string) 
   *  get(type, src)             - equivalent to get(type, src, "")
   *  get(type, key:string)      - equivalent to get(type, Source(None), key)
   *  get(type)                  - equivalent to get(type, Source(None), "")
   *  - pyana compatibility methods (deprecated):
   *  get(string)            - gets any Python object stored with put(object, string)
   *  get(int, addr:string)  - equivalent to get(type, Source(addr), "") where type is deduced
   *                           from integer assumed to be Pds::TypeId::Type value
   *  get(int)               - equivalent to get(int, "")
   *  
   *  In the first four methods type argument can be a type object or a list of type objects.
   *  If the list is given then object is returned whose type matches any one from the list.
   *  The src argument can be an instance of Source or Src types.
   */

  boost::shared_ptr<PSEvt::Event>& cself = psana_python::Event::cppObject(self);

  int nargs = PyTuple_GET_SIZE(args);
  if (nargs == 0) {
    return PyErr_Format(PyExc_ValueError, "Event.get(): at least one argument is required");
  }

  PyObject* arg0 = PyTuple_GET_ITEM(args, 0);
  PyObject* arg1 = nargs > 1 ? PyTuple_GET_ITEM(args, 1) : 0;

  // check the type of the first argument
  if (PyString_Check(arg0)) {

    // get(str)
    if (nargs != 1) {
      return PyErr_Format(PyExc_ValueError, "Event.get(string): one argument required (%d provided)", nargs);
    }
    return psana_python::ProxyDictMethods::get_compat_string(*cself->proxyDict(), arg0);
    
  } else if (PyInt_Check(arg0)) {
    
    // get(int, ...)
    if (nargs > 2) {
      return PyErr_Format(PyExc_ValueError, "Event.get(int, ...): one or two arguments required (%d provided)", nargs);
    }
    return psana_python::ProxyDictMethods::get_compat_typeid(*cself->proxyDict(), arg0, arg1);
    
  } else {

    // anything else
    if (nargs > 3) {
      return PyErr_Format(PyExc_ValueError, "Event.get(...): one to three arguments required (%d provided)", nargs);
    }
    PyObject* arg2 = nargs > 2 ? PyTuple_GET_ITEM(args, 2) : 0;

    // get source and key
    PSEvt::Source source(PSEvt::Source::null);
    std::string key;
    if (arg1) {
      if (psana_python::PdsSrc::Object_TypeCheck(arg1)) {
        // second argument is Src
        source = PSEvt::Source(psana_python::PdsSrc::cppObject(arg1));
      } else if (psana_python::Source::Object_TypeCheck(arg1)) {
        // second argument is Source
        source = psana_python::Source::cppObject(arg1);
      } else if (not arg2 and PyString_Check(arg1)) {
        // second argument is string and no third argument
        key = PyString_AsString(arg1);
      } else {
        // anything else is not expected
        PyErr_SetString(PyExc_TypeError, "Event.get(...) unexpected type of second argument");
        return 0;
      }
    }
    if (arg2) {
      if (PyString_Check(arg2)) {
        key = PyString_AsString(arg2);
      } else {
        PyErr_SetString(PyExc_TypeError, "Event.get(...) unexpected type of third argument");
        return 0;
      }
    }

    return psana_python::ProxyDictMethods::get(*cself->proxyDict(), arg0, source, key);
    
  }

} catch (const std::exception& ex) {

  PyErr_SetString(PyExc_ValueError, ex.what());
  return 0;
}

PyObject* 
Event_put(PyObject* self, PyObject* args)
try {

  /*
   * put() is overloaded method which can accept variable number of parameters:
   *  put(object, src, key:string)
   *  put(object, src) - equivalent to put(object, src, "")
   *  put(object, key:string) - equivalent to put(object, Source(None), key)
   *  put(object) - equivalent to put(object, Source(None), "")
   * The src argument can be an instance of Src type or Source type, Source
   * instance must be "exact".
   */

  boost::shared_ptr<PSEvt::Event>& cself = psana_python::Event::cppObject(self);

  int nargs = PyTuple_GET_SIZE(args);
  if (nargs < 1 or nargs > 3) {
    PyErr_SetString(PyExc_TypeError, "Event.put(): one to three arguments are required");
    return 0;
  }

  // object to store
  PyObject* arg0 = PyTuple_GET_ITEM(args, 0);

  // get two remaining arguments
  std::pair<Pds::Src, std::string> src_key;
  src_key = psana_python::ProxyDictMethods::arg_get_put(args, true, cself->proxyDict()->aliasMap());
  if (PyErr_Occurred()) return 0;

  return psana_python::ProxyDictMethods::put(*cself->proxyDict(), arg0, src_key.first, src_key.second);

} catch (const std::exception& ex) {

  PyErr_SetString(PyExc_ValueError, ex.what());
  return 0;

}

PyObject*
Event_remove(PyObject* self, PyObject* args)
try {

  boost::shared_ptr<PSEvt::Event>& cself = psana_python::Event::cppObject(self);

  int nargs = PyTuple_GET_SIZE(args);
  if (nargs < 1 or nargs > 3) {
    PyErr_SetString(PyExc_TypeError, "Event.remove(): one to three arguments are required");
    return 0;
  }

  // object to store
  PyObject* arg0 = PyTuple_GET_ITEM(args, 0);

  // get two remaining arguments
  std::pair<Pds::Src, std::string> src_key;
  src_key = psana_python::ProxyDictMethods::arg_get_put(args, true, cself->proxyDict()->aliasMap());
  if (PyErr_Occurred()) return 0;
  
  return psana_python::ProxyDictMethods::remove(*cself->proxyDict(), arg0, src_key.first, src_key.second);

} catch (const std::exception& ex) {

  PyErr_SetString(PyExc_ValueError, ex.what());
  return 0;

}

// return run number
PyObject*
Event_run(PyObject* self, PyObject* args)
{
  boost::shared_ptr<PSEvt::Event>& cself = psana_python::Event::cppObject(self);

  int run = -1;
  const boost::shared_ptr<PSEvt::EventId>& eventId = cself->get();
  if (eventId) run = eventId->run();

  return PyInt_FromLong(run);
}

}
