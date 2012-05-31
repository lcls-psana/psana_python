#ifndef PSANA_EVTGETTER_H
#define PSANA_EVTGETTER_H

#include <string>
#include "GenericGetter.h"
#include <boost/python/class.hpp>
#include "PSEvt/Event.h"

namespace Psana {
  class EvtGetter : public GenericGetter {
  public:
    virtual const std::type_info& getGetterTypeInfo() { return typeid(EvtGetter); }
    virtual boost::python::api::object get(PSEvt::Event& evt, const std::string& key=std::string(), Pds::Src* foundSrc=0) = 0;
    virtual boost::python::api::object get(PSEvt::Event& evt, Pds::Src& src, const std::string& key=std::string(), Pds::Src* foundSrc=0) = 0;
    virtual boost::python::api::object get(PSEvt::Event& evt, PSEvt::Source& source, const std::string& key=std::string(), Pds::Src* foundSrc=0) = 0;
  };
}

#endif // PSANA_EVTGETTER_H
