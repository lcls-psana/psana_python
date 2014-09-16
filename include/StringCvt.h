#ifndef PSANA_PYTHON_STRINGCVT_H
#define PSANA_PYTHON_STRINGCVT_H

//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class Stringcvt.
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
namespace psddl_python {
class ConverterMap;
}



//		---------------------
// 		-- Class Interface --
//		---------------------

namespace psana_python {

/// @addtogroup psana_python

/**
 *  @ingroup psana_python
 *
 *  @brief Implementation of converter for C++ std::string
 *
 *  @note This software was developed for the LCLS project.  If you use all or 
 *  part of it, please give an appropriate acknowledgment.
 *
 *  @version $Id$
 *
 *  @author David Schneider
 */

class StringCvt : public psddl_python::Converter {
public:

  // Default constructor
  StringCvt () ;

  // Destructor
  virtual ~StringCvt () ;

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

  // Data members


};

} // namespace psana_python

#endif // PSANA_PYTHON_STRINGCVT_H
