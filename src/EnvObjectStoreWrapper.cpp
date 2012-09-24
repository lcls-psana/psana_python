
#include <psana_python/EnvObjectStoreWrapper.h>

#include <list>
#include <sstream>

#include <psddl_python/EnvObjectStoreGetter.h>

namespace psana_python {

boost::python::object
EnvObjectStoreWrapper::get(const std::string& typeName, const std::string& sourceName)
{
  PSEvt::Source source = (sourceName == "") ? PSEvt::Source() : PSEvt::Source(sourceName);
  return psddl_python::EnvObjectStoreGetter::get(typeName, *_store, source, NULL);
}

boost::python::list
EnvObjectStoreWrapper::keys()
{
  boost::python::list l;
  std::list<PSEvt::EventKey> keys = _store->keys();
  for (std::list<PSEvt::EventKey>::iterator it = keys.begin(); it != keys.end(); ++it) {
    std::ostringstream stream;
    it->print(stream);
    std::string key = stream.str();
    l.append(key);
  }
  return l;
}

}
