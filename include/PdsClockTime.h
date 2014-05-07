#ifndef PSANA_PYTHON_PDSCLOCKTIME_H
#define PSANA_PYTHON_PDSCLOCKTIME_H

//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class PdsClockTime
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
#include "pdsdata/xtc/ClockTime.hh"

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
 *  @brief Wrapper class for Pds::ClockTime.
 *
 *  This software was developed for the LCLS project.  If you use all or 
 *  part of it, please give an appropriate acknowledgment.
 *
 *  @version $Id$
 *
 *  @author David Schneider
 */

class PdsClockTime : public pytools::PyDataType<PdsClockTime, Pds::ClockTime> {
public:

  typedef pytools::PyDataType<PdsClockTime, Pds::ClockTime> BaseType;

  /// Initialize Python type and register it in a module
  static void initType( PyObject* module );

  // Dump object info to a stream
  void print(std::ostream& out) const {
    out << "seconds: " << m_obj.seconds() << " nano:" << m_obj.nanoseconds();
  }

};

} // namespace psana_python

#endif // PSANA_PYTHON_PDSCLOCKTIME_H
