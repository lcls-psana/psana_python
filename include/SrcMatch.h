#ifndef PSANA_PYTHON_SrcMatch_H
#define PSANA_PYTHON_SrcMatch_H

//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class SrcMatch.
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
#include "PSEvt/Source.h"

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------

//              ---------------------
//              -- Class Interface --
//              ---------------------

namespace psana_python {

/// @addtogroup psana_python

/**
 *  @ingroup psana_python
 *
 *  @brief Wrapper class for PSEvt::SrcMatch.
 *
 *  This software was developed for the LCLS project.  If you use all or 
 *  part of it, please give an appropriate acknowledgment.
 *
 *  @version $Id$
 *
 *  @author Andy Salnikov
 */

class SrcMatch : public pytools::PyDataType<SrcMatch, PSEvt::Source::SrcMatch> {
public:

  typedef pytools::PyDataType<SrcMatch, PSEvt::Source::SrcMatch> BaseType;

  /// Initialize Python type and register it in a module
  static void initType( PyObject* module );

  // Dump object info to a stream
  void print(std::ostream& out) const ;
};

} // namespace psana_python

#endif // PSANA_PYTHON_SrcMatch_H
