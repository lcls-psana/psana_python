//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class PdsSrc...
//
// Author List:
//      Andy Salnikov
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "psana_python/PdsSrc.h"

//-----------------
// C/C++ Headers --
//-----------------

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

namespace {

  // type-specific methods
  PyObject* PdsSrc_level(PyObject* self, PyObject*);
  PyObject* PdsSrc_log(PyObject* self, PyObject*);
  PyObject* PdsSrc_phy(PyObject* self, PyObject*);

  PyMethodDef methods[] = {
    { "level",     PdsSrc_level,   METH_NOARGS, "self.level() -> int\n\nReturns integer number." },
    { "log",       PdsSrc_log,     METH_NOARGS, "self.log() -> int\n\nReturns integer number." },
    { "phy",       PdsSrc_phy,     METH_NOARGS, "self.phy() -> int\n\nReturns integer number." },
    {0, 0, 0, 0}
   };

  char typedoc[] = "Base class which defines generic XTC data source \"address\". "
      "One cannot make instances of this class directly, instead use one of the "
      "classes :py:class:`DetInfo`, :py:class:`BldInfo`, or :py:class:`ProcInfo`";

}

//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------

void
psana_python::PdsSrc::initType(PyObject* module)
{
  PyTypeObject* type = BaseType::typeObject() ;
  type->tp_doc = ::typedoc;
  type->tp_methods = ::methods;

  BaseType::initType("Src", module, "psana");
}

namespace {

PyObject*
PdsSrc_level(PyObject* self, PyObject* )
{
  Pds::Src& cself = psana_python::PdsSrc::cppObject(self);
  return PyInt_FromLong(cself.level());
}

PyObject*
PdsSrc_log(PyObject* self, PyObject* )
{
  Pds::Src& cself = psana_python::PdsSrc::cppObject(self);
  return PyInt_FromLong(cself.log());
}

PyObject*
PdsSrc_phy(PyObject* self, PyObject* )
{
  Pds::Src& cself = psana_python::PdsSrc::cppObject(self);
  return PyInt_FromLong(cself.phy());
}

}
