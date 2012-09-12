//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Python module _psana...
//
// Author List:
//      Andrei Salnikov
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------

//-----------------
// C/C++ Headers --
//-----------------
#include "python/Python.h"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "DataSource.h"
#include "EventIter.h"
#include "PSAna.h"
#include "Run.h"
#include "RunIter.h"
#include "Scan.h"
#include "ScanIter.h"
#include "psana_python/CreateWrappers.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------


//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------

// Module entry point
extern "C"
PyMODINIT_FUNC init_psana()
{
  // Initialize the module
  PyObject* module = Py_InitModule3( "_psana", 0, "The Python module for psana" );
  psana_python::pyext::DataSource::initType( module );
  psana_python::pyext::EventIter::initType( module );
  psana_python::pyext::PSAna::initType( module );
  psana_python::pyext::Run::initType( module );
  psana_python::pyext::RunIter::initType( module );
  psana_python::pyext::Scan::initType( module );
  psana_python::pyext::ScanIter::initType( module );

  psana_python::createWrappers();
}
