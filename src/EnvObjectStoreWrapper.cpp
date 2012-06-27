#include <psana_python/EnvObjectStoreWrapper.h>

namespace Psana {
  //using boost::python::api::object;

  object EnvObjectStoreWrapper::get(const string& typeName, const string& sourceName) {
    string typeName2(typeName); // XXX
    PSEvt::Source source = (sourceName == "") ? PSEvt::Source() : PSEvt::Source(sourceName);
    return EnvGetter::get(typeName2, _store, source, NULL);
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
