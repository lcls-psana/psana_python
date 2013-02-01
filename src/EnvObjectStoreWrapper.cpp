
#include "psana_python/EnvObjectStoreWrapper.h"

#include <list>
#include <sstream>

#include "psana_python/EventKey.h"
#include "psddl_python/GetterMap.h"

using psddl_python::GetterMap;
using psddl_python::Getter;


namespace psana_python {

boost::python::object
EnvObjectStoreWrapper::get(const std::string& typeName, const std::string& sourceName)
{
  PSEvt::Source source = (sourceName == "") ? PSEvt::Source() : PSEvt::Source(sourceName);

  // find a getter for this name
  GetterMap& gmap = GetterMap::instance();
  boost::shared_ptr<Getter> getter = gmap.getGetter(typeName);
  if (getter) {
    boost::shared_ptr<void> vdata = _store->proxyDict()->get(&getter->typeinfo(), source, std::string(), 0);
    return getter->convert(vdata);
  }

  // try to find matching type from a range of types
  const GetterMap::NameList& names = gmap.getTemplate(typeName);
  for (GetterMap::NameList::const_iterator it = names.begin(); it != names.end(); ++ it) {
    boost::shared_ptr<Getter> getter = gmap.getGetter(*it);
    if (getter) {
      boost::shared_ptr<void> vdata = _store->proxyDict()->get(&getter->typeinfo(), source, std::string(), 0);
      if (vdata) {
        return getter->convert(vdata);
      }
    }
  }
  return boost::python::object();
}

boost::python::list
EnvObjectStoreWrapper::keys()
{
  const std::list<PSEvt::EventKey>& keys = _store->keys();

  boost::python::list pkeys;
  for (std::list<PSEvt::EventKey>::const_iterator it = keys.begin(); it != keys.end(); ++it) {
    PyObject* key = EventKey::PyObject_FromCpp(*it);
    pkeys.append(boost::python::handle<PyObject>(key));
  }
  return pkeys;
}

}
