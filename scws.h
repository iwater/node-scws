#ifndef TFSCWS_H
#define THSCWS_H

#include <nan.h>
#include <node.h>
#include <node_object_wrap.h>
#include "scws/scws.h"

class Scws : public node::ObjectWrap {
 public:
  static void Init(v8::Handle<v8::Object> exports);

 private:
  explicit Scws();
  ~Scws();

  static v8::Persistent<v8::Function> constructor;
  static NAN_METHOD(New);
  static NAN_METHOD(addDict);
  static NAN_METHOD(setDict);
  static NAN_METHOD(setMulti);
  static NAN_METHOD(setRule);
  static NAN_METHOD(segment);
  static NAN_METHOD(serialize);
  scws_t scws;
};

#endif
