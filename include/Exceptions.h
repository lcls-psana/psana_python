#ifndef PSANA_PYTHON_EXCEPTIONS_H
#define PSANA_PYTHON_EXCEPTIONS_H

//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class Exceptions.
//
//------------------------------------------------------------------------

//-----------------
// C/C++ Headers --
//-----------------

//----------------------
// Base Class Headers --
//----------------------
#include "ErrSvc/Issue.h"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

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
 * @brief Base class for exceptions for psana package.
 *
 *  This software was developed for the LCLS project.  If you use all or 
 *  part of it, please give an appropriate acknowledgment.
 *
 *  @version $Id$
 *
 *  @author Andy Salnikov
 */

class Exception : public ErrSvc::Issue {
public:

  /// Constructor takes the reason for an exception
  Exception ( const ErrSvc::Context& ctx, const std::string& what )
    : ErrSvc::Issue( ctx, "psana_python::Exception: " + what )
  {}

};

/// Exception thrown for Python import errors.
class ExceptionPyLoadError : public Exception {
public:

  /// Constructor takes the reason for an exception
  ExceptionPyLoadError(const ErrSvc::Context& ctx, const std::string& what);

};

/// Exception thrown for Python import errors.
class ExceptionGenericPyError : public Exception {
public:

  /// Constructor takes the reason for an exception
  ExceptionGenericPyError(const ErrSvc::Context& ctx, const std::string& what);

};

} // namespace psana_python

#endif // PSANA_PYTHON_EXCEPTIONS_H
