#include <string>
#include <map>
#include <psddl_pyana/GenericGetter.h>

using std::string;
using std::map;

namespace Psana {

static map<string, GenericGetter*> getter_map;

static string getTypeNameWithHighestVersion(const string& typeNameGeneric) {
  if (getter_map.count(typeNameGeneric)) {
    return typeNameGeneric;
  }
  string oldTypeName = "";
  char versionSuffix[256];
  for (int version = 1; true; version++) {
    sprintf(versionSuffix, "V%d", version);
    string newTypeName = typeNameGeneric + versionSuffix;
    if (! getter_map.count(newTypeName)) {
      return oldTypeName;
    }
    oldTypeName = newTypeName;
  }
}

GenericGetter* GenericGetter::getGetterByType(const char* typeNameGeneric) {
  string typeName = getTypeNameWithHighestVersion(string(typeNameGeneric));
  if (typeName == "") {
    return NULL;
  }
  return getter_map[typeName];
}

void GenericGetter::addGetter(GenericGetter* getter) {
  getter_map[getter->getTypeName()] = getter;
  printf("~~~ adding %s\n", getter->getTypeName());
  if (getter->getTypeId() != -1) {
    char name[64];
    sprintf(name, "@EnvType_%d_V%d", getter->getTypeId(), getter->getVersion());
    getter_map[name] = getter;
    printf("~~~ adding %s\n", name);
  }
}

template<class T> T* GenericGetter::getGetterByType(const char* typeName) {
  printf("~~~ looking for %s\n", typeName);
  GenericGetter* getter = GenericGetter::getGetterByType(typeName);
  if (getter->getGetterTypeInfo() != typeid(T)) {
    return NULL;
  }
  return (T*) getter;
}

}
