#ifndef PSANA_PYTHON_PROXYDICTMETHODS_H
#define PSANA_PYTHON_PROXYDICTMETHODS_H

//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class ProxyDictMethods.
//
//------------------------------------------------------------------------

//-----------------
// C/C++ Headers --
//-----------------
#include "python/Python.h"
#include <utility>
#include <vector>
#include <typeinfo>
#include <boost/shared_ptr.hpp>

//----------------------
// Base Class Headers --
//----------------------

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------
#include "PSEvt/ProxyDictI.h"
#include "PSEvt/AliasMap.h"
#include "psana_python/Source.h"
#include "pytools/make_pyshared.h"
#include "pdsdata/xtc/Src.hh"

//		---------------------
// 		-- Class Interface --
//		---------------------

namespace psana_python {

/// @addtogroup psana_python

/**
 *  @ingroup psana_python
 *
 *  @brief Several methods interfacing ProxyDict methods with Python.
 *
 *  This software was developed for the LCLS project.  If you use all or 
 *  part of it, please give an appropriate acknowledgment.
 *
 *  @version $Id$
 *
 *  @author Andy Salnikov
 */

namespace ProxyDictMethods {

  /**
   *  @brief Return python list of keys from proxy dictionary.
   *
   *  In case of problem set error condition and returnz zero pointer
   *
   *  @param[in] proxyDict  Dictionary instance
   *  @param[in] args       Python argument tuple passed to keys() method
   *
   *  @return Either instance of list type (new reference) or zero pointer for errors.
   */
  PyObject* keys(PSEvt::ProxyDictI& proxyDict, PyObject* args);

  /**
   *  Returns the list types that are passed as a first argument to get() method.
   *  Not strictly a ProxyDict method, but shared between different classes which
   *  deal with ProxyDict. If argument is not good (not a type or not a list of types)
   *  then empty list is returned and Python exception is set.
   *
   *  @param[in] arg0  First argument to get() method
   *  @param[out] method
   *  @return Vector of Python objects
   */
  std::vector<pytools::pyshared_ptr> get_types(PyObject* arg0, const char* method);

  /**
   *  Returns the Src and Key typically passed in arguments 1 & 2 to the get() or put() method.
   *  Not strictly a ProxyDict method, but shared between different classes which
   *  deal with ProxyDict. If both arguments are not present, returns NoSource and an
   *  empty string. If the first argument is a Pds::Src or PSEvt::Source, turns it into the
   *  appropriate Pds::Src - checking for an exact Src match if needExact is true.
   *  If the first argument is a string, again NoSource is returned and the key is this string.
   *
   *  @param[in] args      arguments to the Python function
   *  @param[in] needExact an exact Pds::Src is required, throws error if wildcard found in Source
   *  @param[in] amap      AliasMap to determine exact Pds::Src from alias
   *  @return pair with a Pds::Src and std::string
   */
  std::pair<Pds::Src, std::string>
    arg_get_put(PyObject* args, bool needExact, const PSEvt::AliasMap* amap);

  /**
   *  Implementation of the get method.
   *
   *  get(...) is very overloaded method, here is the list of possible argument combinations:
   *  - get(type, src, key:string)
   *  - get(type, src)             - equivalent to get(type, src, "")
   *  - get(type, key:string)      - equivalent to get(type, Source(None), key)
   *  - get(type)                  - equivalent to get(type, Source(None), "")
   *  Type argument can be a type object or a list of type objects.
   *  If the list is given then object is returned whose type matches any one from the list.
   *  The src argument can be an instance of Source or Src types.
   *
   *  @return New reference, 0 if error occurred.
   */
  PyObject* get(PSEvt::ProxyDictI& proxyDict, PyObject* arg0, const PSEvt::Source& source, const std::string& key);

  /**
   *  Implementation of pyana compatibility get() methods (deprecated):
   *  - get(int, addr:Source)  - equivalent to get(type, addr, "") where type is deduced
   *                           from integer assumed to be Pds::TypeId::Type value
   *  - get(int, addr:string)  - equivalent to get(type, Source(addr), "") where type is deduced
   *                           from integer assumed to be Pds::TypeId::Type value
   *  - get(int)               - equivalent to get(int, "")
   *
   *  @return New reference, 0 if error occurred.
   */
  PyObject* get_compat_typeid(PSEvt::ProxyDictI& proxyDict, PyObject* arg0, PyObject* arg1);


  /**
   *  Implementation of pyana compatibility get() methods (deprecated):
   *  - get(string)            - gets any Python object stored with put(object, string)
   *
   *  @return New reference, 0 if error occurred.
   */
  PyObject* get_compat_string(PSEvt::ProxyDictI& proxyDict, PyObject* arg0);

  /**
   *  Add Python object to event, convert to C++ if possible. If it fails an exception is
   *  raised and zero pointer is returned, but may also throw C++ exception.
   */
  PyObject* put(PSEvt::ProxyDictI& proxyDict, PyObject* arg0, const Pds::Src& source, const std::string& key);

  /**
   *  Remove object from event. If it fails an exception is raised and zero pointer is returned,
   *  but may also throw C++ exception.
   */
  PyObject* remove(PSEvt::ProxyDictI& proxyDict, PyObject* arg0, const Pds::Src& source, const std::string& key);

} // namespace ProxyDictMethods

} // namespace psana_python

#endif // PSANA_PYTHON_PROXYDICTMETHODS_H
