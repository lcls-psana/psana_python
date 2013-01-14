//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class Exceptions...
//
// Author List:
//      Andy Salnikov
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "psana_python/Exceptions.h"

//-----------------
// C/C++ Headers --
//-----------------

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------

namespace psana_python {

ExceptionPyLoadError::ExceptionPyLoadError(const ErrSvc::Context& ctx, const std::string& what)
  : Exception(ctx, what)
{
}

ExceptionGenericPyError::ExceptionGenericPyError(const ErrSvc::Context& ctx, const std::string& what)
  : Exception(ctx, what)
{
}


} // namespace psana_python
