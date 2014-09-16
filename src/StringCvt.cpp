//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class StringCvt...
//
// Author List:
//      David Schneider
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "psana_python/StringCvt.h"

//-----------------
// C/C++ Headers --
//-----------------
#include <stdexcept>
#include <string>
#include <boost/make_shared.hpp>
//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "PSEvt/DataProxy.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------

namespace psana_python {

//----------------
// Constructors --
//----------------
StringCvt::StringCvt ()
  : Converter()
{
}

//--------------
// Destructor --
//--------------
StringCvt::~StringCvt ()
{
}

/// Return type_info of the corresponding C++ type.
std::vector<const std::type_info*>
StringCvt::from_cpp_types() const
{
  std::vector<const std::type_info*> types;
  types.push_back(&typeid(std::string));
  return types;
}

/// Returns source Python types.
std::vector<PyTypeObject*>
StringCvt::from_py_types() const
{
  std::vector<PyTypeObject*> types;
  types.push_back(&PyString_Type);
  return types;
}

/// Returns destination Python types.
std::vector<PyTypeObject*>
StringCvt::to_py_types() const
{
  std::vector<PyTypeObject*> types;
  types.push_back(&PyString_Type);
  return types;
}


/// Convert C++ object to Python
PyObject*
StringCvt::convert(PSEvt::ProxyDictI& proxyDict, const PSEvt::Source& source, const std::string& key) const
{
  const char * data = 0;

  if (boost::shared_ptr<void> vdata = proxyDict.get(&typeid(std::string), source, key, 0)) {
    const std::string & cppString = *boost::static_pointer_cast<std::string>(vdata);
    data = cppString.c_str();
    if (data == NULL) {
      throw std::runtime_error("C++ std::string.c_str() in event store is 0");
      return 0;
    }
    return PyString_FromString(data);
  }
  return 0;
}

bool StringCvt::convert(PyObject* obj, PSEvt::ProxyDictI& proxyDict, const Pds::Src& source, const std::string& key) const
{
  if (not PyString_Check(obj)) return false;
  boost::shared_ptr<std::string> cppStr = boost::make_shared<std::string>(PyString_AsString(obj));
  PSEvt::EventKey evKey = PSEvt::EventKey(&typeid(std::string), source, key);
  boost::shared_ptr<PSEvt::ProxyI> proxyPtr = boost::make_shared<PSEvt::DataProxy<std::string> >(cppStr);
  proxyDict.put(proxyPtr, evKey);
  return true;
}

} // namespace psana_python
