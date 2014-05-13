#ifndef PSANA_PYTHON_PYEXT_EVENTTIME_H
#define PSANA_PYTHON_PYEXT_EVENTTIME_H

//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: EventTime.h 5268 2013-01-31 20:16:06Z salnikov@SLAC.STANFORD.EDU $
//
// Description:
//	Class BldInfo.
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

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------
#include "psana/Index.h"

//    ---------------------
//    -- Class Interface --
//    ---------------------

namespace psana_python {
namespace pyext {

/**
 *  This software was developed for the LUSI project.  If you use all or
 *  part of it, please give an appropriate acknowledgment.
 *
 *  @version $Id: EventTime.h 5268 2013-01-31 20:16:06Z salnikov@SLAC.STANFORD.EDU $
 *
 *  @author Andrei Salnikov
 */

class EventTime : public pytools::PyDataType<EventTime, psana::EventTime> {
public:

  typedef pytools::PyDataType<EventTime, psana::EventTime> BaseType;

  /// Initialize Python type and register it in a module
  static void initType( PyObject* module );

};

} // namespace pyext
} // namespace psana

#endif // PSANA_PYTHON_PYEXT_EVENTTIME_H
