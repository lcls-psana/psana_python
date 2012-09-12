//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class EpicsPvHeaderWrapper
//
// Author List:
//   Joseph S. Barrera III
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include <psana_python/EpicsPvHeaderWrapper.h>
#include <cmath>

namespace psana_python {

  using boost::python::api::object;

  static std::string typeName(int type) {
    switch (type) {
      case Psana::Epics::DBR_STRING:
        return "DBR_STRING";
      case Psana::Epics::DBR_SHORT:
        return "DBR_SHORT";
      case Psana::Epics::DBR_FLOAT:
        return "DBR_FLOAT";
      case Psana::Epics::DBR_ENUM:
        return "DBR_ENUM";
      case Psana::Epics::DBR_CHAR:
        return "DBR_CHAR";
      case Psana::Epics::DBR_LONG:
        return "DBR_LONG";
      case Psana::Epics::DBR_DOUBLE:
        return "DBR_DOUBLE";
      case Psana::Epics::DBR_STS_STRING:
        return "DBR_STS_STRING";
      case Psana::Epics::DBR_STS_SHORT:
        return "DBR_STS_SHORT";
      case Psana::Epics::DBR_STS_FLOAT:
        return "DBR_STS_FLOAT";
      case Psana::Epics::DBR_STS_ENUM:
        return "DBR_STS_ENUM";
      case Psana::Epics::DBR_STS_CHAR:
        return "DBR_STS_CHAR";
      case Psana::Epics::DBR_STS_LONG:
        return "DBR_STS_LONG";
      case Psana::Epics::DBR_STS_DOUBLE:
        return "DBR_STS_DOUBLE";
      case Psana::Epics::DBR_TIME_STRING:
        return "DBR_TIME_STRING";
      case Psana::Epics::DBR_TIME_SHORT:
        return "DBR_TIME_SHORT";
      case Psana::Epics::DBR_TIME_FLOAT:
        return "DBR_TIME_FLOAT";
      case Psana::Epics::DBR_TIME_ENUM:
        return "DBR_TIME_ENUM";
      case Psana::Epics::DBR_TIME_CHAR:
        return "DBR_TIME_CHAR";
      case Psana::Epics::DBR_TIME_LONG:
        return "DBR_TIME_LONG";
      case Psana::Epics::DBR_TIME_DOUBLE:
        return "DBR_TIME_DOUBLE";
      case Psana::Epics::DBR_GR_STRING:
        return "DBR_GR_STRING";
      case Psana::Epics::DBR_GR_SHORT:
        return "DBR_GR_SHORT";
      case Psana::Epics::DBR_GR_FLOAT:
        return "DBR_GR_FLOAT";
      case Psana::Epics::DBR_GR_ENUM:
        return "DBR_GR_ENUM";
      case Psana::Epics::DBR_GR_CHAR:
        return "DBR_GR_CHAR";
      case Psana::Epics::DBR_GR_LONG:
        return "DBR_GR_LONG";
      case Psana::Epics::DBR_GR_DOUBLE:
        return "DBR_GR_DOUBLE";
      case Psana::Epics::DBR_CTRL_STRING:
        return "DBR_CTRL_STRING";
      case Psana::Epics::DBR_CTRL_SHORT:
        return "DBR_CTRL_SHORT";
      case Psana::Epics::DBR_CTRL_FLOAT:
        return "DBR_CTRL_FLOAT";
      case Psana::Epics::DBR_CTRL_ENUM:
        return "DBR_CTRL_ENUM";
      case Psana::Epics::DBR_CTRL_CHAR:
        return "DBR_CTRL_CHAR";
      case Psana::Epics::DBR_CTRL_LONG:
        return "DBR_CTRL_LONG";
      case Psana::Epics::DBR_CTRL_DOUBLE:
        return "DBR_CTRL_DOUBLE";
      default:
        static char buf[64];
        sprintf(buf, "Psana::Epics::DBR_%d\n", type);
        return buf;
    }
  }

  object EpicsPvHeaderWrapper::value(int index) {
    if (not _header.get()) {
      fprintf(stderr, "value() called on empty object\n");
      return object();
    }
    const Psana::Epics::EpicsPvHeader* p = _header.get();
    int type = p->dbrType();
    switch (type) {
      case Psana::Epics::DBR_TIME_STRING:
        return object(((Psana::Epics::EpicsPvTimeString *) p)->value(index));
      case Psana::Epics::DBR_TIME_SHORT:
        return object(((Psana::Epics::EpicsPvTimeShort *) p)->value(index));
      case Psana::Epics::DBR_TIME_FLOAT:
        return object(((Psana::Epics::EpicsPvTimeFloat *) p)->value(index));
      case Psana::Epics::DBR_TIME_ENUM:
        return object(((Psana::Epics::EpicsPvTimeEnum *) p)->value(index));
      case Psana::Epics::DBR_TIME_CHAR:
        return object(((Psana::Epics::EpicsPvTimeChar *) p)->value(index));
      case Psana::Epics::DBR_TIME_LONG:
        return object(((Psana::Epics::EpicsPvTimeLong *) p)->value(index));
      case Psana::Epics::DBR_TIME_DOUBLE:
        return object(((Psana::Epics::EpicsPvTimeDouble *) p)->value(index));
      case Psana::Epics::DBR_CTRL_STRING:
        return object(((Psana::Epics::EpicsPvCtrlString *) p)->value(index));
      case Psana::Epics::DBR_CTRL_SHORT:
        return object(((Psana::Epics::EpicsPvCtrlShort *) p)->value(index));
      case Psana::Epics::DBR_CTRL_FLOAT:
        return object(((Psana::Epics::EpicsPvCtrlFloat *) p)->value(index));
      case Psana::Epics::DBR_CTRL_ENUM:
        return object(((Psana::Epics::EpicsPvCtrlEnum *) p)->value(index));
      case Psana::Epics::DBR_CTRL_CHAR:
        return object(((Psana::Epics::EpicsPvCtrlChar *) p)->value(index));
      case Psana::Epics::DBR_CTRL_LONG:
        return object(((Psana::Epics::EpicsPvCtrlLong *) p)->value(index));
      case Psana::Epics::DBR_CTRL_DOUBLE:
        return object(((Psana::Epics::EpicsPvCtrlDouble *) p)->value(index));
      default:
        fprintf(stderr, "value(%d) called on unsupported type %s\n", index, typeName(type).c_str());
        return object();
    }
  }

  int16_t EpicsPvHeaderWrapper::precision() {
    if (not _header.get()) {
      fprintf(stderr, "precision() called on empty object\n");
      return 0;
    }
    const Psana::Epics::EpicsPvHeader* p = _header.get();
    int type = p->dbrType();
    switch (type) {
      case Psana::Epics::DBR_CTRL_FLOAT:
        return ((Psana::Epics::EpicsPvCtrlFloat *) p)->dbr().precision();
      case Psana::Epics::DBR_CTRL_DOUBLE:
        return ((Psana::Epics::EpicsPvCtrlDouble *) p)->dbr().precision();
      default:
        fprintf(stderr, "precision() called on unsupported type %s\n", typeName(type).c_str());
        return 0;
    }
  }

  std::string EpicsPvHeaderWrapper::units() {
    if (not _header.get()) {
      fprintf(stderr, "units() called on empty object\n");
      return "undefined";
    }
    const Psana::Epics::EpicsPvHeader* p = _header.get();
    int type = p->dbrType();
    switch (type) {
      case Psana::Epics::DBR_CTRL_SHORT:
        return ((Psana::Epics::EpicsPvCtrlShort *) p)->dbr().units();
      case Psana::Epics::DBR_CTRL_FLOAT:
        return ((Psana::Epics::EpicsPvCtrlFloat *) p)->dbr().units();
      case Psana::Epics::DBR_CTRL_CHAR:
        return ((Psana::Epics::EpicsPvCtrlChar *) p)->dbr().units();
      case Psana::Epics::DBR_CTRL_LONG:
        return ((Psana::Epics::EpicsPvCtrlLong *) p)->dbr().units();
      case Psana::Epics::DBR_CTRL_DOUBLE:
        return ((Psana::Epics::EpicsPvCtrlDouble *) p)->dbr().units();
      default:
        fprintf(stderr, "units() called on unsupported type %s\n", typeName(type).c_str());
        return "undefined";
    }
  }

#define DECL_LIMIT_METHOD(method_name)                                  \
  double EpicsPvHeaderWrapper::method_name () {                         \
    if (not _header.get()) {                                            \
      fprintf(stderr, #method_name "() called on empty object\n");      \
      return NAN;                                                       \
    }                                                                   \
    const Psana::Epics::EpicsPvHeader* p = _header.get();               \
    int type = p->dbrType();                                            \
    switch (type) {                                                     \
      case Psana::Epics::DBR_CTRL_SHORT:  return ((Psana::Epics::EpicsPvCtrlShort *) p)->dbr().method_name (); \
      case Psana::Epics::DBR_CTRL_FLOAT:  return ((Psana::Epics::EpicsPvCtrlFloat *) p)->dbr().method_name (); \
      case Psana::Epics::DBR_CTRL_CHAR:   return ((Psana::Epics::EpicsPvCtrlChar *)  p)->dbr().method_name (); \
      case Psana::Epics::DBR_CTRL_LONG:   return ((Psana::Epics::EpicsPvCtrlLong *)  p)->dbr().method_name (); \
      case Psana::Epics::DBR_CTRL_DOUBLE: return ((Psana::Epics::EpicsPvCtrlDouble *)p)->dbr().method_name (); \
      default:                                                          \
        fprintf(stderr, #method_name "() called on unsupported type %s\n", typeName(type).c_str()); \
        return NAN;                                                     \
    }                                                                   \
  }

DECL_LIMIT_METHOD(upper_disp_limit)
DECL_LIMIT_METHOD(lower_disp_limit)
DECL_LIMIT_METHOD(upper_alarm_limit)
DECL_LIMIT_METHOD(upper_warning_limit)
DECL_LIMIT_METHOD(lower_warning_limit)
DECL_LIMIT_METHOD(lower_alarm_limit)
DECL_LIMIT_METHOD(upper_ctrl_limit)
DECL_LIMIT_METHOD(lower_ctrl_limit)
}
