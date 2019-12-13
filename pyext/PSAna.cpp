//--------------------------------------------------------------------------
// File and Version Information:
//  $Id$
//
// Description:
//  Class PSAna...
//
// Author List:
//      Andrei Salnikov
//
//------------------------------------------------------------------------

#if PY_MAJOR_VERSION >= 3
#define IS_PY3K
#endif

//-----------------------
// This Class's Header --
//-----------------------
#include "PSAna.h"

//-----------------
// C/C++ Headers --
//-----------------

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "DataSource.h"
#include "pytools/PyUtil.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

namespace {

#if PY_VERSION_HEX >= 0x02050000
  typedef Py_ssize_t PyDictPosType;
#else
  typedef int PyDictPosType;
#endif

  // type-specific methods
  PyObject* PSAna_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds);
  PyObject* PSAna_dataSource(PyObject* self, PyObject* args);

  PyMethodDef methods[] = {
    { "dataSource",  PSAna_dataSource,  METH_VARARGS, 
        "self.dataSource(input, ...) -> DataSource\n\n"
        "Returns data source object (:py:class:`_DataSource`), accepts a list of strings or "
        "any number of string arguments. Strings can be either names of files or datasets." },
    {0, 0, 0, 0}
   };

  char typedoc[] = "PSAna(configFile: str[, options: dict]) -> instance\n\n"
      "Python wrapper for psana framework class (PSAna). Constructor takes the name "
      "of the configuration file (typically \"psana.cfg\") and optional dictionary "
      "of configuration options. Keys in the options dictionary are option names, "
      "name has format \"section.option\" where \"section\" is the name of the section "
      "in configuration file (such as \"psana\" or \"psana_examples.DumpPrinceton\"). "
      "Value can be any string, possibly empty, non-string values will be converted "
      "to strings using str() call. Options from a dictionary override corresponding "
      "options in configuration file.\n\n"
      "Objects of this type cannot be directly instantiated in user code, users will typically "
      "use high-level wrappers for the framework (see :py:mod:`psana`).";

}

//    ----------------------------------------
//    -- Public Function Member Definitions --
//    ----------------------------------------

void
psana_python::pyext::PSAna::initType(PyObject* module)
{
  PyTypeObject* type = BaseType::typeObject() ;
  type->tp_doc = ::typedoc;
  type->tp_methods = ::methods;
  type->tp_new = ::PSAna_new;

  BaseType::initType("PSAna", module, "psana");
}

namespace {

PyObject* 
PSAna_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds)
{
  // parse arguments
  const char* config = 0;
  PyObject* options = 0;
  if (not PyArg_ParseTuple(args, "s|O:PSAna", &config, &options)) return 0;
  if (options and not PyDict_Check(options)) {
    PyErr_SetString(PyExc_TypeError, "Error: PSAna expects dict-like object as second argument");
    return 0;
  }

  // copy dict to map
  std::map<std::string, std::string> optMap;
  PyObject *key, *value;
  PyDictPosType pos = 0;
  while (options and PyDict_Next(options, &pos, &key, &value)) {
#ifdef IS_PY3K
    if (not (PyUnicode_Check(key) or PyBytes_Check(key))) {
#else
    if (not PyString_Check(key)) {
#endif
      PyErr_SetString(PyExc_TypeError, "Error: PSAna options keys must be strings");
      return 0;
    }
#ifdef IS_PY3K
    if (not (PyUnicode_Check(value) or PyBytes_Check(value))) {
#else
    if (not PyString_Check(value)) {
#endif
      PyErr_SetString(PyExc_TypeError, "Error: PSAna options keys must be strings");
      return 0;
    }
    optMap[PyString_AsString_Compatible(key)] = PyString_AsString_Compatible(value);
  }

  // allocate 
  PyObject * self = subtype->tp_alloc(subtype, 1);
  psana_python::pyext::PSAna* py_this = static_cast<psana_python::pyext::PSAna*>(self);

  // construct shared pointer
  new(&py_this->m_obj) boost::shared_ptr<psana::PSAna>();

  // instantiate framework
  try {
    py_this->m_obj = boost::make_shared<psana::PSAna>(std::string(config), optMap);
  } catch (const std::exception& ex) {
    subtype->tp_dealloc(self);
    PyErr_SetString(PyExc_RuntimeError, ex.what());
    return 0;
  }
  
  return self;
}

PyObject*
PSAna_dataSource(PyObject* self, PyObject* args)
try {
  psana_python::pyext::PSAna* py_this = static_cast<psana_python::pyext::PSAna*>(self);

  // parse arguments
  std::vector<std::string> inpVec;
  if (PyTuple_GET_SIZE(args) == 0) {
    PyErr_SetString(PyExc_TypeError, "Error: PSAna.dataSource expects at least one argument");
    return 0;
  } else if (PyTuple_GET_SIZE(args) == 1 and PyList_Check(PyTuple_GET_ITEM(args, 0))) {
    // single argument which is a list
    const int size = PyList_GET_SIZE(PyTuple_GET_ITEM(args, 0));
    inpVec.reserve(size);
    for (int i = 0; i != size; ++ i) {
      PyObject* inp = PyList_GET_ITEM(PyTuple_GET_ITEM(args, 0), i);
#ifdef IS_PY3K
      if (not (PyUnicode_Check(inp) or PyBytes_Check(inp))) {
#else
      if (not PyString_Check(inp)) {
#endif
        PyErr_SetString(PyExc_TypeError, "Error: PSAna.dataSource expects list of strings as an argument");
        return 0;
      }
      inpVec.push_back(PyString_AsString_Compatible(inp));
    }
  } else {
    // any number of string arguments
    const int size = PyTuple_GET_SIZE(args);
    inpVec.reserve(size);
    for (int i = 0; i != size; ++ i) {
      PyObject* inp = PyTuple_GET_ITEM(args, i);
#ifdef IS_PY3K
      if (not (PyUnicode_Check(inp) or PyBytes_Check(inp))) {
#else
      if (not PyString_Check(inp)) {
#endif
        PyErr_SetString(PyExc_TypeError, "Error: PSAna.dataSource expects strings as an arguments");
        return 0;
      }
      inpVec.push_back(PyString_AsString_Compatible(inp));
    }
  }
    
  return psana_python::pyext::DataSource::PyObject_FromCpp(py_this->m_obj->dataSource(inpVec));

} catch (const std::exception& ex) {
  PyErr_SetString(PyExc_RuntimeError, ex.what());
  return 0;
}

}
