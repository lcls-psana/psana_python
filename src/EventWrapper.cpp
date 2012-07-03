#include <cxxabi.h>
#include <string>
#include <boost/python.hpp>
#include <PSEvt/Event.h>
#include <PSEvt/EventId.h>
#include <psana_python/EventWrapper.h>
#include <psddl_python/EventGetter.h>

namespace Psana {
  using boost::shared_ptr;

  void EventWrapper::putBoolean(bool value, string key) {
    //printf("put('%s', %s)\n", key.c_str(), value ? "true" : "false");
    const shared_ptr<bool> v(new bool(value));
    try {
      _event.put(v, key);
    } catch (...) {
      printf("problem with put(key=%s, %s)\n", key.c_str(), value ? "true" : "false");
    }
  }

  void EventWrapper::putList(boost::python::list list, string key) {
    boost::python::ssize_t n = boost::python::len(list);
    //printf("put('%s', list(len=%d))\n", key.c_str(), n);
    const shared_ptr<boost::python::list> l = boost::make_shared<boost::python::list>(list);
    _event.put(l, key);
  }

  string getDemangledKeyTypeName(const EventKey& eventKey) {
    int status;
    const char* mangledTypeName = eventKey.typeinfo()->name();
    const char* unmangledTypeName = abi::__cxa_demangle(mangledTypeName, 0, 0, &status);
    if (status == 0 && unmangledTypeName) {
      //printf("demangled %s -> %s\n", mangledTypeName, unmangledTypeName);
      return unmangledTypeName;
    }
    fprintf(stderr, "error: get('%s'): could not demangle type '%s'\n", eventKey.key().c_str(), mangledTypeName);
    return mangledTypeName;
  }

  object EventWrapper::getValue(const string& key, const EventKey& eventKey) {
    string typeName = getDemangledKeyTypeName(eventKey);
    if (typeName == "bool") {
      shared_ptr<bool> result(_event.get(key));
      bool value = *result;
      //printf("get('%s') -> %s\n", key.c_str(), value ? "true" : "false");
      return object(value);
    }
    if (typeName == "boost::python::list") {
      shared_ptr<boost::python::list> result(_event.get(key));
      boost::python::list l = *result;
      boost::python::ssize_t len = boost::python::len(l);
      //printf("get('%s') -> list(len=%d)\n", key.c_str(), boost::python::len(l));
      return object(l);
    }

    fprintf(stderr, "**************************************** get('%s'): unknown type %s\n\n\n\n\n", key.c_str(), typeName.c_str());
#if 1
    exit(1);
#endif
    return object();
  }

  object EventWrapper::get(const string& key) {
    PSEvt::Event::GetResultProxy proxy(_event.get(key));
    std::list<EventKey> keys;
    proxy.m_dict->keys(keys, Source(Source::null));
    std::list<EventKey>::iterator it;
    for (it = keys.begin(); it != keys.end(); it++) {
      const EventKey& eventKey = *it;
      if (eventKey.key() == key) {
        return getValue(key, eventKey);
      }
    }
    // nothing found for key
    return object();
  }

  object EventWrapper::getByType(const string& typeName, const string& detectorSourceName) {
    if (typeName == "PSEvt::EventId") {
      printf("!!!! aha! EventId\n");
      exit(1);
      const shared_ptr<PSEvt::EventId> eventId = _event.get();
      return object(eventId);
    }
    Source source = (detectorSourceName == "") ? Source() : Source(detectorSourceName);
    return EventGetter::get(typeName, _event, source, "", NULL);
  }

  boost::python::list EventWrapper::keys() {
    Event::GetResultProxy proxy = _event.get();
    std::list<EventKey> keys;
    proxy.m_dict->keys(keys, Source());

    boost::python::list keyNames;
    std::list<EventKey>::iterator it;
    for (it = keys.begin(); it != keys.end(); it++) {
      EventKey& key = *it;
      int status;
      char* keyName = abi::__cxa_demangle(key.typeinfo()->name(), 0, 0, &status);
      keyNames.append(string(keyName));
    }
    return keyNames;
  }

  int EventWrapper::run() {
    const shared_ptr<PSEvt::EventId> eventId = _event.get();
    return eventId->run();
  }
}
