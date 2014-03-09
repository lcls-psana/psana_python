//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class SrcMatch...
//
// Author List:
//      Andy Salnikov
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "psana_python/SrcMatch.h"

//-----------------
// C/C++ Headers --
//-----------------
#include <exception>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "PSEvt/EventKey.h"
#include "psana_python/PdsSrc.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

using namespace psana_python;

namespace {

  // type-specific methods
  PyObject* SrcMatch_match(PyObject* self, PyObject*);
  PyObject* SrcMatch_in(PyObject* self, PyObject*);
  PyObject* SrcMatch_isNoSource(PyObject* self, PyObject*);
  PyObject* SrcMatch_isExact(PyObject* self, PyObject*);

  int SrcMatch_contains(PyObject *o, PyObject *value);

  PyMethodDef methods[] = {
    { "match",       SrcMatch_match,       METH_VARARGS, "self.match(src:Src) -> bool\n\nMatch source with :py:class:`Src` object." },
    { "in_",         SrcMatch_in,          METH_VARARGS, "self.in_(match:SrcMatch) -> bool\n\n"
        "Returns true if set of addresses matched by this instance is contained entirely in the set of addresses matched by an argument. "
        "One can also use expression `match1 in match2` which is equivalent to `match1.in_(match2)`." },
    { "isNoSource",  SrcMatch_isNoSource,  METH_NOARGS,  "self.isNoSource() -> bool\n\nReturns true if it matches no-source only." },
    { "isExact",     SrcMatch_isExact,     METH_NOARGS,  "self.isExact() -> bool\n\nReturns true if it is exact match, no-source is also exact." },
    {0, 0, 0, 0}
   };

  char typedoc[] = "\
Helper class which provides logic for matching Source values to Src instances.\n\
\n\
This is an abstraction of the set of addresses. Having this set one can ask\
questions like:\n\
- does a specific address (:py:class:`Src` instance) belong to the set (match() method).\n\
- does other set (another :py:class:`SrcMatch` instance) contains this set completely (so that\
  for example one could ask question like \"will this set match cspad-only devices\")\n\
- check if this is a special \"no-source\" match (matching only data that do not come\
  from any device)\n\
- check if this is \"exact\" match (matching only one specific device, or no-source)\
";

  PySequenceMethods seq_methods;

}

//              ----------------------------------------
//              -- Public Function Member Definitions --
//              ----------------------------------------

void
psana_python::SrcMatch::initType(PyObject* module)
{
  PyTypeObject* type = BaseType::typeObject() ;
  type->tp_doc = ::typedoc;
  type->tp_methods = ::methods;
  type->tp_as_sequence = &seq_methods;
  seq_methods.sq_contains = ::SrcMatch_contains;

  BaseType::initType("SrcMatch", module, "psana");
}

// Dump object info to a stream
void
psana_python::SrcMatch::print(std::ostream& out) const
{
  out << "SrcMatch(" << m_obj.src() << ')';
}

namespace {

PyObject*
SrcMatch_match(PyObject* self, PyObject* args)
{
  PSEvt::Source::SrcMatch& cself = SrcMatch::cppObject(self);

  PyObject* srcObj;
  if (not PyArg_ParseTuple( args, "O!:SrcMatch.match", PdsSrc::typeObject(), &srcObj)) return 0;

  return PyBool_FromLong(cself.match(PdsSrc::cppObject(srcObj)));
}

PyObject*
SrcMatch_in(PyObject* self, PyObject* args)
{
  PSEvt::Source::SrcMatch& cself = SrcMatch::cppObject(self);

  PyObject* srcObj;
  if (not PyArg_ParseTuple( args, "O!:SrcMatch.in", SrcMatch::typeObject(), &srcObj)) return 0;

  return PyBool_FromLong(cself.in(SrcMatch::cppObject(srcObj)));
}

PyObject*
SrcMatch_isNoSource(PyObject* self, PyObject* )
{
  PSEvt::Source::SrcMatch& cself = SrcMatch::cppObject(self);
  return PyBool_FromLong(cself.isNoSource());
}

PyObject*
SrcMatch_isExact(PyObject* self, PyObject* )
{
  PSEvt::Source::SrcMatch& cself = SrcMatch::cppObject(self);
  return PyBool_FromLong(cself.isExact());
}

int SrcMatch_contains(PyObject *o, PyObject *value)
{
  if (not SrcMatch::Object_TypeCheck(value)) {
    PyErr_SetString(PyExc_TypeError, "Error: parameter is not a SrcMatch instance");
    return -1;
  }
  if (not SrcMatch::Object_TypeCheck(o)) {
    PyErr_SetString(PyExc_TypeError, "Error: parameter is not a SrcMatch instance");
    return -1;
  }

  PSEvt::Source::SrcMatch& cself = SrcMatch::cppObject(value);
  return cself.in(SrcMatch::cppObject(o)) ? 1 : 0;
}

}
