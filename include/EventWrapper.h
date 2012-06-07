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



    boost::shared_ptr<string> get(const string& key) {
      return boost::shared_ptr<string>(_event.get(key));
    }

    object getByType_Event(const string& typeName, const string& detectorSourceName) {
      if (typeName == "PSEvt::EventId") {
        const boost::shared_ptr<PSEvt::EventId> eventId = _event.get();
        return object(eventId);
      }

      //printAllKeys_Event(evt);
      Source detectorSource;
      if (detectorSourceName == "") {
        detectorSource = Source();
      } else {
        detectorSource = Source(detectorSourceName);
      }

      EvtGetMethod method(_event);
      method.addSource(&detectorSource);
      string typeName2(typeName);
      return GenericGetter::get(typeName2, &method);
    }

    list<string> getAllKeys_Event() {
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

    int run_Event() {
      const boost::shared_ptr<PSEvt::EventId> eventId = _event.get();
      return eventId->run();
    }

    boost::shared_ptr<string> get_Event(const string& key) {
      return boost::shared_ptr<string>(_event.get(key));
    }

  };

}

#endif // PSANA_EVENTWRAPPER_H
