#include <nan.h>
#include "scws.h"

using namespace v8;

void InitAll(Handle<Object> exports) {
  Scws::Init(exports);
}

NODE_MODULE(addon, InitAll)
