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
#include "Step.h"
#include "StepIter.h"
#include "EventTime.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

#if PY_MAJOR_VERSION >= 3
#define IS_PY3K
#endif

#ifdef IS_PY3K
#define DDL_CREATE_MODULE(m_name, m_methods, m_doc) \
  static struct PyModuleDef moduledef = { \
    PyModuleDef_HEAD_INIT, \
    "(m_name)",\
    "(m_doc)", \
    -1,                  \
    (m_methods),                    \
    NULL,                 \
    NULL,                 \
    NULL,                \
    NULL,                \
  }; \
  PyObject* module = PyModule_Create(&moduledef)
#else
#define DDL_CREATE_MODULE(m_name, m_methods, m_doc) \
  PyObject* module = Py_InitModule3( "(m_name)", (m_methods), "(m_doc)")
#endif

namespace psana_python {

  // defined in src/CreateWrappers.cpp
  extern void createWrappers(PyObject* module);

}

//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------

// Module entry point
extern "C"
#ifdef IS_PY3K
PyMODINIT_FUNC PyInit__psana()
#else
PyMODINIT_FUNC init_psana()
#endif
{
  // Initialize the module
  DDL_CREATE_MODULE( "_psana", 0, "The Python module for psana" );
  psana_python::pyext::DataSource::initType( module );
  psana_python::pyext::EventIter::initType( module );
  psana_python::pyext::PSAna::initType( module );
  psana_python::pyext::Run::initType( module );
  psana_python::pyext::RunIter::initType( module );
  psana_python::pyext::Step::initType( module );
  psana_python::pyext::StepIter::initType( module );
  psana_python::pyext::EventTime::initType( module );

  psana_python::createWrappers(module);

#ifdef IS_PY3K
  return module;
#endif
}

