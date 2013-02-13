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

//-----------------------
// This Class's Header --
//-----------------------

//-----------------
// C/C++ Headers --
//-----------------
#include <string>
#include <functional>

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
        psana_python::EventId::typeObject(), -1, -1));

    // instantiate all sub-modules
    psddl_python::createDeviceWrappers(module);

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
