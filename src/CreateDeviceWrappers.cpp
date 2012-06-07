namespace Psana {
#undef DECL
#define DECL(ns) namespace ns { extern void createWrappers(); }
#include "WrapperList.txt"
  namespace CreateWrappers {
    void createDeviceWrappers() {
#undef DECL
#define DECL(ns) ns::createWrappers();
#include "WrapperList.txt"
    }
  } // namespace CreateWrappers
} // namespace Psana