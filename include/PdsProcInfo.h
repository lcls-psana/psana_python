#ifndef PSANA_PYTHON_PDSPROCINFO_H
#define PSANA_PYTHON_PDSPROCINFO_H

//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class PdsProcInfo.
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
#include "pdsdata/xtc/ProcInfo.hh"
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
 *  @brief Wrapper class for Pds::ProcInfo.
 *
 *  This software was developed for the LCLS project.  If you use all or 
 *  part of it, please give an appropriate acknowledgment.
 *
 *  @version $Id$
 *
 *  @author Andy Salnikov
 */

class PdsProcInfo : public pytools::PyDataType<PdsProcInfo, Pds::ProcInfo> {
public:

  typedef pytools::PyDataType<PdsProcInfo, Pds::ProcInfo> BaseType;

  /// Initialize Python type and register it in a module
  static void initType( PyObject* module );

  // Dump object info to a stream
  void print(std::ostream& out) const {
    out << static_cast<const Pds::Src&>(m_obj);
  }

};

} // namespace psana_python

#endif // PSANA_PYTHON_PDSPROCINFO_H
