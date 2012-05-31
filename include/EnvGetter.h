#ifndef PSANA_ENVGETTER_H
#define PSANA_ENVGETTER_H 1

#include <boost/python/class.hpp>
#include "GenericGetter.h"
#include "PSEnv/Env.h"

namespace Psana {
  class EnvGetter : public GenericGetter {
  public:
    virtual const std::type_info& getGetterTypeInfo() { return typeid(EnvGetter); }
    virtual boost::python::api::object get(PSEnv::EnvObjectStore& store, const PSEvt::Source& src) = 0;
  };
}

#endif // PSANA_ENVGETTER_H
