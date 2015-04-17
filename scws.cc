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

NAN_METHOD(Scws::addDict) {
  NanScope();
  Scws *scws = ObjectWrap::Unwrap<Scws>(args.This());
  Handle<Value> arg0 = args[0];
  String::Utf8Value dict(arg0);
  if (strstr(*dict, "xdb")) {
    scws_add_dict(scws->scws, *dict, SCWS_XDICT_MEM);
  } else {
    scws_add_dict(scws->scws, *dict, SCWS_XDICT_TXT);
  }
  NanReturnUndefined();
}

NAN_METHOD(Scws::setDict) {
  NanScope();
  Scws *scws = ObjectWrap::Unwrap<Scws>(args.This());
  Handle<Value> arg0 = args[0];
  String::Utf8Value dict(arg0);
  if (strstr(*dict, "xdb")) {
    scws_set_dict(scws->scws, *dict, SCWS_XDICT_MEM);
  } else {
    scws_set_dict(scws->scws, *dict, SCWS_XDICT_TXT);
  }
  NanReturnUndefined();
}

NAN_METHOD(Scws::setMulti) {
  NanScope();
  Scws *scws = ObjectWrap::Unwrap<Scws>(args.This());
  int multi = args[0]->NumberValue();
  if (multi == 1) {
    scws_set_multi(scws->scws, SCWS_MULTI_SHORT);
  } else {
    scws_set_multi(scws->scws, 0);
  }
  NanReturnUndefined();
}

NAN_METHOD(Scws::setRule) {
  NanScope();
  Scws *scws = ObjectWrap::Unwrap<Scws>(args.This());
  Handle<Value> arg0 = args[0];
  String::Utf8Value rule(arg0);
  scws_set_rule(scws->scws, *rule);
  NanReturnUndefined();
}

NAN_METHOD(Scws::segment) {
  NanScope();
    Scws *scwsObj = ObjectWrap::Unwrap<Scws>(args.This());
    scws_top_t res, cur;
    Handle<Value> arg0 = args[0];
    String::Utf8Value txt(arg0);
    int txtLen = strlen(*txt);
    int limit = txtLen;
    if (args[1]->IsNumber()) {
      limit = args[1]->NumberValue();
    }
    scws_t scws = scwsObj->scws;
    scws_send_text(scws, *txt, txtLen);
    Local<Array> tops = NanNew<Array>();
    int index = 0;

    cur = res = scws_get_tops(scws, limit, NULL);
    while(cur != NULL){
        Local<Object> objWord = NanNew<Object>();
        objWord->Set(NanNew<String>("word"), NanNew<String>(cur->word));
        objWord->Set(NanNew<String>("weight"), NanNew<Number>(cur->weight));
        objWord->Set(NanNew<String>("times"), NanNew<Integer>(cur->times));
        if(cur->attr[1] == '\0'){
            objWord->Set(NanNew<String>("attr"), NanNew<String>(cur->attr, 1));
        }else{
            objWord->Set(NanNew<String>("attr"), NanNew<String>(cur->attr, 2));
        }
        cur = cur->next;
        tops->Set(index, objWord);
        index += 1;
    }
    scws_free_tops(res);
    scws_free_tops(cur);
    NanReturnValue(tops);
}

NAN_METHOD(Scws::serialize) {
  NanScope();

  Scws *scwsObj = ObjectWrap::Unwrap<Scws>(args.This());
  scws_t scws = scwsObj->scws;
  scws_res_t res, cur;
  Handle<Value> arg0 = args[0];
  String::Utf8Value txt(arg0);
  int txtLen = strlen(*txt);
  scws_send_text(scws, *txt, txtLen);
  Local<Array> words = NanNew<Array>();
  int index = 0;
  while ((res = cur = scws_get_result(scws)))
  {
    while (cur != NULL)
    {
      Local<Object> word = NanNew<Object>();
      word->Set(NanNew<String>("word"), NanNew<String>(*txt + cur->off, cur->len));
      if(cur->attr[1] == '\0'){
        word->Set(NanNew<String>("attr"), NanNew<String>(cur->attr, 1));
      }else{
        word->Set(NanNew<String>("attr"), NanNew<String>(cur->attr, 2));
      }
      word->Set(NanNew<String>("idf"), NanNew<Number>(cur->idf));
      words->Set(index, word);
      //printf("WORD: %.*s/%s (IDF = %4.2f)\n", cur->len, text+cur->off, cur->attr, cur->idf);
      cur = cur->next;
      index ++;

    }
    scws_free_result(res);
    scws_free_result(cur);
  }
  NanReturnValue(words);
}

void Scws::Init(Handle<Object> exports) {
  NanScope();
  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);
  tpl->SetClassName(NanNew("Scws"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(tpl, "setDict", setDict);
  NODE_SET_PROTOTYPE_METHOD(tpl, "serialize", serialize);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setRule", setRule);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setMulti", setMulti);
  NODE_SET_PROTOTYPE_METHOD(tpl, "addDict", addDict);
  NODE_SET_PROTOTYPE_METHOD(tpl, "segment", segment);

  NanAssignPersistent(constructor, tpl->GetFunction());
  exports->Set(NanNew("Scws"), tpl->GetFunction());
}

NAN_METHOD(Scws::New) {
  NanScope();
  Scws* obj = new Scws();
  obj->Wrap(args.This());
  NanReturnValue(args.This());
}
