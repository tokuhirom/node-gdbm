#include <gdbm.h>

#include <node.h>
#include <v8.h>
#include <assert.h>
#include <stdlib.h>

using namespace v8;
using namespace node;

Handle<Value> Method(const Arguments& args) {
  HandleScope scope;
  return scope.Close(String::New("world"));
}

static Persistent<String> open_symbol;
static Persistent<String> close_symbol;


class GDBM : public node::ObjectWrap {
public:
    static void Initialize(v8::Handle<v8::Object> target) {
        HandleScope scope;

        Local<FunctionTemplate> t = FunctionTemplate::New(New);
        t->InstanceTemplate()->SetInternalFieldCount(1);

        open_symbol  = NODE_PSYMBOL("open");
        close_symbol = NODE_PSYMBOL("close");

        // NODE_SET_PROTOTYPE_METHOD(t, "open", open);
        // NODE_SET_PROTOTYPE_METHOD(t, "close", Close);

        target->Set(String::NewSymbol("GDBM"), t->GetFunction());
    }

protected:
    static Handle<Value> New (const Arguments& args) {
        HandleScope scope;

        GDBM *gdbm = new GDBM();
        gdbm->Wrap(args.This());

        return args.This();
    }
};

extern "C" void
init(Handle<Object> target) {
    HandleScope scope;
    NODE_SET_METHOD(target, "gdbm", Method);
    GDBM::Initialize(target);
}

