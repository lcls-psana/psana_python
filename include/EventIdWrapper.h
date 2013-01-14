#ifndef PSANA_PYTHON_EVENTIDWRAPPER_H
#define PSANA_PYTHON_EVENTIDWRAPPER_H

//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class EventIdWrapper.
//
//------------------------------------------------------------------------

//-----------------
// C/C++ Headers --
//-----------------
#include <boost/shared_ptr.hpp>
#include <boost/python.hpp>
#include <boost/python/tuple.hpp>

//----------------------
// Base Class Headers --
//----------------------

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

class EventIdWrapper {
public:

  // Default constructor
  EventIdWrapper(const boost::shared_ptr<PSEvt::EventId>& evtId) : m_evtId(evtId) {}

  boost::python::tuple time() const;
  int run() const { return m_evtId->run(); }
  unsigned fiducials() const { return m_evtId->fiducials(); }
  unsigned vector() const { return m_evtId->vector(); }

protected:

private:

  // Data members
  boost::shared_ptr<PSEvt::EventId> m_evtId;

};

} // namespace psana_python

#endif // PSANA_PYTHON_EVENTIDWRAPPER_H
