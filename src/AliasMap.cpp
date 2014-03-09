//--------------------------------------------------------------------------
// File and Version Information:
//  $Id$
//
// Description:
//  Class AliasMap...
//
// Author List:
//      Andrei Salnikov
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "psana_python/AliasMap.h"

//-----------------
// C/C++ Headers --
//-----------------
#include <iostream>
#include <vector>
#include <typeinfo>
#include <utility>
#include <boost/python.hpp>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "psana_python/PdsSrc.h"
#include "pytools/make_pyshared.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

using namespace psana_python;

namespace {

  // standard Python stuff
  PyObject* AliasMap_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds);

  // type-specific methods
  PyObject* AliasMap_add(PyObject* self, PyObject* args);
  PyObject* AliasMap_clear(PyObject* self, PyObject* args);
  PyObject* AliasMap_src(PyObject* self, PyObject* args);
  PyObject* AliasMap_alias(PyObject* self, PyObject* args);

  PyMethodDef methods[] = {
    { "add",    AliasMap_add,    METH_VARARGS,  "self.add(alias:string, source:Src)\n\nAdd one more alias to the map."},
    { "clear",  AliasMap_clear,  METH_NOARGS,   "self.clear()\n\nRemove all aliases."},
    { "src",    AliasMap_src,    METH_VARARGS,  "self.src(alias:string)\n\nFind matching Src for given alias name. "
        "If specified alias name does not exist in the map then default-constructed instance of Src will be returned."},
    { "alias",  AliasMap_alias,  METH_VARARGS,  "self.alias(source:Src)\n\nFind matching alias name for given Src. "
        "If specified Src does not exist in the map then empty string will be returned."},
    {0, 0, 0, 0}
   };

  char typedoc[] = "\
Python wrapper for psana AliasMap class. Usually the instances of this class are \
created by the framework itself and returned to the client from framework methods. \
Clients can instantiate objects of this type directly, this may not be very useful \
in general but could be helpful during testing.\n\
";

}

//    ----------------------------------------
//    -- Public Function Member Definitions --
//    ----------------------------------------

void
psana_python::AliasMap::initType(PyObject* module)
{
  PyTypeObject* type = BaseType::typeObject() ;
  type->tp_doc = ::typedoc;
  type->tp_methods = ::methods;
  type->tp_new = ::AliasMap_new;

  BaseType::initType("AliasMap", module, "psana");
}

// Dump object info to a stream
void 
psana_python::AliasMap::print(std::ostream& out) const
{
  out << "psana.AliasMap()" ;
}

namespace {

PyObject*
AliasMap_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds)
{
  // no arguments expected
  if (PyTuple_GET_SIZE(args) != 0) {
    PyErr_SetString(PyExc_TypeError, "AliasMap() does not need any arguments");
    return 0;
  }

  PyObject* self = subtype->tp_alloc(subtype, 1);
  AliasMap* py_this = static_cast<AliasMap*>(self);

  // construct in place, may not throw
  new(&py_this->m_obj) boost::shared_ptr<PSEvt::AliasMap>(new PSEvt::AliasMap());

  return self;
}

PyObject*
AliasMap_clear(PyObject* self, PyObject*)
{
  boost::shared_ptr<PSEvt::AliasMap>& cself = AliasMap::cppObject(self);
  cself->clear();
  Py_RETURN_NONE;
}

PyObject* 
AliasMap_add(PyObject* self, PyObject* args)
{
  boost::shared_ptr<PSEvt::AliasMap>& cself = psana_python::AliasMap::cppObject(self);

  const char* alias;
  PyObject* srcObj;
  if (not PyArg_ParseTuple( args, "sO:AliasMap.add", &alias, &srcObj)) return 0;
  if (not PdsSrc::Object_TypeCheck(srcObj)) {
    PyErr_SetString(PyExc_TypeError, "Error: parameter is not a Pds.Src instance");
    return 0;
  }

  const Pds::Src& src = PdsSrc::cppObject(srcObj);
  cself->add(alias, src);
  Py_RETURN_NONE;
}

PyObject* 
AliasMap_src(PyObject* self, PyObject* args)
{
  boost::shared_ptr<PSEvt::AliasMap>& cself = psana_python::AliasMap::cppObject(self);

  const char* alias;
  if (not PyArg_ParseTuple( args, "s:AliasMap.src", &alias)) return 0;

  return PdsSrc::PyObject_FromCpp(cself->src(alias));
}

PyObject*
AliasMap_alias(PyObject* self, PyObject* args)
{
  boost::shared_ptr<PSEvt::AliasMap>& cself = psana_python::AliasMap::cppObject(self);

  PyObject* srcObj;
  if (not PyArg_ParseTuple( args, "O:AliasMap.add", &srcObj)) return 0;
  if (not PdsSrc::Object_TypeCheck(srcObj)) {
    PyErr_SetString(PyExc_TypeError, "Error: parameter is not a Pds.Src instance");
    return 0;
  }

  const std::string& alias = cself->alias(PdsSrc::cppObject(srcObj));
  return PyString_FromStringAndSize(alias.data(), alias.size());
}

}
