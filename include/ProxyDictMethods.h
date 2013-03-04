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
#include "psana_python/Source.h"
#include "pytools/make_pyshared.h"

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
  PyObject* keys(const boost::shared_ptr<PSEvt::ProxyDictI>& proxyDict, PyObject* args);

  /*
   *  Returns the list types that are passed as a first argument to get() method.
   *  Not strictly a ProxyDict method, but shared between different classes which
   *  deal with ProxyDict.
   *
   *  @param[in] arg0  First argument to get() method
   *  @return Vector of Python objects
   */
  std::vector<pytools::pyshared_ptr> get_types(PyObject* arg0);

  /**
   *  Extract C++ type_info object from Python type, return 0 if cannot be done.
   */
  const std::type_info* get_type_info(PyObject* type);

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
  PyObject* get(const boost::shared_ptr<PSEvt::ProxyDictI>& proxyDict, PyObject* arg0, 
      const PSEvt::Source& source, const std::string& key);

  /**
   *  Implementation of pyana compatibility get() methods (deprecated):
   *  - get(int, addr:string)  - equivalent to get(type, Source(addr), "") where type is deduced
   *                           from integer assumed to be Pds::TypeId::Type value
   *  - get(int)               - equivalent to get(int, "")
   *
   *  @return New reference, 0 if error occurred.
   */
  PyObject* get_compat_typeid(const boost::shared_ptr<PSEvt::ProxyDictI>& proxyDict, PyObject* arg0, PyObject* arg1);


  /**
   *  Implementation of pyana compatibility get() methods (deprecated):
   *  - get(string)            - gets any Python object stored with put(object, string)
   *
   *  @return New reference, 0 if error occurred.
   */
  PyObject* get_compat_string(const boost::shared_ptr<PSEvt::ProxyDictI>& proxyDict, PyObject* arg0);


} // namespace ProxyDictMethods

} // namespace psana_python

#endif // PSANA_PYTHON_PROXYDICTMETHODS_H
