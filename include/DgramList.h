#ifndef PSANA_PYTHON_DGRAMLIST_H
#define PSANA_PYTHON_DGRAMLIST_H

//--------------------------------------------------------------------------
// File and Version Information:
//      $Id$
//
// Description:
//      Class DgramList.
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
#include "XtcInput/DgramList.h"

//    ---------------------
//    -- Class Interface --
//    ---------------------

namespace psana_python {

/// @addtogroup psana_python

/**
 *  @ingroup psana_python
 *
 *  @brief Wrapper class for DgramList.
 *  
 *  This software was developed for the LUSI project.  If you use all or
 *  part of it, please give an appropriate acknowledgment.
 *
 *  @version $Id: DgramList.h 7817 2014-03-09 07:50:00Z salnikov@SLAC.STANFORD.EDU $
 *
 *  @author Andrei Salnikov
 */

class DgramList : public pytools::PyDataType<DgramList, boost::shared_ptr<XtcInput::DgramList> > {
public:

  typedef pytools::PyDataType<DgramList, boost::shared_ptr<XtcInput::DgramList> > BaseType;

  /// Initialize Python type and register it in a module
  static void initType( PyObject* module );

  // Dump object info to a stream
  void print(std::ostream& out) const;
};

} // namespace psana_python

#endif // PSANA_PYTHON_ALIASMAP_H
