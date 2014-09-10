//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: PSAnaApp.cpp 3280 2012-05-01 16:41:53Z salnikov@SLAC.STANFORD.EDU $
//
// Description:
//	Class PSAnaApp
//
// Author List:
//  Andy Salnikov, Joseph S. Barrera III
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "psana_python/PythonModule.h"

//-----------------
// C/C++ Headers --
//-----------------
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <boost/python.hpp>
#include <boost/foreach.hpp>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "MsgLogger/MsgLogger.h"
#include "PSEvt/EventId.h"
#include "psana_python/Exceptions.h"
#include "psana_python/Env.h"
#include "psana_python/Event.h"
#include "psana_python/Source.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

using std::string;
using boost::python::object;

namespace {

  const char logger[] = "psana_python.PythonModule";

  // return string describing Python exception
  string pyExcStr()
  {
    PyObject *ptype;
    PyObject *pvalue;
    PyObject *ptraceback;

    PyErr_Fetch(&ptype, &pvalue, &ptraceback);
    PyObject* errstr = PyObject_Str(pvalue);
    string msg = PyString_AsString(errstr);

    PyErr_Display(ptype, pvalue, ptraceback);

    Py_CLEAR(errstr);
    Py_CLEAR(ptype);
    Py_CLEAR(pvalue);
    Py_CLEAR(ptraceback);

    return msg;
  }

  // initialization of interpreter
  bool py_init();

  // names for psana-style methods, this must correspond to method enums in class declaration
  const char* psana_methods[] = {
    "beginJob", "beginRun", "beginCalibCycle", "event", "endCalibCycle", "endRun", "endJob"
  };

  // names for pyana-style methods, this must correspond to method enums in class declaration
  const char* pyana_methods[] = {
    "beginjob", "beginrun", "begincalibcycle", "event", "endcalibcycle", "endrun", "endjob"
  };

  // functor to check for non-zero pointers
  struct NonZero {
    bool operator()(const pytools::pyshared_ptr& p) const { return bool(p); }
  };

  std::ostream& operator<<(std::ostream& str, PyObject* obj) {
    if (obj) {
      pytools::pyshared_ptr repr = pytools::make_pyshared(PyObject_Repr(obj));
      return str << PyString_AsString(repr.get());
    } else {
      return str << "PyObject<NULL>";
    }
  }

  // extra methods definition
  psana_python::PythonModule* cpp_module(PyObject* self);
  PyObject* extra_name(PyObject* self, PyObject* args);
  PyObject* extra_className(PyObject* self, PyObject* args);
  PyObject* extra_configBool(PyObject* self, PyObject* args);
  PyObject* extra_configInt(PyObject* self, PyObject* args);
  PyObject* extra_configFloat(PyObject* self, PyObject* args);
  PyObject* extra_configStr(PyObject* self, PyObject* args);
  PyObject* extra_configSrc(PyObject* self, PyObject* args);
  PyObject* extra_configListBool(PyObject* self, PyObject* args);
  PyObject* extra_configListInt(PyObject* self, PyObject* args);
  PyObject* extra_configListFloat(PyObject* self, PyObject* args);
  PyObject* extra_configListStr(PyObject* self, PyObject* args);
  PyObject* extra_configListSrc(PyObject* self, PyObject* args);
  PyObject* extra_skip(PyObject* self, PyObject* args);
  PyObject* extra_stop(PyObject* self, PyObject* args);
  PyObject* extra_terminate(PyObject* self, PyObject* args);

  static PyMethodDef extraMethods[] = {
    {"name",        extra_name,          METH_NOARGS,  "self.name() -> str\n\nReturns the name of this module"},
    {"className",   extra_className,     METH_NOARGS,  "self.className() -> str\n\nReturns class name of this module"},
    {"configBool",  extra_configBool,    METH_VARARGS,
        "self.configBool(param:str[, default:bool]) -> bool\n\nReturns value of boolean parameter"},
    {"configInt",   extra_configInt,     METH_VARARGS,
        "self.configInt(param:str[, default:int]) -> int\n\nReturns value of integer parameter"},
    {"configFloat", extra_configFloat,   METH_VARARGS,
        "self.configFloat(param:str[, default:float]) -> float\n\nReturns value of floating point parameter"},
    {"configStr",   extra_configStr,     METH_VARARGS,
        "self.configStr(param:str[, default:str]) -> str\n\nReturns string value of parameter"},
    {"configSrc",   extra_configSrc,     METH_VARARGS,
        "self.configSrc(param:str[, default:str]) -> Source\n\nReturns value of parameter as a Source object"},
    {"configListBool",  extra_configListBool,     METH_O,
        "self.configListBool(param:str) -> list of bool\n\nReturns value of parameter as a list of booleans, "
        "if parameter is not defined then empty list is returned."},
    {"configListInt",   extra_configListInt,     METH_O,
        "self.configListInt(param:str) -> list of int\n\nReturns value of parameter as a list of integer numbers, "
        "if parameter is not defined then empty list is returned."},
    {"configListFloat",   extra_configListFloat, METH_O,
        "self.configListFloat(param:str) -> list of float\n\nReturns value of parameter as a list of floating numbers, "
        "if parameter is not defined then empty list is returned."},
    {"configListStr",   extra_configListStr,     METH_O,
        "self.configListStr(param:str) -> list of str\n\nReturns value of parameter as a list of strings, "
        "if parameter is not defined then empty list is returned."},
    {"configListSrc",   extra_configListSrc,     METH_O,
        "self.configListSrc(param:str) -> list of Source\n\nReturns value of parameter as a list of Source objects, "
        "if parameter is not defined then empty list is returned."},
    {"skip",        extra_skip,          METH_NOARGS,  "self.skip()\n\n"
        "Signal framework to skip current event and do not call other downstream modules. "
        "Note that this method does not skip code in the current module, control is returned back to the module. "
        "If you want to stop processing after this call then add a return statement."},
    {"stop",        extra_stop,          METH_NOARGS,  "self.stop()\n\n"
        "Signal framework to stop event loop and finish job gracefully. "
        "Note that this method does not terminate processing in the current module. "
        "If you want to stop processing after this call then add a return statement."},
    {"terminate",   extra_terminate,     METH_NOARGS,  "self.terminate()\n\n"
        "Signal framework to terminate immediately. "
        "Note that this method does not terminate processing in the current module. "
        "If you want to stop processing after this call then add a return statement."},
    {NULL},
  };


}

//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------

namespace psana_python {

//----------------
// Constructors --
//----------------
PythonModule::PythonModule(const std::string& name, PyObject* instance)
  : Module(name)
  , m_instance(pytools::make_pyshared(instance, false))
  , m_pyanaCompat(true)
{
  // Currently, pyana compatibity is enabled unless 'psana.pyana_compat' config option is set to 0.
  m_pyanaCompat = configSvc().get("psana", "pyana_compat", true);

  // check pyana-style methods first
  for (int i = 0; i != NumMethods; ++ i) {
    m_methods[i] = pytools::make_pyshared(PyObject_GetAttrString(m_instance.get(), pyana_methods[i]));
  }

  // reset errors from PyObject_GetAttrString
  PyErr_Clear();

  // check for presence of any methods except of "event"
  bool any = m_methods[MethBeginJob] or m_methods[MethBeginRun] or m_methods[MethBeginScan] or
      m_methods[MethEndScan] or m_methods[MethEndRun] or m_methods[MethEndJob];

  // check that we are not getting pyana stuff if compatibility is disabled
  if (any and not m_pyanaCompat) {
    throw Exception(ERR_LOC, "Error: old pyana-style methods used (e.g. beginjob instead of beginJob)");
  }

  if (not m_pyanaCompat or not any) {
    // search for psana-style methods
    m_pyanaCompat = false;
    for (int i = 0; i != NumMethods; ++ i) {
      m_methods[i] = pytools::make_pyshared(PyObject_GetAttrString(m_instance.get(), psana_methods[i]));
    }
    // reset errors from PyObject_GetAttrString
    PyErr_Clear();
  }

  // check that at least one method is there
  any = std::find_if(m_methods, m_methods+NumMethods, ::NonZero()) != (m_methods+NumMethods);
  if (not any) {
    throw Exception(ERR_LOC, "Error: module " + name + " does not define any methods");
  }

}

//--------------
// Destructor --
//--------------
PythonModule::~PythonModule ()
{
}

void
PythonModule::call(PyObject* method, bool pyana_optional_evt, PSEvt::Event& evt, PSEnv::Env& env)
{
  if (not method) return;

  // ensuer GIL is locked, restore when lock goes out of scope
  GILLocker lock;

  // in pyana mode some methods can take either (env) or (evt, env),
  // check number of arguments to guess how to call it
  int nargs = 2;
  if (pyana_optional_evt) {
    PyCodeObject* code = (PyCodeObject*)PyFunction_GetCode(PyMethod_Function(method));
    // co_argcount includes self argument
    nargs = code->co_argcount - 1;
  }

  pytools::pyshared_ptr args = pytools::make_pyshared(PyTuple_New(nargs));
  if (nargs > 1) {
    PyObject* pyevt = psana_python::Event::PyObject_FromCpp(evt.shared_from_this());
    PyTuple_SET_ITEM(args.get(), 0, pyevt);
  }
  PyObject* pyenv = psana_python::Env::PyObject_FromCpp(env.shared_from_this());
  PyTuple_SET_ITEM(args.get(), nargs - 1, pyenv);
  
  // call the method
  pytools::pyshared_ptr res = pytools::make_pyshared(PyObject_Call(method, args.get(), NULL));
  if (not res) {
    PyErr_Print();
    throw ExceptionGenericPyError(ERR_LOC, "Python exception raised, check error output for details");
  }

  // if method returns integer number try to translate it into skip/stop/terminate
  if (PyInt_Check(res.get())) {
    switch (PyInt_AS_LONG(res.get())) {
    case Skip:
      skip();
      break;
    case Stop:
      stop();
      break;
    case Terminate:
      terminate();
      break;
    default:
      break;
    }
  }
}

// Load one user module. The name of the module has a format [Package.]Class[:name]
extern "C"
psana::Module*
moduleFactory(const string& name)
{
  // Make class name and module name. Use psana for package name if not given.
  // Full name should be package name . class name.
  string fullName = name;
  string moduleName = name;
  string::size_type p1 = moduleName.find(':');
  if (p1 != string::npos) {
    moduleName.erase(p1);
  }
  string className = moduleName;
  p1 = className.find('.');
  if (p1 == string::npos) {
    moduleName = "psana." + moduleName;
    fullName = "psana." + fullName;
  } else {
    className.erase(0, p1+1);
  }

  MsgLog(logger, debug, "names: module=" << moduleName << " class=" << className << " full=" << fullName);

  // make sure that Python is initialized correctly
  static bool pyInitOnce __attribute__((unused)) = ::py_init();

  // try to import module
  MsgLog(logger, debug, "import module name=" << moduleName);
  pytools::pyshared_ptr mod = pytools::make_pyshared(PyImport_ImportModule((char*)moduleName.c_str()));
  if (not mod) {
    string msg = "failed to import module " + moduleName + ": " + ::pyExcStr();
    MsgLog(logger, error, msg);
    throw ExceptionPyLoadError(ERR_LOC, msg);
  }

  // there must be a class defined with this name
  MsgLog(logger, debug, "find class in a module, name=" << className);
  pytools::pyshared_ptr cls = pytools::make_pyshared(PyObject_GetAttrString(mod.get(), (char*)className.c_str()));
  if (not cls) {
    string msg = "module " + moduleName + " does not define class " + className;
    MsgLog(logger, error, msg);
    throw ExceptionPyLoadError(ERR_LOC, msg);
  }

  // make sure class is a type as we want to extend it with few methods
  if (not PyType_Check(cls.get())) {
    string msg = "name " + className + " is not a type object (not a class)";
    MsgLog(logger, error, msg);
    throw ExceptionPyLoadError(ERR_LOC, msg);
  }

  // define/override few extra methods, need to be called before we make an instance
  MsgLog(logger, debug, "define extra methods for a class");
  for (PyMethodDef *def = ::extraMethods; def->ml_name != 0; ++ def) {
    if (not PyObject_HasAttrString(cls.get(), def->ml_name)) {
      pytools::pyshared_ptr method = pytools::make_pyshared(PyDescr_NewMethod((PyTypeObject*)cls.get(), def));
      if (PyObject_SetAttrString(cls.get(), def->ml_name, method.get()) < 0) {
        throw ExceptionGenericPyError(ERR_LOC, "PyObject_SetAttrString failed");
      }

    }
  }

  // this is a dirty hack to make module name available inside module constructor,
  // this is not thread safe
  MsgLog(logger, debug, "set special attribute for module name");
  pytools::pyshared_ptr modname = pytools::make_pyshared(PyString_FromString(fullName.c_str()));
  if (PyObject_SetAttrString(cls.get(), "__psana_module_name__", modname.get()) < 0) {
    throw ExceptionGenericPyError(ERR_LOC, "PyObject_SetAttrString failed");
  }

  // Make empty positional args list.
  pytools::pyshared_ptr args = pytools::make_pyshared(PyTuple_New(0));

  pytools::pyshared_ptr instance;

  // First try pyana-compatibility mode if enabled
  ConfigSvc::ConfigSvc configSvc(Context::get());
  if (configSvc.get("psana", "pyana_compat", true)) {

    // Create keyword args list.
    pytools::pyshared_ptr kwargs = pytools::make_pyshared(PyDict_New());
    ConfigSvc::ConfigSvc cfg(psana::Context::get());

    std::list<std::string> keys_mod = cfg.getKeys(moduleName);
    std::list<std::string>::iterator it_mod;
    for (it_mod = keys_mod.begin(); it_mod != keys_mod.end(); it_mod++) {
      const std::string& key = *it_mod;
      const char* value = cfg.getStr(moduleName, key).c_str();
      PyDict_SetItemString(kwargs.get(), key.c_str(), PyString_FromString(value));
    }

    std::list<std::string> keys = cfg.getKeys(fullName);
    std::list<std::string>::iterator it;
    for (it = keys.begin(); it != keys.end(); it++) {
      const std::string& key = *it;
      const char* value = cfg.getStr(fullName, key).c_str();
      PyDict_SetItemString(kwargs.get(), key.c_str(), PyString_FromString(value));
    }

    MsgLog(logger, debug, "try to make pyana-compatible module instance, class name=" << className << " kw=" << kwargs.get());

    // Construct the instance, pass kw list constructor
    instance = pytools::make_pyshared(PyObject_Call(cls.get(), args.get(), kwargs.get()));
    if (not instance) {
      MsgLog(logger, debug, "failed to make class instance in pyana-compatibility mode: name=" + className);
    }

  }

  // Construct the instance, do not pass anything to constructor
  if (not instance) {
    MsgLog(logger, debug, "try to make standard module instance, class name=" << className);
    pytools::pyshared_ptr kwargs = pytools::make_pyshared(PyDict_New());
    instance = pytools::make_pyshared(PyObject_Call(cls.get(), args.get(), kwargs.get()));
  }

  if (not instance) {
    std::string msg = "cannot create instance of class " + className + ": " + ::pyExcStr();
    MsgLog(logger, error, msg);
    throw ExceptionPyLoadError(ERR_LOC, msg);
  }

  // check that instance has at least an event() method
  MsgLog(logger, debug, "check for event method");
  if (not PyObject_HasAttrString(instance.get(), "event")) {
    std::string msg = "class " + className + " does not define event() method";
    MsgLog(logger, error, msg);
    throw ExceptionPyLoadError(ERR_LOC, msg);
  }

  // fetch or make C++ module instance
  PythonModule* module = cpp_module(instance.get());
  if (not module) {
    throw ExceptionGenericPyError(ERR_LOC, "Failed to make C++ module instance: " + ::pyExcStr());
  }

  // do not need special attribute any more
  if (PyObject_DelAttrString(cls.get(), "__psana_module_name__") < 0) {
    PyErr_Clear();
  }

  return module;
}

} // namespace psana




namespace {


bool py_init()
{
  // Make sure python is initialized
  Py_Initialize();

  // Some things (like IPython.embed) may depend on sys.argv which may not be defined
  // by default (depending on how psana is instantiated), set it here.
  char argv[] = "argv";  // need non-const char* pointer
  if (not PySys_GetObject(argv)) {
    char argv0[] = "psana";
    char* pargv = argv0;
    PySys_SetArgv(1, &pargv);
  }

  // Clear any lingering errors
  PyErr_Clear();

  // The whole shebang from this package needs to be initialized to expose the
  // wrapped stuff to Python interpreter. We are doing this via importing the module _psana.
  pytools::pyshared_ptr psanamod = pytools::make_pyshared(PyImport_ImportModule("_psana"));
  if (not psanamod) {
    string msg = "failed to import module _psana: " + ::pyExcStr();
    MsgLog(logger, error, msg);
    throw psana_python::ExceptionPyLoadError(ERR_LOC, msg);
  }

  return true;
}

psana_python::PythonModule*
cpp_module(PyObject* self)
{
  // get wrapped C++ instance
  if (not PyObject_HasAttrString(self, "__psana_cpp_module__")) {
    
    pytools::pyshared_ptr modname = pytools::make_pyshared(PyObject_GetAttrString(self, "__psana_module_name__"));
    if (not modname) return 0;
    
    // make C++ module instance
    psana_python::PythonModule* module = 0;
    try {
      const char* cmodname = PyString_AsString(modname.get());
      MsgLog(logger, debug, "make C++ instance for module " << cmodname);
      module = new psana_python::PythonModule(cmodname, self);
    } catch (const std::exception& ex) {
      PyErr_SetString(PyExc_RuntimeError, ex.what());
      return 0;
    }

    // store it inside Python object
    pytools::pyshared_ptr pymodule = pytools::make_pyshared(PyCObject_FromVoidPtr(static_cast<void*>(module), NULL));
    if (PyObject_SetAttrString(self, "__psana_cpp_module__", pymodule.get()) < 0) {
      delete module;
      return 0;
    }
    
    return module;
    
  } else {
    
    pytools::pyshared_ptr pymod = pytools::make_pyshared(PyObject_GetAttrString(self, "__psana_cpp_module__"));
    if (not pymod) return 0;
    if (not PyCObject_Check(pymod.get())) {
      PyErr_SetString(PyExc_TypeError, "incorrect type of __psana_cpp_module__ attribute");
      return 0;
    }
  
    // unwrap it
    return static_cast<psana_python::PythonModule*>(PyCObject_AsVoidPtr(pymod.get()));
    
  }
}

PyObject*
extra_name(PyObject* self, PyObject* args)
{
  psana_python::PythonModule* module = cpp_module(self);
  if (not module) return 0;

  return PyString_FromString(module->name().c_str());
}

PyObject*
extra_className(PyObject* self, PyObject* args)
{
  psana_python::PythonModule* module = cpp_module(self);
  if (not module) return 0;

  return PyString_FromString(module->className().c_str());
}

PyObject*
extra_configStr(PyObject* self, PyObject* args)
{
  psana_python::PythonModule* module = cpp_module(self);
  if (not module) return 0;

  // parse args
  const char* parm = 0;
  PyObject* def = 0;
  if (not PyArg_ParseTuple(args, "s|O:module.configStr", &parm, &def)) return 0;

  // call C++ module method, take care of all exceptions
  try {
    return PyString_FromString(module->configStr(parm).c_str());
  } catch (const ConfigSvc::ExceptionMissing& ex) {
    // parameter not found, return default value if present without any conversion
    if (def) {
      Py_INCREF(def);
      return def;
    }
    PyErr_SetString(PyExc_ValueError, ex.what());
  } catch (const std::exception& ex) {
    PyErr_SetString(PyExc_ValueError, ex.what());
  }
  return 0;
}

PyObject*
extra_configBool(PyObject* self, PyObject* args)
{
  psana_python::PythonModule* module = cpp_module(self);
  if (not module) return 0;

  // parse args
  const char* parm = 0;
  PyObject* def = 0;
  if (not PyArg_ParseTuple(args, "s|O:module.configBool", &parm, &def)) return 0;

  // call C++ module method, take care of all exceptions
  try {
    return PyBool_FromLong(static_cast<bool>(module->config(parm)));
  } catch (const ConfigSvc::ExceptionMissing& ex) {
    // parameter not found, return default value if present without any conversion
    if (def) {
      Py_INCREF(def);
      return def;
    }
    PyErr_SetString(PyExc_ValueError, ex.what());
  } catch (const std::exception& ex) {
    PyErr_SetString(PyExc_ValueError, ex.what());
  }
  return 0;
}

PyObject*
extra_configInt(PyObject* self, PyObject* args)
{
  psana_python::PythonModule* module = cpp_module(self);
  if (not module) return 0;

  // parse args
  const char* parm = 0;
  PyObject* def = 0;
  if (not PyArg_ParseTuple(args, "s|O:module.configInt", &parm, &def)) return 0;

  // call C++ module method, take care of all exceptions
  try {
    return PyInt_FromLong(static_cast<long>(module->config(parm)));
  } catch (const ConfigSvc::ExceptionMissing& ex) {
    // parameter not found, return default value if present without any conversion
    if (def) {
      Py_INCREF(def);
      return def;
    }
    PyErr_SetString(PyExc_ValueError, ex.what());
  } catch (const std::exception& ex) {
    PyErr_SetString(PyExc_ValueError, ex.what());
  }
  return 0;
}

PyObject*
extra_configFloat(PyObject* self, PyObject* args)
{
  psana_python::PythonModule* module = cpp_module(self);
  if (not module) return 0;

  // parse args
  const char* parm = 0;
  PyObject* def = 0;
  if (not PyArg_ParseTuple(args, "s|O:module.configFloat", &parm, &def)) return 0;

  // call C++ module method, take care of all exceptions
  try {
    return PyFloat_FromDouble(static_cast<double>(module->config(parm)));
  } catch (const ConfigSvc::ExceptionMissing& ex) {
    // parameter not found, return default value if present without any conversion
    if (def) {
      Py_INCREF(def);
      return def;
    }
    PyErr_SetString(PyExc_ValueError, ex.what());
  } catch (const std::exception& ex) {
    PyErr_SetString(PyExc_ValueError, ex.what());
  }
  return 0;
}

PyObject*
extra_configSrc(PyObject* self, PyObject* args)
{
  psana_python::PythonModule* module = cpp_module(self);
  if (not module) return 0;

  // parse args
  const char* parm = 0;
  const char* def = 0;
  if (not PyArg_ParseTuple(args, "s|s:module.configSrc", &parm, &def)) return 0;

  // call C++ module method, take care of all exceptions
  try {
    PSEvt::Source val;
    if (def) {
      val = module->configSrc(parm, def);
    } else {
      val = module->configSrc(parm);
    }
    return psana_python::Source::PyObject_FromCpp(val);
  } catch (const std::exception& ex) {
    PyErr_SetString(PyExc_ValueError, ex.what());
    return 0;
  }
}

PyObject*
extra_configListBool(PyObject* self, PyObject* arg)
{
  psana_python::PythonModule* module = cpp_module(self);
  if (not module) return 0;

  // parse args
  const char* parm = PyString_AsString(arg);
  if (not parm) return 0;

  // call C++ module method, take care of all exceptions
  try {
    // get the list from config
    const std::list<bool>& cfglist = module->configList(parm, std::list<bool>());

    // convert to python list
    PyObject* res = PyList_New(cfglist.size());
    unsigned i = 0;
    BOOST_FOREACH(long v, cfglist) {
      PyList_SET_ITEM(res, i, PyBool_FromLong(v));
      ++ i;
    }
    return res;
  } catch (const std::exception& ex) {
    PyErr_SetString(PyExc_ValueError, ex.what());
    return 0;
  }
}

PyObject*
extra_configListInt(PyObject* self, PyObject* arg)
{
  psana_python::PythonModule* module = cpp_module(self);
  if (not module) return 0;

  // parse args
  const char* parm = PyString_AsString(arg);
  if (not parm) return 0;

  // call C++ module method, take care of all exceptions
  try {
    // get the list from config
    const std::list<long>& cfglist = module->configList(parm, std::list<long>());

    // convert to python list
    PyObject* res = PyList_New(cfglist.size());
    unsigned i = 0;
    BOOST_FOREACH(long v, cfglist) {
      PyList_SET_ITEM(res, i, PyInt_FromLong(v));
      ++ i;
    }
    return res;
  } catch (const std::exception& ex) {
    PyErr_SetString(PyExc_ValueError, ex.what());
    return 0;
  }
}

PyObject*
extra_configListFloat(PyObject* self, PyObject* arg)
{
  psana_python::PythonModule* module = cpp_module(self);
  if (not module) return 0;

  // parse args
  const char* parm = PyString_AsString(arg);
  if (not parm) return 0;

  // call C++ module method, take care of all exceptions
  try {
    // get the list from config
    const std::list<double>& cfglist = module->configList(parm, std::list<double>());

    // convert to python list
    PyObject* res = PyList_New(cfglist.size());
    unsigned i = 0;
    BOOST_FOREACH(double v, cfglist) {
      PyList_SET_ITEM(res, i, PyFloat_FromDouble(v));
      ++ i;
    }
    return res;
  } catch (const std::exception& ex) {
    PyErr_SetString(PyExc_ValueError, ex.what());
    return 0;
  }
}

PyObject*
extra_configListStr(PyObject* self, PyObject* arg)
{
  psana_python::PythonModule* module = cpp_module(self);
  if (not module) return 0;

  // parse args
  const char* parm = PyString_AsString(arg);
  if (not parm) return 0;

  // call C++ module method, take care of all exceptions
  try {
    // get the list from config
    const std::list<std::string>& cfglist = module->configList(parm, std::list<std::string>());

    // convert to python list
    PyObject* res = PyList_New(cfglist.size());
    unsigned i = 0;
    BOOST_FOREACH(const std::string& v, cfglist) {
      PyList_SET_ITEM(res, i, PyString_FromString(v.c_str()));
      ++ i;
    }
    return res;
  } catch (const std::exception& ex) {
    PyErr_SetString(PyExc_ValueError, ex.what());
    return 0;
  }
}

PyObject*
extra_configListSrc(PyObject* self, PyObject* arg)
{
  psana_python::PythonModule* module = cpp_module(self);
  if (not module) return 0;

  // parse args
  const char* parm = PyString_AsString(arg);
  if (not parm) return 0;

  // call C++ module method, take care of all exceptions
  try {
    // get the list from config
    const std::list<std::string>& cfglist = module->configList(parm, std::list<std::string>());

    // convert to python list
    PyObject* res = PyList_New(cfglist.size());
    unsigned i = 0;
    BOOST_FOREACH(const std::string& v, cfglist) {
      PSEvt::Source src(v);
      PyList_SET_ITEM(res, i, psana_python::Source::PyObject_FromCpp(src));
      ++ i;
    }
    return res;
  } catch (const std::exception& ex) {
    PyErr_SetString(PyExc_ValueError, ex.what());
    return 0;
  }
}

PyObject*
extra_skip(PyObject* self, PyObject*)
{
  psana_python::PythonModule* module = cpp_module(self);
  if (not module) return 0;
  module->skip();
  Py_RETURN_NONE;
}

PyObject*
extra_stop(PyObject* self, PyObject*)
{
  psana_python::PythonModule* module = cpp_module(self);
  if (not module) return 0;
  module->stop();
  Py_RETURN_NONE;
}

PyObject*
extra_terminate(PyObject* self, PyObject*)
{
  psana_python::PythonModule* module = cpp_module(self);
  if (not module) return 0;
  module->terminate();
  Py_RETURN_NONE;
}

}
