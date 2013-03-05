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

  PyMethodDef methods[] = {
    { "get",  EnvObjectStore_get,  METH_VARARGS, 
        "self.get(...) -> object\n\n"
        "Finds and retrieves objects from a store. This is an overloaded method which "
        "can accept variable number of parameters:\n"
        " * ``get(type, src)`` - find object of specified type and source address\n\n"
        "pyana compatibility methods (deprecated):\n"
        " * ``get(int, addr:string)`` - equivalent to ``get(type, Source(addr))`` where type is deduced"
        " from integer assumed to be Pds::TypeId::Type value\n"
        " * ``get(int)`` - equivalent to ``get(int, \"\")``\n\n"
        "In the first method type argument can be a type object or a list of type objects. "
        "If the list is given then object is returned whose type matches any one from the list. "
        "The src argument can be an instance of :py:class:`Source` or :py:class:`Src` types."},
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
  boost::shared_ptr<PSEvt::ProxyDictI> dict = boost::make_shared<PSEvt::ProxyDict>();
  new(&py_this->m_obj) boost::shared_ptr<PSEnv::EnvObjectStore>(new PSEnv::EnvObjectStore(dict));

  return self;
}

PyObject*
EnvObjectStore_keys(PyObject* self, PyObject* args)
{
  boost::shared_ptr<PSEnv::EnvObjectStore>& cself = EnvObjectStore::cppObject(self);
  return ProxyDictMethods::keys(cself->proxyDict(), args);
}

PyObject* 
EnvObjectStore_get(PyObject* self, PyObject* args)
{
  /*
   *  get(...) is very overloaded method, here is the list of possible argument combinations:
   *  get(type, src)             - equivalent to get(type, src, "")
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
  if (nargs < 1 or nargs > 2) {
    return PyErr_Format(PyExc_ValueError, "EnvObjectStore.get(...): one to two arguments required (%d provided)", nargs);
  }

  PyObject* arg0 = PyTuple_GET_ITEM(args, 0);
  PyObject* arg1 = nargs > 1 ? PyTuple_GET_ITEM(args, 1) : 0;

  if (PyInt_Check(arg0)) {
    
    // get(int, ...)
    return psana_python::ProxyDictMethods::get_compat_typeid(cself->proxyDict(), arg0, arg1);
    
  } else {

    // get source
    PSEvt::Source source;
    if (arg1) {
      if (psana_python::PdsSrc::Object_TypeCheck(arg1)) {
        // second argument is Src
        source = PSEvt::Source(psana_python::PdsSrc::cppObject(arg1));
      } else if (psana_python::Source::Object_TypeCheck(arg1)) {
        // second argument is Source
        source = psana_python::Source::cppObject(arg1);
      } else {
        // anything else is not expected
        PyErr_SetString(PyExc_TypeError, "EnvObjectStore.get(...) unexpected type of second argument");
        return 0;
      }
    }

    return psana_python::ProxyDictMethods::get(cself->proxyDict(), arg0, source, std::string());
    
  }
}

}
