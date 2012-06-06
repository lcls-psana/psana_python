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

    PSEnv::EnvObjectStore::GetResultProxy getBySrc(const Pds::Src& src) {
      return _store.get(src);
    }

    PSEnv::EnvObjectStore::GetResultProxy getBySource(const PSEvt::Source& source, Pds::Src* foundSrc = 0) {
      return _store.get(source, foundSrc);
    }

    boost::python::api::object getByType2(const string& typeName, const PSEvt::Source& source, boost::python::list m_src) {
      Pds::Src foundSrc;
      EnvGetMethod method(_store, source, &foundSrc);
      string typeName2(typeName);
      boost::python::api::object result = GenericGetter::get(typeName2, &method);
      m_src.append(foundSrc);
      printf("%x.%x\n", foundSrc.log(), foundSrc.phy());
      return result;
    }

    boost::python::api::object getByType1(const string& typeName, const PSEvt::Source& source) {
      boost::python::list m_src;
      return getByType2(typeName, source, m_src);
    }

    list<PSEvt::EventKey> keys(const PSEvt::Source& source = PSEvt::Source()) const { return _store.keys(); }
  };
}

#endif // PSANA_ENVOBJECTSTOREWRAPPER_H
