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
#include <boost/python.hpp>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "MsgLogger/MsgLogger.h"
#include "psana_python/AliasMap.h"
#include "psana_python/Env.h"
#include "psana_python/EnvObjectStore.h"
#include "psana_python/EventId.h"
#include "psana_python/EventKey.h"
#include "psana_python/Event.h"
#include "psana_python/EpicsStore.h"
#include "psana_python/PdsBldInfo.h"
#include "psana_python/PdsClockTime.h"
#include "psana_python/PdsDetInfo.h"
#include "psana_python/PdsProcInfo.h"
#include "psana_python/PdsSrc.h"
#include "psana_python/PythonModule.h"
#include "psana_python/Source.h"
#include "psana_python/SrcMatch.h"
#include "psddl_python/ConverterMap.h"
#include "psddl_python/ConverterFun.h"
#include "psddl_python/CreateDeviceWrappers.h"
#include "psana_python/Ndarray2CppCvt.h"
#include "psana_python/NdarrayCvt.h"
#include "psana_python/StringCvt.h"
#include "psana_python/python_converter.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

using psddl_python::ConverterMap;
using psddl_python::make_converter_fun;

namespace {

  template <typename PdsType, typename PsanaPyType>
  struct PdsSrcConverter {
    static PyObject* convert(PdsType const& pds) { return PsanaPyType::PyObject_FromCpp(pds); }
    static PyTypeObject const* get_pytype() { return PsanaPyType::typeObject(); }
  };

  bool
  _createWrappers(PyObject* module)
  {

    // initialize all local types
    psana_python::AliasMap::initType(module);
    psana_python::Env::initType(module);
    psana_python::EnvObjectStore::initType(module);
    psana_python::EpicsStore::initType(module);
    psana_python::Event::initType(module);
    psana_python::EventId::initType(module);
    psana_python::EventKey::initType(module);
    psana_python::PdsBldInfo::initType(module);
    psana_python::PdsClockTime::initType(module);
    psana_python::PdsDetInfo::initType(module);
    psana_python::PdsProcInfo::initType(module);
    psana_python::PdsSrc::initType(module);
    psana_python::Source::initType(module);
    psana_python::SrcMatch::initType(module);

    // register conversion for some classes
    ConverterMap& cmap = ConverterMap::instance();
    cmap.addConverter(make_converter_fun<PSEvt::EventId>(std::ptr_fun(&psana_python::EventId::PyObject_FromCpp),
        psana_python::EventId::typeObject()));

    // register converter for standard Python str (takes no module argument)
    cmap.addConverter(boost::make_shared<psana_python::StringCvt>());

    // instantiate all sub-modules
    psddl_python::createDeviceWrappers(module);

    // to help boost we need to register convertes for several types that we define here
    boost::python::to_python_converter<Pds::Src, PdsSrcConverter<Pds::Src, psana_python::PdsSrc>, true>();
    boost::python::to_python_converter<Pds::BldInfo, PdsSrcConverter<Pds::BldInfo, psana_python::PdsBldInfo>, true>();
    boost::python::to_python_converter<Pds::ClockTime, PdsSrcConverter<Pds::ClockTime, psana_python::PdsClockTime>, true>();
    boost::python::to_python_converter<Pds::DetInfo, PdsSrcConverter<Pds::DetInfo, psana_python::PdsDetInfo>, true>();
    boost::python::to_python_converter<Pds::ProcInfo, PdsSrcConverter<Pds::ProcInfo, psana_python::PdsProcInfo>, true>();

    // must be after psddl_python as it needs numpy initialization which
    // happens in psddl_python
    psana_python::initNdarrayCvt(cmap, module);
    cmap.addConverter(boost::make_shared<psana_python::Ndarray2CppCvt>());

    // Add additional python converters
    psana_python::createConverters();
    
    // add few constants
    PyModule_AddIntConstant(module, "Normal", psana_python::PythonModule::Normal);
    PyModule_AddIntConstant(module, "Skip", psana_python::PythonModule::Skip);
    PyModule_AddIntConstant(module, "Stop", psana_python::PythonModule::Stop);
    PyModule_AddIntConstant(module, "Terminate", psana_python::PythonModule::Terminate);

    return true;
  }

}

namespace psana_python {

void
createWrappers(PyObject* module)
{
  static bool createWrappersDone __attribute__((unused)) = _createWrappers(module);
}

}
