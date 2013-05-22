//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class ProxyDictMethods...
//
// Author List:
//      Andy Salnikov
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "psana_python/ProxyDictMethods.h"

//-----------------
// C/C++ Headers --
//-----------------
#include <list>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "pdsdata/xtc/TypeId.hh"
#include "psana_python/EventKey.h"
#include "psddl_python/ConverterMap.h"
#include "PSEvt/DataProxy.h"
#include "PSEvt/Exceptions.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

using psddl_python::ConverterMap;
using psddl_python::Converter;

//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------

namespace psana_python {

PyObject*
ProxyDictMethods::keys(PSEvt::ProxyDictI& proxyDict, PyObject* args)
{
  // parse arguments
  PSEvt::Source src;
  PyObject* obj = 0;
  if (not PyArg_ParseTuple(args, "|o:Event.keys", &obj)) return 0;

  // check type
  if (obj) {
    if (psana_python::Source::Object_TypeCheck(obj)) {
      src = psana_python::Source::cppObject(obj);
    } else if (PyString_Check(obj)) {
      // this can throw
      try {
        src = PSEvt::Source(PyString_AsString(obj));
      } catch (const std::exception& ex) {
        PyErr_SetString(PyExc_ValueError, ex.what());
        return 0;
      }
    } else {
      PyErr_SetString(PyExc_TypeError, "Event.keys(src) only accepts Source or string argument");
      return 0;
    }
  }

  // call C++ event method
  std::list<PSEvt::EventKey> keys;
  proxyDict.keys(keys, src);

  // convert keys to python objects
  PyObject* pykeys = PyList_New(keys.size());
  int i = 0;
  for (std::list<PSEvt::EventKey>::const_iterator it = keys.begin(); it != keys.end(); ++ it, ++ i) {
    PyList_SET_ITEM(pykeys, i, psana_python::EventKey::PyObject_FromCpp(*it));
  }
  return pykeys;
}

std::vector<pytools::pyshared_ptr>
ProxyDictMethods::get_types(PyObject* arg0, const char* method)
{
  std::vector<pytools::pyshared_ptr> types;

  pytools::pyshared_ptr iter = pytools::make_pyshared(PyObject_GetIter(arg0));
  if (iter) {
    // any iterable means the list of types
    pytools::pyshared_ptr item = pytools::make_pyshared(PyIter_Next(iter.get()));
    while (item) {
      types.push_back(item);
      item = pytools::make_pyshared(PyIter_Next(iter.get()));
    }
  } else {
    // clear exception from PyObject_GetIter
    PyErr_Clear();
    types.push_back(pytools::make_pyshared(arg0, false));
  }

  if (types.empty()) {
    PyErr_Format(PyExc_TypeError, "Event.%s(...) expecting a sequence of types, received empty sequence", method);
    return types;
  }

  // make sure that they are all types
  for (std::vector<pytools::pyshared_ptr>::const_iterator it = types.begin(); it != types.end(); ++ it) {
    if (not PyType_Check(it->get())) {
      PyErr_Format(PyExc_TypeError, "Event.%s(...) argument must be type", method);
      types.clear();
      break;
    }
  }

  return types;
}

PyObject*
ProxyDictMethods::get_compat_string(PSEvt::ProxyDictI& proxyDict, PyObject* arg0)
{
  /*
   *  pyana compatibility method (deprecated):
   *  get(string)            - gets any Python object stored with put(object, string)
   */

  std::string key(PyString_AsString(arg0));

  // get any PyObject with no address and a key
  boost::shared_ptr<void> vdata = proxyDict.get(&typeid(const PyObject), PSEvt::Source(PSEvt::Source::null), key, 0);
  if (vdata) {
    PyObject* pyobj = (PyObject*)vdata.get();
    Py_INCREF(pyobj);
    return pyobj;
  }

  Py_RETURN_NONE;
}

PyObject*
ProxyDictMethods::get_compat_typeid(PSEvt::ProxyDictI& proxyDict, PyObject* arg0, PyObject* arg1)
{
  /*
   *  pyana compatibility methods (deprecated):
   *  get(int, addr:string)  - equivalent to get(type, Source(addr), "") where type is deduced
   *                           from integer assumed to be Pds::TypeId::Type value
   *  get(int)               - equivalent to get(int, "")
   */

  // integer means get all types matching PDS TypeId
  int pdsTypeId = PyInt_AsLong(arg0);
  if (pdsTypeId < 0 or pdsTypeId >= Pds::TypeId::NumberOf) {
    return PyErr_Format(PyExc_ValueError, "Event.get(int, ...): argument value outside range 0..%d", Pds::TypeId::NumberOf-1);
  }

  // get an address
  PSEvt::Source source;
  if (arg1) {
    if (PyString_Check(arg1)) {
      // this can throw
      try {
        source = PSEvt::Source(PyString_AsString(arg1));
      } catch (const std::exception& ex) {
        PyErr_SetString(PyExc_ValueError, ex.what());
        return 0;
      }
    }
  }

  ConverterMap& cmap = ConverterMap::instance();
  BOOST_FOREACH(boost::shared_ptr<Converter> cvt, cmap.getFromPdsConverters(pdsTypeId)) {
    if (PyObject* obj = cvt->convert(proxyDict, source, std::string())) return obj;
  }

  Py_RETURN_NONE;
}

PyObject*
ProxyDictMethods::get(PSEvt::ProxyDictI& proxyDict, PyObject* arg0, const PSEvt::Source& source, const std::string& key)
{
  /*
   *  get(...) is very overloaded method, here is the list of possible argument combinations:
   *  get(type, src, key:string)
   *  get(type, src)             - equivalent to get(type, src, "")
   *  get(type, key:string)      - equivalent to get(type, Source(None), key)
   *  get(type)                  - equivalent to get(type, Source(None), "")
   *
   *  Type argument can be a type object or a list of type objects.
   *  If the list is given then object is returned whose type matches any one from the list.
   *  The src argument can be an instance of Source or Src types.
   */

  // get the list of types from first argument
  const std::vector<pytools::pyshared_ptr>& types = get_types(arg0, "get");
  if (types.empty()) return 0;

  // loop over types and find first matching object
  ConverterMap& cmap = ConverterMap::instance();
  BOOST_FOREACH(pytools::pyshared_ptr type_ptr, types) {
    // get converters defined for this Python type
    PyTypeObject* pytype = (PyTypeObject*)type_ptr.get();
    std::vector<boost::shared_ptr<Converter> > converters = cmap.getToPyConverters(pytype);
    if (not converters.empty()) {
      // there are converters registered for this type, try all of them
      BOOST_FOREACH(boost::shared_ptr<Converter> cvt, converters) {
        if (PyObject* obj = cvt->convert(proxyDict, source, key)) return obj;
      }
    } else if (pytype == &PyBaseObject_Type) {
      // interested in basic Python object type
      boost::shared_ptr<void> vdata = proxyDict.get(&typeid(const PyObject), source, key, 0);
      if (vdata) {
        PyObject* pyobj = (PyObject*)vdata.get();
        Py_INCREF(pyobj);
        return pyobj;
      }
    }
  }


  Py_RETURN_NONE;
}

/**
 *  Add Python object to event, convert to C++ if possible. If it fails an exception is
 *  raised and zero pointer is returned.
 */
PyObject*
ProxyDictMethods::put(PSEvt::ProxyDictI& proxyDict, PyObject* arg0, const PSEvt::Source& source, const std::string& key)
{
  // get type of python object
  PyTypeObject* pytype = arg0->ob_type;

  // get converters defined for this Python type
  ConverterMap& cmap = ConverterMap::instance();
  std::vector<boost::shared_ptr<Converter> > converters = cmap.getFromPyConverters(pytype);
  if (not converters.empty()) {

    // there are converters registered for this type, try all of them
    BOOST_FOREACH(boost::shared_ptr<Converter> cvt, converters) {
      try {
        if (cvt->convert(arg0, proxyDict, source, key)) {
          Py_RETURN_NONE;
        }
      } catch (const PSEvt::ExceptionDuplicateKey& e) {
        // means already there, we do not allow replacement of C++ objects, raise Python exception
        return 0;
      }
    }

  } else {

    // no converters, store it as Python object, for compatibility we allow
    // replacement of the objects
    pytools::pyshared_ptr optr = pytools::make_pyshared(arg0, false);
    boost::shared_ptr<PSEvt::ProxyI> proxyPtr(boost::make_shared<PSEvt::DataProxy<PyObject> >(optr));
    PSEvt::EventKey evKey(&typeid(const PyObject), source.src(), key);
    try {
      proxyDict.put(proxyPtr, evKey);
    } catch (const PSEvt::ExceptionDuplicateKey& e) {
      // on Python side we allow replacing existing objects
      proxyDict.remove(evKey);
      proxyDict.put(proxyPtr, evKey);
    }

  }

  Py_RETURN_NONE;
}

/**
 *  Remove object from event. If it fails an exception is raised and zero pointer is returned,
 *  but may also throw C++ exception.
 */
PyObject*
ProxyDictMethods::remove(PSEvt::ProxyDictI& proxyDict, PyObject* arg0, const PSEvt::Source& source, const std::string& key)
{
  // first argument is a type or list of types like in get()
  const std::vector<pytools::pyshared_ptr>& types = get_types(arg0, "remove");
  if (types.empty()) return 0;

  bool result = false;

  // loop over types and find first matching object
  ConverterMap& cmap = ConverterMap::instance();
  BOOST_FOREACH(pytools::pyshared_ptr type_ptr, types) {

    // get converters defined for this Python type
    PyTypeObject* pytype = (PyTypeObject*)type_ptr.get();
    std::vector<boost::shared_ptr<Converter> > converters = cmap.getToPyConverters(pytype);
    if (not converters.empty()) {

      // there are converters registered for this type, try all of them
      BOOST_FOREACH(boost::shared_ptr<Converter> cvt, converters) {
        BOOST_FOREACH(const std::type_info* cpptype, cvt->from_cpp_types()) {
          PSEvt::EventKey evKey(cpptype, source.src(), key);
          if (proxyDict.remove(evKey)) return PyBool_FromLong(1L);
        }
      }

    } else if (pytype == &PyBaseObject_Type) {

      PSEvt::EventKey evKey(&typeid(const PyObject), source.src(), key);
      result = proxyDict.remove(evKey);

    }

  }

  return PyBool_FromLong(result);
}

} // namespace psana_python
