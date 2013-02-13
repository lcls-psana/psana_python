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
#include "psddl_python/ConverterMap.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

using psddl_python::ConverterMap;
using psddl_python::Converter;

namespace {

  std::string type_name(PyTypeObject*);

  // type-specific methods
  PyObject* EventKey_type(PyObject* self, PyObject*);
  PyObject* EventKey_src(PyObject* self, PyObject*);
  PyObject* EventKey_key(PyObject* self, PyObject*);

  PyMethodDef methods[] = {
    { "type",       EventKey_type,     METH_NOARGS, "self.type() -> type\n\nReturns Python type (class) or None if type cannot be deduced." },
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

// Dump object info to a stream
void 
psana_python::EventKey::print(std::ostream& out) const 
{
  // get Python type name
  std::string type = "None";
  if (const std::type_info* typeinfo = m_obj.typeinfo()) {
    const boost::shared_ptr<Converter>& cvt = ConverterMap::instance().getConverter(typeinfo);
    if (cvt) {
      PyTypeObject* tobj = cvt->pyTypeObject();
      type = ::type_name(tobj);
    }
  }

  out << "EventKey(type=" << type;

  if (m_obj.src() == PSEvt::EventKey::anySource()) {
    out << ", src=AnySource";
  } else if (m_obj.validSrc()) {
    out << ", src='" << m_obj.src() << '\'';
  }

  if (not m_obj.key().empty()) {
    out << ", key='" << m_obj.key() << '\'';
  }
  
  out << ')';
}

namespace {

std::string 
type_name(PyTypeObject* type)
{
  std::string name = type->tp_name;
  if (name.find('.') != std::string::npos) return name;

  if (type->tp_flags & Py_TPFLAGS_HEAPTYPE) {
    if(PyObject *mod = PyDict_GetItemString(type->tp_dict, "__module__")) {
      name = PyString_AsString(mod);
      name += ".";
      name += type->tp_name;
    }    
  }
  return name;
}

PyObject*
EventKey_type(PyObject* self, PyObject* )
{
  PSEvt::EventKey& cself = psana_python::EventKey::cppObject(self);
  if (const std::type_info* typeinfo = cself.typeinfo()) {
    const boost::shared_ptr<Converter>& cvt = ConverterMap::instance().getConverter(typeinfo);
    if (cvt) {
      PyObject* res = (PyObject*)cvt->pyTypeObject();
      Py_INCREF(res);
      return res;
    }
  }
  Py_RETURN_NONE;
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
