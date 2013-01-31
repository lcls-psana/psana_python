#ifndef PSANA_PYTHON_PYEXT_DATASOURCE_H
#define PSANA_PYTHON_PYEXT_DATASOURCE_H

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
#include "psana/DataSource.h"

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

class DataSource : public pytools::PyDataType<DataSource, psana::DataSource> {
public:

  typedef pytools::PyDataType<DataSource, psana::DataSource> BaseType;

  /// Initialize Python type and register it in a module
  static void initType( PyObject* module );

};

} // namespace pyext
} // namespace psana_python

#endif // PSANA_PYTHON_PYEXT_DATASOURCE_H
