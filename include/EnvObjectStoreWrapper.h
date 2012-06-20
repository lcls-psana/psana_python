#ifndef PSANA_ENVOBJECTSTOREWRAPPER_H
#define PSANA_ENVOBJECTSTOREWRAPPER_H

#include <string>
#include <boost/python.hpp>
#include <PSEnv/Env.h>
#include <psddl_python/EnvGetter.h>
#include <psddl_python/EnvGetMethod.h>

namespace Psana {

  // Need wrapper because EnvObjectStore is boost::noncopyable
  class EnvObjectStoreWrapper {
  private:
    PSEnv::EnvObjectStore& _store;
  public:
    EnvObjectStoreWrapper(PSEnv::EnvObjectStore& store) : _store(store) {}
    // template <typename T> void putProxy(const boost::shared_ptr<PSEvt::Proxy<T> >& proxy, const Pds::Src& source);
    // template <typename T> void put(const boost::shared_ptr<T>& data, const Pds::Src& source);

    boost::python::api::object get(const string& typeName, const string& sourceName) {
      PSEvt::Source source = (sourceName == "") ? PSEvt::Source() : PSEvt::Source(sourceName);
      Pds::Src foundSrc;
      EnvGetMethod method(_store, source, &foundSrc);
      string typeName2(typeName);
      boost::python::api::object result = GenericGetter::get(typeName2, &method);
      return result;
    }

    Pds::Src foundSrc(const string& typeName, const string& sourceName) {
      PSEvt::Source source = (sourceName == "") ? PSEvt::Source() : PSEvt::Source(sourceName);
      Pds::Src foundSrc;
      EnvGetMethod method(_store, source, &foundSrc);
      printf("%x.%x\n", foundSrc.log(), foundSrc.phy());
      return foundSrc;
    }

    list<PSEvt::EventKey> keys(const PSEvt::Source& source = PSEvt::Source()) const { return _store.keys(); }
  };
}

#endif // PSANA_ENVOBJECTSTOREWRAPPER_H
