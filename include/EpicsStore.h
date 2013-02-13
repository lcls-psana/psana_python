#ifndef PSANA_PYTHON_EPICSSTORE_H
#define PSANA_PYTHON_EPICSSTORE_H

//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class EpicsStore.
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
#include "PSEnv/EpicsStore.h"

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
 *  @brief Python wrapper for EpicsStore class.
 *
 *  This software was developed for the LCLS project.  If you use all or 
 *  part of it, please give an appropriate acknowledgment.
 *
 *  @version $Id$
 *
 *  @author Andy Salnikov
 */

class EpicsStore : public pytools::PyDataType<EpicsStore, boost::shared_ptr<PSEnv::EpicsStore> > {
public:

  typedef pytools::PyDataType<EpicsStore, boost::shared_ptr<PSEnv::EpicsStore> > BaseType;

  /// Initialize Python type and register it in a module
  static void initType( PyObject* module );

  // Dump object info to a stream
  void print(std::ostream& out) const {
    out << "EpicsStore()";
  }

};

} // namespace psana_python

#endif // PSANA_PYTHON_EPICSSTORE_H
