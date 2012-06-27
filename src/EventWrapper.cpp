#include <psana_python/EventWrapper.h>
#include <cxxabi.h>
#include <string>
#include <boost/python.hpp>
#include <PSEvt/Event.h>
#include <psddl_python/EventGetter.h>
#include <PSEvt/EventId.h>

namespace Psana {

  void EventWrapper::putBoolean(bool value, string key) {
    printf("put(key=%s, %s)\n", key.c_str(), value ? "true" : "false");
    const shared_ptr<bool> v(new bool(value));
    try {
      _event.put(v, key);
    } catch (...) {
      printf("problem with put(key=%s, %s)\n", key.c_str(), value ? "true" : "false");
    }
  }

  void EventWrapper::putList(boost::python::list list, string key) {
    boost::python::ssize_t n = boost::python::len(list);
    printf("putList(key=%s): len(list)=%d\n", key.c_str(), n);
    const shared_ptr<boost::python::list> l = boost::make_shared<boost::python::list>(list);
    _event.put(l, key);
  }

  object EventWrapper::get(const string& key) {
    //printf("get(key=%s)\n", key.c_str());
    shared_ptr<string> s(_event.get(key));
    if (s.get()) {
      //printf("get(%s) = %s\n", key.c_str(), ss.c_str());
      return object(s);
    }
    shared_ptr<bool> b(_event.get(key));
    if (b.get()) {
      bool bb = *b;
      //printf("get(%s) = %s\n", key.c_str(), (bb ? "true" : "false"));
      return object(bb);
    }
    shared_ptr<boost::python::list> l(_event.get(key));
    if (l.get()) {
      boost::python::list ll = *l;
      //printf("get(%s): is a list\n", key.c_str());
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
    string typeName2(typeName);
    Source source = (detectorSourceName == "") ? Source() : Source(detectorSourceName);
    return EventGetter::get(typeName2, _event, source, "", NULL);
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
