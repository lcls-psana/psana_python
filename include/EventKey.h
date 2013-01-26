#ifndef PSANA_PYTHON_EVENTKEY_H
#define PSANA_PYTHON_EVENTKEY_H

//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class EventKey.
//
//------------------------------------------------------------------------

//-----------------
// C/C++ Headers --
//-----------------

//----------------------
// Base Class Headers --
//----------------------
#include "psana_python/PyDataType.h"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "PSEvt/EventKey.h"

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
 *  @brief Wrapper class for EventKey.
 *
 *  This software was developed for the LCLS project.  If you use all or 
 *  part of it, please give an appropriate acknowledgment.
 *
 *  @version $Id$
 *
 *  @author Andy Salnikov
 */

class EventKey : public PyDataType<EventKey, PSEvt::EventKey> {
public:

  typedef PyDataType<EventKey, PSEvt::EventKey> BaseType;

  /// Initialize Python type and register it in a module
  static void initType( PyObject* module );

  // Dump object info to a stream
  void print(std::ostream& out) const {
    out << m_obj;
  }

};

} // namespace psana_python

#endif // PSANA_PYTHON_EVENTKEY_H
