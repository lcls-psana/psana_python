#ifndef PSANA_PYTHON_NDARRAYCVT_H
#define PSANA_PYTHON_NDARRAYCVT_H

//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class NdarrayCvt.
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
 *  @brief Implementation of converters for C++ ndarray type.
 *
 *  @note This software was developed for the LCLS project.  If you use all or 
 *  part of it, please give an appropriate acknowledgment.
 *
 *  @version $Id$
 *
 *  @author Andy Salnikov
 */

template <typename T, unsigned Rank>
class NdarrayCvt : public psddl_python::Converter {
public:

  // Default constructor
  NdarrayCvt (PyObject* module) ;

  // Destructor
  virtual ~NdarrayCvt () ;

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

protected:

private:

  // Data members


};

/**
 *  Method that registers converters for all supported types, this will also
 *  create all necessary data types.
 */
void initNdarrayCvt(psddl_python::ConverterMap& cmap, PyObject* module);

} // namespace psana_python

#endif // PSANA_PYTHON_NDARRAYCVT_H
