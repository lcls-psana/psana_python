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

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "pdsdata/xtc/TypeId.hh"
#include "psana_python/EventKey.h"
#include "psddl_python/ConverterMap.h"

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
ProxyDictMethods::keys(const boost::shared_ptr<PSEvt::ProxyDictI>& proxyDict, PyObject* args)
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
  proxyDict->keys(keys, src);

  // convert keys to python objects
  PyObject* pykeys = PyList_New(keys.size());
  int i = 0;
  for (std::list<PSEvt::EventKey>::const_iterator it = keys.begin(); it != keys.end(); ++ it, ++ i) {
    PyList_SET_ITEM(pykeys, i, psana_python::EventKey::PyObject_FromCpp(*it));
  }
  return pykeys;
}

std::vector<pytools::pyshared_ptr>
ProxyDictMethods::get_types(PyObject* arg0)
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
    PyErr_SetString(PyExc_TypeError, "Event.get(...) expecting a sequence of types, received empty sequence");
    return types;
  }

  // make sure that they are all types
  for (std::vector<pytools::pyshared_ptr>::const_iterator it = types.begin(); it != types.end(); ++ it) {
    if (not PyType_Check(it->get())) {
      PyErr_SetString(PyExc_TypeError, "Event.get(...) argument must be type");
      types.clear();
      break;
    }
  }

  return types;
}

const std::type_info*
ProxyDictMethods::get_type_info(PyObject* type)
{
  // Does it have __typeid__ method?
  if (PyObject_HasAttrString(type, "__typeid__")) {
    pytools::pyshared_ptr typeid_pyobj = pytools::make_pyshared(PyObject_CallMethod(type, "__typeid__", 0));
    if (PyCObject_Check(typeid_pyobj.get())) {
      return static_cast<std::type_info*>(PyCObject_AsVoidPtr(typeid_pyobj.get()));
    }
  }

  return 0;
}


PyObject*
ProxyDictMethods::get_compat_string(const boost::shared_ptr<PSEvt::ProxyDictI>& proxyDict, PyObject* arg0)
{
  /*
   *  pyana compatibility method (deprecated):
   *  get(string)            - gets any Python object stored with put(object, string)
   */

  std::string key(PyString_AsString(arg0));

  // get any PyObject with no address and a key
  boost::shared_ptr<void> vdata = proxyDict->get(&typeid(PyObject), PSEvt::Source(PSEvt::Source::null), key, 0);
  if (vdata) {
    PyObject* pyobj = (PyObject*)vdata.get();
    Py_INCREF(pyobj);
    return pyobj;
  }

  Py_RETURN_NONE;
}

PyObject*
ProxyDictMethods::get_compat_typeid(const boost::shared_ptr<PSEvt::ProxyDictI>& proxyDict, PyObject* arg0, PyObject* arg1)
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
  const std::vector<const std::type_info*>& typeids = cmap.pdsTypeInfos(pdsTypeId);
  for (std::vector<const std::type_info*>::const_iterator it = typeids.begin(); it != typeids.end(); ++ it) {
    // need converter for this type
    boost::shared_ptr<Converter> cvt = cmap.getConverter(*it);
    if (cvt) {
      boost::shared_ptr<void> vdata = proxyDict->get(*it, source, std::string(), 0);
      if (vdata) {
        return cvt->convert(vdata);
      }
    }
  }

  Py_RETURN_NONE;
}

PyObject*
ProxyDictMethods::get(const boost::shared_ptr<PSEvt::ProxyDictI>& proxyDict, PyObject* arg0, 
    const PSEvt::Source& source, const std::string& key)
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
  const std::vector<pytools::pyshared_ptr>& types = get_types(arg0);
  if (types.empty()) return 0;

  // loop over types and find first matching object
  for (std::vector<pytools::pyshared_ptr>::const_iterator it = types.begin(); it != types.end(); ++ it) {

    const std::type_info* cpp_type = get_type_info(it->get());
    if (not cpp_type) {
      // interested in Python type which has no C++ counterpart, we should
      // allow that in the future and do something smart (like converting
      // ndarray to numpy), for now just return any Python object.
      const boost::shared_ptr<void>& vdata = proxyDict->get(&typeid(PyObject), source, key, 0);
      if (vdata) {
        PyObject* obj = static_cast<PyObject*>(vdata.get());
        Py_INCREF(obj);
        return obj;
      }
      Py_RETURN_NONE;
    }

    // call C++ method
    ConverterMap& cmap = ConverterMap::instance();
    const boost::shared_ptr<Converter>& cvt = cmap.getConverter(cpp_type);
    if (cvt) {
      const boost::shared_ptr<void>& vdata = proxyDict->get(cpp_type, source, key, 0);
      // found something, need converter now
      if (vdata) {
        return cvt->convert(vdata);
      }
    }

  }

  Py_RETURN_NONE;
}

} // namespace psana_python
