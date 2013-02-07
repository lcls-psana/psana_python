//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class EventKey...
//
// Author List:
//      Andy Salnikov
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "psana_python/EventKey.h"

//-----------------
// C/C++ Headers --
//-----------------
#include <cxxabi.h>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "psana_python/PdsBldInfo.h"
#include "psana_python/PdsDetInfo.h"
#include "psana_python/PdsProcInfo.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

namespace {

  // type-specific methods
  PyObject* EventKey_type(PyObject* self, PyObject*);
  PyObject* EventKey_typeName(PyObject* self, PyObject*);
  PyObject* EventKey_src(PyObject* self, PyObject*);
  PyObject* EventKey_key(PyObject* self, PyObject*);

  PyMethodDef methods[] = {
    { "type",       EventKey_type,     METH_NOARGS, "self.type() -> type\n\nReturns Python type (class) or None if type cannot be deduced." },
    { "typeName",   EventKey_typeName, METH_NOARGS,
        "self.typeName() -> string\n\nDEPRECATED: Returns name of the corresponding C++ class. This method is likely to be removed in the future." },
    { "src",        EventKey_src,      METH_NOARGS, "self.src() -> object\n\nReturns data source address ((:py:class:`Src`)." },
    { "key",        EventKey_key,      METH_NOARGS, "self.key() -> string\n\nReturns string key." },
    {0, 0, 0, 0}
   };

  char typedoc[] = "Class describing a key of the data object in event.";

}

//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------

void
psana_python::EventKey::initType(PyObject* module)
{
  PyTypeObject* type = BaseType::typeObject() ;
  type->tp_doc = ::typedoc;
  type->tp_methods = ::methods;

  BaseType::initType("EventKey", module, "psana");
}

namespace {

PyObject*
EventKey_type(PyObject* self, PyObject* )
{
  // TODO: need something more specific here
  Py_RETURN_NONE;
}

PyObject*
EventKey_typeName(PyObject* self, PyObject* )
{
  PSEvt::EventKey& cself = psana_python::EventKey::cppObject(self);
  int status;
  const char* typeName = abi::__cxa_demangle(cself.typeinfo()->name(), 0, 0, &status);
  return PyString_FromString(typeName ? typeName : "");
}

PyObject*
EventKey_src(PyObject* self, PyObject* )
{
  PSEvt::EventKey& cself = psana_python::EventKey::cppObject(self);
  // TODO: need something more specific here
  if (cself.src().level() == Pds::Level::Source)
    return psana_python::PdsDetInfo::PyObject_FromCpp(static_cast<const Pds::DetInfo&>(cself.src()));
  else if (cself.src().level() == Pds::Level::Reporter)
    return psana_python::PdsBldInfo::PyObject_FromCpp(static_cast<const Pds::BldInfo&>(cself.src()));
  else
    return psana_python::PdsProcInfo::PyObject_FromCpp(static_cast<const Pds::ProcInfo&>(cself.src()));
}

PyObject*
EventKey_key(PyObject* self, PyObject* )
{
  PSEvt::EventKey& cself = psana_python::EventKey::cppObject(self);
  return PyString_FromString(cself.key().c_str());
}

}
