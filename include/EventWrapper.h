#ifndef PSANA_EVENTWRAPPER_H
#define PSANA_EVENTWRAPPER_H

#include <cxxabi.h>
#include <psana_python/EnvObjectStoreWrapper.h>
#include <string>
#include <boost/python.hpp>
#include <PSEvt/Event.h>
#include <psddl_python/EvtGetter.h>
#include <psddl_python/EvtGetMethod.h>
#include <psddl_python/GenericGetter.h>
#include <PSEvt/EventId.h>

namespace Psana {
  using boost::python::api::object;
  using std::string;
  using PSEvt::EventKey;

  class EventWrapper {
  private:
    Event& _event;
  public:
    EventWrapper(Event& event) : _event(event) {}

#if 0
      GetResultProxy get(const std::string& key=std::string(), Pds::Src* foundSrc=0);
      GetResultProxy get(const Pds::Src& source, const std::string& key=std::string(), Pds::Src* foundSrc=0);
      GetResultProxy get(const Source& source, const std::string& key=std::string(), Pds::Src* foundSrc=0);
#endif

    boost::shared_ptr<string> get(const string& key) {
      return boost::shared_ptr<string>(_event.get(key));
    }

    object getByType(const string& typeName, Source& detectorSource /*const string& detectorSourceName*/) {
      if (typeName == "PSEvt::EventId") {
        const boost::shared_ptr<PSEvt::EventId> eventId = _event.get();
        return object(eventId);
      }

#if 0
      //printAllKeys(evt);
      Source detectorSource;
      if (detectorSourceName == "") {
        detectorSource = Source();
      } else {
        detectorSource = Source(detectorSourceName);
      }
#endif

      EvtGetMethod method(_event);
      method.addSource(&detectorSource);
      string typeName2(typeName);
      return GenericGetter::get(typeName2, &method);
    }

    list<string> getAllKeys() {
      Event::GetResultProxy proxy = _event.get();
      list<EventKey> keys;
      proxy.m_dict->keys(keys, Source());

      list<string> keyNames;
      list<EventKey>::iterator it;
      for (it = keys.begin(); it != keys.end(); it++) {
        EventKey& key = *it;
        cout << "THIS is a key: " << key << endl;

        //cout << "THIS is a key typeid: " << key << endl;

        int status;
        char* keyName = abi::__cxa_demangle(key.typeinfo()->name(), 0, 0, &status);

        cout << "THIS is a keyName: " << keyName << endl;
        keyNames.push_back(string(keyName));
      }
      return keyNames;
    }

    int run() {
      const boost::shared_ptr<PSEvt::EventId> eventId = _event.get();
      return eventId->run();
    }


  };

}

#endif // PSANA_EVENTWRAPPER_H
