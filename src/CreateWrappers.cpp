#undef DECL
#define DECL(ns) namespace ns { extern void createWrappers(); }
namespace Psana {
  #include "WrapperList.txt"
}

#undef DECL
#define DECL(ns) ns::createWrappers();
namespace Psana {
  namespace CreateWrappers {
    void createWrappers() {
        #include "WrapperList.txt"
    }
  }
}
