#include "scws.h"

using namespace v8;

Persistent<Function> Scws::constructor;

Scws::Scws() {
  scws = scws_new();
  scws_set_charset(scws, "utf8");
}

Scws::~Scws() {
  scws_free(scws);
}

void Scws::addDict(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  Nan::HandleScope scope;
  Scws *scws = ObjectWrap::Unwrap<Scws>(info.Holder());
  Handle<Value> arg0 = info[0];
  String::Utf8Value dict(arg0);
  if (strstr(*dict, "xdb")) {
    scws_add_dict(scws->scws, *dict, SCWS_XDICT_MEM);
  } else {
    scws_add_dict(scws->scws, *dict, SCWS_XDICT_TXT);
  }
  info.GetReturnValue().Set(Nan::Undefined());
}

void Scws::setDict(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  Nan::HandleScope scope;
  Scws *scws = ObjectWrap::Unwrap<Scws>(info.Holder());
  Handle<Value> arg0 = info[0];
  String::Utf8Value dict(arg0);
  if (strstr(*dict, "xdb")) {
    scws_set_dict(scws->scws, *dict, SCWS_XDICT_MEM);
  } else {
    scws_set_dict(scws->scws, *dict, SCWS_XDICT_TXT);
  }
  info.GetReturnValue().Set(Nan::Undefined());
}

void Scws::setMulti(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  Nan::HandleScope scope;
  Scws *scws = ObjectWrap::Unwrap<Scws>(info.Holder());
  int multi = info[0]->NumberValue();
  if (multi == 1) {
    scws_set_multi(scws->scws, SCWS_MULTI_SHORT);
  } else {
    scws_set_multi(scws->scws, 0);
  }
  info.GetReturnValue().Set(Nan::Undefined());
}

void Scws::setRule(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  Nan::HandleScope scope;
  Scws *scws = ObjectWrap::Unwrap<Scws>(info.Holder());
  Handle<Value> arg0 = info[0];
  String::Utf8Value rule(arg0);
  scws_set_rule(scws->scws, *rule);
  info.GetReturnValue().Set(Nan::Undefined());
}

void Scws::segment(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  Nan::HandleScope scope;
  Scws *scwsObj = ObjectWrap::Unwrap<Scws>(info.Holder());
    scws_top_t res, cur;
    Handle<Value> arg0 = info[0];
    String::Utf8Value txt(arg0);
    int txtLen = strlen(*txt);
    int limit = txtLen;
    if (info[1]->IsNumber()) {
      limit = info[1]->NumberValue();
    }
    scws_t scws = scwsObj->scws;
    scws_send_text(scws, *txt, txtLen);
    Local<Array> tops = Nan::New<Array>();
    int index = 0;

    cur = res = scws_get_tops(scws, limit, NULL);
    while(cur != NULL){
        v8::Local<v8::Object> objWord = Nan::New<v8::Object>();
        objWord->Set(Nan::New<String>("word").ToLocalChecked(), Nan::New<String>(cur->word).ToLocalChecked());
        objWord->Set(Nan::New<String>("weight").ToLocalChecked(), Nan::New<Number>(cur->weight));
        objWord->Set(Nan::New<String>("times").ToLocalChecked(), Nan::New<Integer>(cur->times));
        if(cur->attr[1] == '\0'){
            objWord->Set(Nan::New<String>("attr").ToLocalChecked(), Nan::New<String>(cur->attr, 1).ToLocalChecked());
        }else{
            objWord->Set(Nan::New<String>("attr").ToLocalChecked(), Nan::New<String>(cur->attr, 2).ToLocalChecked());
        }
        cur = cur->next;
        tops->Set(index, objWord);
        index += 1;
    }
    scws_free_tops(res);
    scws_free_tops(cur);
    info.GetReturnValue().Set(tops);
}

void Scws::serialize(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  Nan::HandleScope scope;

  Scws *scwsObj = ObjectWrap::Unwrap<Scws>(info.Holder());
  scws_t scws = scwsObj->scws;
  scws_res_t res, cur;
  Handle<Value> arg0 = info[0];
  String::Utf8Value txt(arg0);
  int txtLen = strlen(*txt);
  scws_send_text(scws, *txt, txtLen);
  Local<Array> words = Nan::New<Array>();
  int index = 0;
  while ((res = cur = scws_get_result(scws)))
  {
    while (cur != NULL)
    {
      Local<Object> word = Nan::New<Object>();
      word->Set(Nan::New<String>("word").ToLocalChecked(), Nan::New<String>(*txt + cur->off, cur->len).ToLocalChecked());
      if(cur->attr[1] == '\0'){
        word->Set(Nan::New<String>("attr").ToLocalChecked(), Nan::New<String>(cur->attr, 1).ToLocalChecked());
      }else{
        word->Set(Nan::New<String>("attr").ToLocalChecked(), Nan::New<String>(cur->attr, 2).ToLocalChecked());
      }
      word->Set(Nan::New<String>("idf").ToLocalChecked(), Nan::New<Number>(cur->idf));
      words->Set(index, word);
      //printf("WORD: %.*s/%s (IDF = %4.2f)\n", cur->len, text+cur->off, cur->attr, cur->idf);
      cur = cur->next;
      index ++;

    }
    scws_free_result(res);
    scws_free_result(cur);
  }
  info.GetReturnValue().Set(words);
}

void Scws::Init(v8::Local<v8::Object> exports) {
  Isolate* isolate = exports->GetIsolate();
  Nan::HandleScope scope;
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Scws").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "setDict", setDict);
  Nan::SetPrototypeMethod(tpl, "serialize", serialize);
  Nan::SetPrototypeMethod(tpl, "setRule", setRule);
  Nan::SetPrototypeMethod(tpl, "setMulti", setMulti);
  Nan::SetPrototypeMethod(tpl, "addDict", addDict);
  Nan::SetPrototypeMethod(tpl, "segment", segment);

  constructor.Reset(isolate, tpl->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "Scws"),
               tpl->GetFunction());
}

void Scws::New(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  Isolate* isolate = info.GetIsolate();
  if (info.IsConstructCall()) {
    Scws* obj = new Scws();
    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } else {
    const int argc = 1;
    Local<Value> argv[argc] = { info[0] };
    Local<Context> context = isolate->GetCurrentContext();
    Local<Function> cons = Local<Function>::New(isolate, constructor);
    Local<Object> result =
        cons->NewInstance(context, argc, argv).ToLocalChecked();
    info.GetReturnValue().Set(result);
  }
}
