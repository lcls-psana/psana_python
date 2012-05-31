//#include <psddl_pyana/acqiris.ddl.wrapper.h>

#define DECL(ns) namespace ns { extern void createWrappers(); }

namespace Psana {
  #include <psana_python/CreateWrappers.h>
}

#undef DECL
#define DECL(ns) ns::createWrappers();

namespace Psana {
  namespace CreateWrappers {
    void createWrappers() {
      #include <psana_python/CreateWrappers.h>
    }
  }
}
