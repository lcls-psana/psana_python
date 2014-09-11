//--------------------------------------------------------------------------
// File and Version Information:
//  $Id: EnvObjectStore.cpp 4455 2012-09-12 00:22:58Z salnikov@SLAC.STANFORD.EDU $
//
// Description:
//  Class EnvObjectStore...
//
// Author List:
//      Andrei Salnikov
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "psana_python/EnvObjectStore.h"

//-----------------
// C/C++ Headers --
//-----------------
#include <iostream>
#include <vector>
#include <typeinfo>
#include <utility>
#include <boost/python.hpp>
#include <boost/make_shared.hpp>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "PSEvt/ProxyDict.h"
#include "psana_python/EventKey.h"
#include "psana_python/PdsSrc.h"
#include "psana_python/Source.h"
#include "psana_python/ProxyDictMethods.h"
#include "pytools/make_pyshared.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

using namespace psana_python;

namespace {

  // standard Python stuff
  PyObject* EnvObjectStore_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds);

  // type-specific methods
  PyObject* EnvObjectStore_keys(PyObject* self, PyObject* args);
  PyObject* EnvObjectStore_get(PyObject* self, PyObject* args);
  PyObject* EnvObjectStore_put(PyObject* self, PyObject* args);

  PyMethodDef methods[] = {
    { "get",  EnvObjectStore_get,  METH_VARARGS, 
        "self.get(...) -> object\n\n"
        "Finds and retrieves objects from a store. This is an overloaded method which "
        "can accept variable number of parameters:\n"
        " * ``get(type, src)`` - find object of specified type and source address\n\n"
        " * ``get(type, src, key:string)`` - find object of specified type and source address with the given key\n\n"
        "pyana compatibility methods (deprecated):\n"
        " * ``get(int, addr:Source)`` - equivalent to ``get(type, addr)`` where type is deduced"
        " from integer assumed to be Pds::TypeId::Type value\n"
        " * ``get(int, addr:string)`` - equivalent to ``get(type, Source(addr))`` where type is deduced"
        " from integer assumed to be Pds::TypeId::Type value\n"
        " * ``get(int)`` - equivalent to ``get(int, \"\")``\n\n"
        "In the first method type argument can be a type object or a list of type objects. "
        "If the list is given then object is returned whose type matches any one from the list. "
        "The src argument can be an instance of :py:class:`Source` or :py:class:`Src` types."},
    { "put", EnvObjectStore_put,  METH_VARARGS,
      "self.put(...) -> None\n\n"
      "Store new object in the store. This is an overloaded method which "
      "can accept a variable number of parameters:\n"
      " * ``put(object,src)``\n"
      " * ``put(object)`` - equivalent to ``put(type, Source(None), \"\")``\n\n"
      "The src argument can be an instance of :py:class:`Source` or :py:class:`Src` types. If Source instance is used "
      "for the src argument it must describe the source exactly (cannot contain wildcards)."},
    { "keys",  EnvObjectStore_keys,  METH_VARARGS, 
        "self.keys([src]) -> list\n\nGet the list of event keys (type :py:class:`EventKey`) for objects in the store. "
        "Optional argument can be either :py:class:`Source` instance or string. Without argument keys for all "
        "sources are returned."},
    {0, 0, 0, 0}
   };

  char typedoc[] = "\
Python wrapper for psana EnvObjectStore class. Usually the instances of this class are \
created by the framework itself and returned to the client from framework methods. \
Clients can instantiate objects of this type directly, this may not be very useful \
in general but could be helpful during testing.\n\
The main method of this class are get() which allows you to retrieve data \
from the store.\
";

}

//    ----------------------------------------
//    -- Public Function Member Definitions --
//    ----------------------------------------

void
psana_python::EnvObjectStore::initType(PyObject* module)
{
  PyTypeObject* type = BaseType::typeObject() ;
  type->tp_doc = ::typedoc;
  type->tp_methods = ::methods;
  type->tp_new = ::EnvObjectStore_new;

  BaseType::initType("EnvObjectStore", module, "psana");
}

// Dump object info to a stream
void 
psana_python::EnvObjectStore::print(std::ostream& out) const
{
  out << "psana.EnvObjectStore()" ;
}

namespace {

PyObject*
EnvObjectStore_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds)
{
  // no arguments expected
  if (PyTuple_GET_SIZE(args) != 0) {
    PyErr_SetString(PyExc_TypeError, "EnvObjectStore() does not need any arguments");
    return 0;
  }

  PyObject* self = subtype->tp_alloc(subtype, 1);
  EnvObjectStore* py_this = static_cast<EnvObjectStore*>(self);

  // construct in place, cannot throw
  boost::shared_ptr<PSEvt::ProxyDictI> dict = boost::make_shared<PSEvt::ProxyDict>(boost::shared_ptr<PSEvt::AliasMap>());
  new(&py_this->m_obj) boost::shared_ptr<PSEnv::EnvObjectStore>(new PSEnv::EnvObjectStore(dict));

  return self;
}

PyObject*
EnvObjectStore_keys(PyObject* self, PyObject* args)
{
  boost::shared_ptr<PSEnv::EnvObjectStore>& cself = EnvObjectStore::cppObject(self);
  return ProxyDictMethods::keys(*cself->proxyDict(), args);
}

PyObject* 
EnvObjectStore_get(PyObject* self, PyObject* args)
{
  /*
   *  get(...) is very overloaded method, here is the list of possible argument combinations:
   *  get(type, src)             - equivalent to get(type, src, "")
   *  get(type, src, key)        - equivalent to get(type, src, "")
   *  get(type)                  - equivalent to get(type, Source(None), "")
   *  - pyana compatibility methods (deprecated):
   *  get(int, addr:string)  - equivalent to get(type, Source(addr), "") where type is deduced
   *                           from integer assumed to be Pds::TypeId::Type value
   *  get(int)               - equivalent to get(int, "")
   *  
   *  In the first four methods type argument can be a type object or a list of type objects.
   *  If the list is given then object is returned whose type matches any one from the list.
   *  The src argument can be an instance of Source or Src types.
   */

  boost::shared_ptr<PSEnv::EnvObjectStore>& cself = psana_python::EnvObjectStore::cppObject(self);

  int nargs = PyTuple_GET_SIZE(args);
  if (nargs < 1 or nargs > 3) {
    return PyErr_Format(PyExc_ValueError, "EnvObjectStore.get(...): one to three arguments required (%d provided)", nargs);
  }

  PyObject* arg0 = PyTuple_GET_ITEM(args, 0);
  PyObject* arg1 = nargs >= 2 ? PyTuple_GET_ITEM(args, 1) : 0;
  PyObject *arg2 = nargs >= 3 ? PyTuple_GET_ITEM(args, 2) : 0;

  if (PyInt_Check(arg0)) {

    // get(int, ...)
    return psana_python::ProxyDictMethods::get_compat_typeid(*cself->proxyDict(), arg0, arg1);
    
  } else {

    // get source and key
    PSEvt::Source source;
    std::string key;

    if (arg1) {
      source = PSEvt::Source(PSEvt::Source::null);
      if (psana_python::PdsSrc::Object_TypeCheck(arg1)) {
        // second argument is Src
        source = PSEvt::Source(psana_python::PdsSrc::cppObject(arg1));
      } else if (psana_python::Source::Object_TypeCheck(arg1)) {
        // second argument is Source
        source = psana_python::Source::cppObject(arg1);
      } else if (not arg2 and PyString_Check(arg1)) {
        key = PyString_AsString(arg1);
      } else {
        // anything else is not expected
        PyErr_SetString(PyExc_TypeError, "EnvObjectStore.get(...) unexpected type of second argument");
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
}

PyObject* 
EnvObjectStore_put(PyObject* self, PyObject* args)
try {
  /*
   *  put(...) is overloaded method, possible argument combinations:
   *  put(type, src)             - equivalent to put(type, src, "")
   *  put(type,key)
   *  put(type, src, key)        - equivalent to put(type, src, key)
   *  put(type)                  - equivalent to put(type, Source(None), "")
   *  - pyana compatibility methods (deprecated):
   *  No pyana compatible methods implemented - this was never available to pyana in the past
   */

  boost::shared_ptr<PSEnv::EnvObjectStore>& cself = psana_python::EnvObjectStore::cppObject(self);

  int nargs = PyTuple_GET_SIZE(args);
  if (nargs < 1 or nargs > 3) {
    return PyErr_Format(PyExc_ValueError, "EnvObjectStore.put(...): one to three arguments required (%d provided)", nargs);
  }
  PyObject* arg0 = PyTuple_GET_ITEM(args, 0);

  // get source and key
  std::pair<Pds::Src, std::string> src_key;
  src_key = psana_python::ProxyDictMethods::arg_get_put(args, false, cself->proxyDict()->aliasMap());
  if (PyErr_Occurred()) return 0;
  PyObject * retVal = psana_python::ProxyDictMethods::put(*cself->proxyDict(), arg0, src_key.first, src_key.second);
  return retVal;

} catch (const std::exception& ex) {
  PyErr_SetString(PyExc_ValueError, ex.what());
  return 0;
}

} // local namespace
