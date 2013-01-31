#ifndef PSANA_PYTHON_PYEXT_EVENTITER_H
#define PSANA_PYTHON_PYEXT_EVENTITER_H

//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
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
#include "psana/EventIter.h"

//    ---------------------
//    -- Class Interface --
//    ---------------------

namespace psana_python {
namespace pyext {

/**
 *  This software was developed for the LUSI project.  If you use all or
 *  part of it, please give an appropriate acknowledgment.
 *
 *  @version $Id$
 *
 *  @author Andrei Salnikov
 */

class EventIter : public pytools::PyDataType<EventIter, psana::EventIter> {
public:

  typedef pytools::PyDataType<EventIter, psana::EventIter> BaseType;

  /// Initialize Python type and register it in a module
  static void initType( PyObject* module );

};

} // namespace pyext
} // namespace psana

#endif // PSANA_PYTHON_PYEXT_EVENTITER_H
