#ifndef PSANA_PYTHON_EVENT_H
#define PSANA_PYTHON_EVENT_H

//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: Event.h 5268 2013-01-31 20:16:06Z salnikov@SLAC.STANFORD.EDU $
//
// Description:
//      Class Event.
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

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------
#include "PSEvt/Event.h"

//    ---------------------
//    -- Class Interface --
//    ---------------------

namespace psana_python {

/// @addtogroup psana_python

/**
 *  @ingroup psana_python
 *
 *  @brief Wrapper class for Event.
 *  
 *  This software was developed for the LUSI project.  If you use all or
 *  part of it, please give an appropriate acknowledgment.
 *
 *  @version $Id: PSAna.h 5268 2013-01-31 20:16:06Z salnikov@SLAC.STANFORD.EDU $
 *
 *  @author Andrei Salnikov
 */

class Event : public pytools::PyDataType<Event, boost::shared_ptr<PSEvt::Event> > {
public:

  typedef pytools::PyDataType<Event, boost::shared_ptr<PSEvt::Event> > BaseType;

  /// Initialize Python type and register it in a module
  static void initType( PyObject* module );

  // Dump object info to a stream
  void print(std::ostream& out) const;
};

} // namespace psana_python

#endif // PSANA_PYTHON_EVENT_H
