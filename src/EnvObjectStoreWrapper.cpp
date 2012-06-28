#include <psana_python/EnvObjectStoreWrapper.h>
#include <psddl_python/EnvObjectStoreGetter.h>
#include <sstream>

namespace Psana {
  object EnvObjectStoreWrapper::get(const string& typeName, const string& sourceName) {
    PSEvt::Source source = (sourceName == "") ? PSEvt::Source() : PSEvt::Source(sourceName);
    return EnvObjectStoreGetter::get(typeName, _store, source, NULL);
  }

  boost::python::list EnvObjectStoreWrapper::keys() {
    boost::python::list l;
    list<PSEvt::EventKey> keys = _store.keys();
    list<PSEvt::EventKey>::iterator it;
    for (it = keys.begin(); it != keys.end(); it++) {
      std::ostringstream stream;
      it->print(stream);
      string key = stream.str();
      l.append(key);
    }
    return l;
  }
}
