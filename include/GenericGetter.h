#ifndef PSANA_GENERICGETTER_H
#define PSANA_GENERICGETTER_H

#include <typeinfo>

namespace Psana {
  class GenericGetter {
  public:
    virtual const std::type_info& getGetterTypeInfo() = 0;
    virtual const char* getTypeName() = 0;
    virtual int getTypeId() { return -1; }
    virtual int getVersion() { return 0; }
    virtual ~GenericGetter() {}
    template<class T> 
    static T* getGetterByType(const char* typeName);
    static GenericGetter* getGetterByType(const char* typeName);
    static void addGetter(GenericGetter* getter);
  };

}

#endif // PSANA_GENERICGETTER_H
