#include <gdbm.h>

#include <node.h>
#include <v8.h>
#include <assert.h>
#include <stdlib.h>
#include <cstdio>

using namespace v8;
using namespace node;

Handle<Value> Method(const Arguments& args) {
  HandleScope scope;
  return scope.Close(String::New("world"));
}


static Persistent<String> open_symbol;
static Persistent<String> close_symbol;
static Persistent<String> store_symbol;
static Persistent<String> fetch_symbol;


class GDBM : public node::ObjectWrap {
private:
    GDBM_FILE db_;

public:
    static void Initialize(v8::Handle<v8::Object> target) {
        HandleScope scope;

        Local<FunctionTemplate> t = FunctionTemplate::New(New);
        t->InstanceTemplate()->SetInternalFieldCount(1);

        open_symbol  = NODE_PSYMBOL("open");
        close_symbol = NODE_PSYMBOL("close");
        store_symbol = NODE_PSYMBOL("store");
        fetch_symbol = NODE_PSYMBOL("fetch");

        NODE_SET_PROTOTYPE_METHOD(t, "open", Open);
        NODE_SET_PROTOTYPE_METHOD(t, "close", Close);
        NODE_SET_PROTOTYPE_METHOD(t, "sync", Sync);
        NODE_SET_PROTOTYPE_METHOD(t, "errno", Errno);
        NODE_SET_PROTOTYPE_METHOD(t, "strerror", StrError);
        NODE_SET_PROTOTYPE_METHOD(t, "store", Store);
        NODE_SET_PROTOTYPE_METHOD(t, "fetch", Fetch);
        NODE_SET_PROTOTYPE_METHOD(t, "exists", Exists);

        target->Set(String::NewSymbol("GDBM"), t->GetFunction());
        NODE_SET_METHOD(t, "strerror", StrError);
    }

    bool Open (const char* fname, int block_size, int flags, int mode) {
        fprintf(stderr, "args::: %s, %d, %d, %d\n", fname, block_size, flags, mode);
        db_ = gdbm_open(fname, block_size, flags, mode, NULL);
        return !!db_;
    }
	static Handle<Value> Open(const v8::Arguments& args) {
		HandleScope scope;
        int block_size = 0;
        assert(args.Length() > 1);
        if (args.Length()>=2) {
            block_size = args[1]->Int32Value();
        }
        int flags = GDBM_READER;
        if (args.Length()>=3) {
            flags = args[2]->Int32Value();
        }
        int mode = 0666;
        if (args.Length()>=4) {
            mode = args[3]->Int32Value();
        }
		bool ret = Unwrap<GDBM>(args.This())->Open(
            *String::Utf8Value(args[0]),
            block_size, flags, mode
        );
		return v8::Boolean::New(ret);
	}

    static Handle<Value> StrError(const Arguments& args) {
        HandleScope scope;
        return scope.Close(String::New(gdbm_strerror(gdbm_errno)));
    }

    void Close() {
        if (db_) { gdbm_close(db_); }
        db_ = NULL;
    }
	static Handle<Value> Close(const v8::Arguments& args) {
        HandleScope scope;
        Unwrap<GDBM>(args.This())->Close();
        return Undefined();
    }

    void Sync() {
        assert(db_);
        gdbm_sync(db_);
    }
	static Handle<Value> Sync(const v8::Arguments& args) {
        HandleScope scope;
        Unwrap<GDBM>(args.This())->Sync();
        return Undefined();
    }

    int Store(char *key_str, int key_len, char * content_str, int content_len, int flag) {
        datum key;
        key.dptr = key_str;
        key.dsize = key_len;
        datum content;
        content.dptr = content_str;
        content.dsize = content_len;
        // int gdbm_store (GDBM_FILE dbf, datum key, datum content, int flag);
        assert(db_);
        gdbm_store(db_, key, content, flag);
        return 0;
    }
	static Handle<Value> Store(const v8::Arguments& args) {
        HandleScope scope;
        assert(args.Length() >= 2);
        int flag = GDBM_REPLACE;
        if (args.Length() > 2) {
            flag = args[2]->Int32Value();
        }
        v8::Handle<v8::String> key = args[0]->ToString();
        v8::Handle<v8::String> val = args[1]->ToString();
        int ret = Unwrap<GDBM>(args.This())->Store(
            *String::Utf8Value(key),
            key->Length(),
            *String::Utf8Value(val),
            val->Length(),
            flag
        );
        return scope.Close(v8::Int32::New(ret));
    }

    // datum gdbm_fetch (GDBM_FILE dbf, datum key);
    datum Fetch(char * str, int len) {
        datum key;
        key.dptr = str;
        key.dsize = len;
        return gdbm_fetch(db_, key);
    }
    static Handle<Value> Fetch(const v8::Arguments& args) {
        HandleScope scope;
        assert(args.Length() >= 1);
        v8::Handle<v8::String> key = args[0]->ToString();
        datum val = Unwrap<GDBM>(args.This())->Fetch(
            *String::Utf8Value(key),
            key->Length()
        );
        return scope.Close(v8::String::New(val.dptr, val.dsize));
    }

    bool Exists(char * str, int len) {
        datum key;
        key.dptr = str;
        key.dsize = len;
        return gdbm_exists(db_, key);
    }
    static Handle<Value> Exists(const v8::Arguments& args) {
        HandleScope scope;
        assert(args.Length() >= 1);
        v8::Handle<v8::String> key = args[0]->ToString();
        bool ret = Unwrap<GDBM>(args.This())->Exists(
            *String::Utf8Value(key),
            key->Length()
        );
        return scope.Close(v8::Boolean::New(ret));
    }

	static Handle<Value> Errno(const v8::Arguments& args) {
        HandleScope scope;
        return scope.Close(Int32::New(gdbm_errno));
    }

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

    NODE_DEFINE_CONSTANT(target, GDBM_READER);
    NODE_DEFINE_CONSTANT(target, GDBM_WRITER);
    NODE_DEFINE_CONSTANT(target, GDBM_WRCREAT);
    NODE_DEFINE_CONSTANT(target, GDBM_NEWDB);
    NODE_DEFINE_CONSTANT(target, GDBM_SYNC);
    NODE_DEFINE_CONSTANT(target, GDBM_NOLOCK);

    target->Set(v8::String::NewSymbol("gdbm_version"),
        v8::String::New(gdbm_version));

    GDBM::Initialize(target);
}

