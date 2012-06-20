#include <psana_python/EventWrapper.h>
#include <cxxabi.h>
#include <string>
#include <boost/python.hpp>
#include <PSEvt/Event.h>
#include <psddl_python/EvtGetMethod.h>
#include <psddl_python/GenericGetter.h>
#include <PSEvt/EventId.h>

namespace Psana {

  object EventWrapper::get(const string& key) {
    printf("get(key=%s)\n", key.c_str());
    shared_ptr<string> s(_event.get(key));
    if (s.get()) {
      string& ss = *s;
      printf("get(%s) = %s\n", key.c_str(), ss.c_str());
      return object(s);
    }
    shared_ptr<bool> b(_event.get(key));
    if (b.get()) {
      bool bb = *b;
      printf("get(%s) = %s\n", key.c_str(), (bb ? "true" : "false"));
      return object(bb);
    }
    shared_ptr<boost::python::list> l(_event.get(key));
    if (l.get()) {
      boost::python::list ll = *l;
      printf("get(%s): is a list\n", key.c_str());
      return object(ll);
    }
    printf("WARNING: get(%s) found nothing of a known type\n", key.c_str());
    return object();
  }

  object EventWrapper::getByType(const string& typeName, const string& detectorSourceName) {
    if (typeName == "PSEvt::EventId") {
      const shared_ptr<PSEvt::EventId> eventId = _event.get();
      return object(eventId);
    }
    EvtGetMethod method(_event);
    Source source = (detectorSourceName == "") ? Source() : Source(detectorSourceName);
    method.addSource(&source);
    string typeName2(typeName);
    return GenericGetter::get(typeName2, &method);
  }

  list<string> EventWrapper::getAllKeys() {
    Event::GetResultProxy proxy = _event.get();
    list<EventKey> keys;
    proxy.m_dict->keys(keys, Source());

    list<string> keyNames;
    list<EventKey>::iterator it;
    for (it = keys.begin(); it != keys.end(); it++) {
      EventKey& key = *it;
      int status;
      char* keyName = abi::__cxa_demangle(key.typeinfo()->name(), 0, 0, &status);
      printf("getAllKeys: %s\n", keyName);
      keyNames.push_back(string(keyName));
    }
    return keyNames;
  }

  int EventWrapper::run() {
    const shared_ptr<PSEvt::EventId> eventId = _event.get();
    return eventId->run();
  }
}
