#ifndef PSANA_PYTHON_EVENTID_H
#define PSANA_PYTHON_EVENTID_H

//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class EventId.
//
//------------------------------------------------------------------------

//-----------------
// C/C++ Headers --
//-----------------
#include "python/Python.h"
#include <boost/shared_ptr.hpp>

//----------------------
// Base Class Headers --
//----------------------
#include "pytools/PyDataType.h"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "PSEvt/EventId.h"

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
 *  @brief Wrapper class for EventId.
 *
 *  This software was developed for the LCLS project.  If you use all or 
 *  part of it, please give an appropriate acknowledgment.
 *
 *  @version $Id$
 *
 *  @author Andy Salnikov
 */

class EventId : public pytools::PyDataType<EventId, boost::shared_ptr<PSEvt::EventId> > {
public:

  typedef pytools::PyDataType<EventId, boost::shared_ptr<PSEvt::EventId> > BaseType;

  /// Initialize Python type and register it in a module
  static void initType( PyObject* module );

  // Dump object info to a stream
  void print(std::ostream& out) const {
    if (m_obj) out << *m_obj;
  }
};

} // namespace psana_python

#endif // PSANA_PYTHON_EVENTID_H
