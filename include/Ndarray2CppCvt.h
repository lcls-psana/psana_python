#ifndef PSANA_PYTHON_NDARRAY2CPPCVT_H
#define PSANA_PYTHON_NDARRAY2CPPCVT_H

//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class Ndarray2CppCvt.
//
//------------------------------------------------------------------------

//-----------------
// C/C++ Headers --
//-----------------

//----------------------
// Base Class Headers --
//----------------------
#include "psddl_python/Converter.h"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------

//		---------------------
// 		-- Class Interface --
//		---------------------

namespace psana_python {

/// @addtogroup psana_python

/**
 *  @ingroup psana_python
 *
 *  @brief Implementation of converters for C++ ndarray type.
 *
 *  This class is responsible for conversion of Python numpy.ndarray type
 *  into C++ ndarray. For opposite direction check NdarrayCvt class.
 *
 *  @note This software was developed for the LCLS project.  If you use all or 
 *  part of it, please give an appropriate acknowledgment.
 *
 *  @version $Id$
 *
 *  @author Andy Salnikov
 */

class Ndarray2CppCvt : public psddl_python::Converter {
public:

  // Default constructor
  Ndarray2CppCvt () ;

  // Destructor
  virtual ~Ndarray2CppCvt () ;

  /**
   *  @brief Return type_infos of source C++ types.
   *
   *  If converter supports conversion from C++ to Python
   *  this method shall return non-empty vector.
   */
  virtual std::vector<const std::type_info*> from_cpp_types() const;

  /**
   *  @brief Returns source Python types.
   *
   *  If converter supports conversion from Python to C++
   *  this method shall return non-empty vector.
   *
   *  @return Borrowed references
   */
  virtual std::vector<PyTypeObject*> from_py_types() const;

  /**
   *  @brief Returns destination Python types.
   *
   *  If converter supports conversion from C++ to Python
   *  this method shall return non-empty vector.
   *
   *  @return Borrowed references
   */
  virtual std::vector<PyTypeObject*> to_py_types() const;

  /**
   *  @brief Convert C++ object to Python
   *
   *  @return New reference
   */
  virtual PyObject* convert(PSEvt::ProxyDictI& proxyDict, const PSEvt::Source& source, const std::string& key) const;

  /**
   *  @brief Convert Python object to C++
   *
   *  @return True for successful conversion, false otherwise
   */
  virtual bool convert(PyObject* obj, PSEvt::ProxyDictI& proxyDict, const Pds::Src& source, const std::string& key) const;

protected:

private:

};

} // namespace psana_python

#endif // PSANA_PYTHON_NDARRAY2CPPCVT_H
