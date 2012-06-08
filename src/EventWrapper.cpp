#include <psana_python/EventWrapper.h>
#include <cxxabi.h>
#include <string>
#include <boost/python.hpp>
#include <PSEvt/Event.h>
#include <psddl_python/EvtGetMethod.h>
#include <psddl_python/GenericGetter.h>
#include <PSEvt/EventId.h>

using PSEvt::EventKey;
using PSEvt::Source;
using boost::python::api::object;

namespace Psana {

  object EventWrapper::getByType(const string& typeName, Source& detectorSource) {
    if (typeName == "PSEvt::EventId") {
      const boost::shared_ptr<PSEvt::EventId> eventId = _event.get();
      return object(eventId);
    }
    EvtGetMethod method(_event);
    method.addSource(&detectorSource);
    string typeName2(typeName);
    return GenericGetter::get(typeName2, &method);
  }

  object EventWrapper::getByTypeId(int typeId, const string& detectorSourceName) {
    Source detectorSource;
    if (detectorSourceName == "") {
      detectorSource = Source();
    } else {
      detectorSource = Source(detectorSourceName);
    }

    string typeName(GenericGetter::getTypeNameForId(typeId));
    EvtGetMethod method(_event);
    method.addSource(&detectorSource);
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
    const boost::shared_ptr<PSEvt::EventId> eventId = _event.get();
    return eventId->run();
  }
}
