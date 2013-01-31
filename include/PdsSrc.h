#ifndef PSANA_PYTHON_PDSSRC_H
#define PSANA_PYTHON_PDSSRC_H

//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class PdsSrc.
//
//------------------------------------------------------------------------

//-----------------
// C/C++ Headers --
//-----------------

//----------------------
// Base Class Headers --
//----------------------
#include "pytools/PyDataType.h"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "pdsdata/xtc/Src.hh"
#include "PSEvt/EventKey.h"   // needed for operator<<

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
 *  @brief Wrapper class for Pds::Src.
 *
 *  @version $Id$
 *
 *  @author Andy Salnikov
 */

class PdsSrc : public pytools::PyDataType<PdsSrc, Pds::Src> {
public:

  typedef pytools::PyDataType<PdsSrc, Pds::Src> BaseType;

  /// Initialize Python type and register it in a module
  static void initType( PyObject* module );

  // Dump object info to a stream
  void print(std::ostream& out) const {
    out << m_obj;
  }

};

} // namespace psana_python

#endif // PSANA_PYTHON_PDSSRC_H
