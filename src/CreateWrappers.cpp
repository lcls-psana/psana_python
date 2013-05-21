//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class PythonHelp
//
// Author List:
//   Joseph S. Barrera III
//
//------------------------------------------------------------------------

// Python header first to suppress warnings
#include "python/Python.h"

//-----------------------
// This Class's Header --
//-----------------------

//-----------------
// C/C++ Headers --
//-----------------
#include <string>
#include <functional>
#include <boost/make_shared.hpp>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "MsgLogger/MsgLogger.h"
#include "psana_python/Env.h"
#include "psana_python/EnvObjectStore.h"
#include "psana_python/EventId.h"
#include "psana_python/EventKey.h"
#include "psana_python/Event.h"
#include "psana_python/EpicsStore.h"
#include "psana_python/PdsBldInfo.h"
#include "psana_python/PdsDetInfo.h"
#include "psana_python/PdsProcInfo.h"
#include "psana_python/PdsSrc.h"
#include "psana_python/Source.h"
#include "psddl_python/ConverterMap.h"
#include "psddl_python/ConverterFun.h"
#include "psddl_python/CreateDeviceWrappers.h"
#include "psana_python/Ndarray2CppCvt.h"
#include "psana_python/NdarrayCvt.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

using psddl_python::ConverterMap;
using psddl_python::make_converter_fun;

namespace {

  bool
  _createWrappers(PyObject* module)
  {

    // initialize all local types
    psana_python::Env::initType(module);
    psana_python::EnvObjectStore::initType(module);
    psana_python::EpicsStore::initType(module);
    psana_python::Event::initType(module);
    psana_python::EventId::initType(module);
    psana_python::EventKey::initType(module);
    psana_python::PdsBldInfo::initType(module);
    psana_python::PdsDetInfo::initType(module);
    psana_python::PdsProcInfo::initType(module);
    psana_python::PdsSrc::initType(module);
    psana_python::Source::initType(module);

    // register conversion for some classes
    ConverterMap& cmap = ConverterMap::instance();
    cmap.addConverter(make_converter_fun<PSEvt::EventId>(std::ptr_fun(&psana_python::EventId::PyObject_FromCpp),
        psana_python::EventId::typeObject()));

    // instantiate all sub-modules
    psddl_python::createDeviceWrappers(module);

    // must be after psddl_python as it needs numpy initialization which
    // happens in psddl_python
    psana_python::initNdarrayCvt(cmap, module);
    cmap.addConverter(boost::make_shared<psana_python::Ndarray2CppCvt>());

    return true;
  }

}

namespace psana_python {

void
createWrappers(PyObject* module)
{
  static bool createWrappersDone = _createWrappers(module);
  // just to suppress warning about unused variable
  if (not createWrappersDone) {
    createWrappersDone = true;
  }
}

}
