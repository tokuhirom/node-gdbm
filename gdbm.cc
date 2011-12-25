#include <gdbm.h>

#include <node.h>
#include <v8.h>
#include <assert.h>
#include <stdlib.h>
#include <cstdio>

#include "version.h"

using namespace v8;
using namespace node;

#define THROW(msg) return ThrowException(Exception::Error(String::New(msg)));

static Persistent<String> open_symbol;
static Persistent<String> close_symbol;
static Persistent<String> store_symbol;
static Persistent<String> fetch_symbol;

#define CHECK_DB() \
    if (!Unwrap<GDBM>(args.This())->db_) { \
        return ThrowException(Exception::Error(String::New("DB has not been opened"))); \
    }

class GDBM : public node::ObjectWrap {
private:
    GDBM_FILE db_;

public:
    GDBM() {
        db_ = NULL;
    }

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
        NODE_SET_PROTOTYPE_METHOD(t, "delete", Delete);
        NODE_SET_PROTOTYPE_METHOD(t, "fdesc", Fdesc);
        NODE_SET_PROTOTYPE_METHOD(t, "reorganize", Reorganize);
        NODE_SET_PROTOTYPE_METHOD(t, "firstkey", FirstKey);
        NODE_SET_PROTOTYPE_METHOD(t, "nextkey", NextKey);

        target->Set(String::NewSymbol("GDBM"), t->GetFunction());
        NODE_SET_METHOD(t, "strerror", StrError);
    }

    bool Open (char* fname, int block_size, int flags, int mode) {
        db_ = gdbm_open(fname, block_size, flags, mode, NULL);
        return !!db_;
    }
	static Handle<Value> Open(const v8::Arguments& args) {
		HandleScope scope;
        int block_size = 0;
        if (args.Length()<1) {
            THROW("First parameter must be a string");
        }
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
        CHECK_DB();
        Unwrap<GDBM>(args.This())->Sync();
        return Undefined();
    }

    int Store(char *key_str, int key_len, char * content_str, int content_len, int flag) {
        datum key = {key_str, key_len};
        datum content = {content_str, content_len};
        // int gdbm_store (GDBM_FILE dbf, datum key, datum content, int flag);
        assert(db_);
        gdbm_store(db_, key, content, flag);
        return 0;
    }
	static Handle<Value> Store(const v8::Arguments& args) {
        HandleScope scope;
        CHECK_DB();
        if (args.Length()<2) {
            THROW("Usage: db.store(key, val[, flags])");
        }
        int flag = GDBM_REPLACE;
        if (args.Length() > 2) {
            flag = args[2]->Int32Value();
        }
        v8::Handle<v8::String> key = args[0]->ToString();
        v8::Handle<v8::String> val = args[1]->ToString();
        int ret = Unwrap<GDBM>(args.This())->Store(
            *String::Utf8Value(key),
            key->Utf8Length(),
            *String::Utf8Value(val),
            val->Utf8Length(),
            flag
        );
        return scope.Close(v8::Int32::New(ret));
    }

    // datum gdbm_fetch (GDBM_FILE dbf, datum key);
    datum Fetch(char * str, int len) {
        datum key = {str, len};
        return gdbm_fetch(db_, key);
    }
    static Handle<Value> Fetch(const v8::Arguments& args) {
        HandleScope scope;
        CHECK_DB();
        if (args.Length()<1) {
            THROW("Usage: db.fetch(key)");
        }
        v8::Handle<v8::String> key = args[0]->ToString();
        datum val = Unwrap<GDBM>(args.This())->Fetch(
            *String::Utf8Value(key),
            key->Utf8Length()
        );
        if (val.dptr) {
            Handle <String>str = v8::String::New(val.dptr, val.dsize);
            free(val.dptr);
            return scope.Close(str);
        } else {
            return scope.Close(Undefined());
        }
    }

    bool Exists(char * str, int len) {
        datum key = {str, len};
        return gdbm_exists(db_, key);
    }
    static Handle<Value> Exists(const v8::Arguments& args) {
        HandleScope scope;
        CHECK_DB();
        if (args.Length()<1) {
            THROW("Usage: db.exists(key)");
        }
        v8::Handle<v8::String> key = args[0]->ToString();
        bool ret = Unwrap<GDBM>(args.This())->Exists(
            *String::Utf8Value(key),
            key->Utf8Length()
        );
        return scope.Close(v8::Boolean::New(ret));
    }

    int Reorganize() {
        assert(db_);
        return gdbm_reorganize(db_);
    }
    static Handle<Value> Reorganize(const v8::Arguments& args) {
        HandleScope scope;
        CHECK_DB();
        Unwrap<GDBM>(args.This())->Reorganize();
        return Undefined();
    }

    int Fdesc() {
        return gdbm_fdesc(db_);
    }
    static Handle<Value> Fdesc(const v8::Arguments& args) {
        HandleScope scope;
        CHECK_DB();
        int ret = Unwrap<GDBM>(args.This())->Fdesc();
        return scope.Close(v8::Int32::New(ret));
    }

    bool Delete(char * str, int len) {
        datum key = {str, len};
        return gdbm_delete(db_, key)==0;
    }
    static Handle<Value> Delete(const v8::Arguments& args) {
        HandleScope scope;
        CHECK_DB();
        assert(args.Length() >= 1);
        v8::Handle<v8::String> key = args[0]->ToString();
        bool ret = Unwrap<GDBM>(args.This())->Delete(
            *String::Utf8Value(key),
            key->Utf8Length()
        );
        return scope.Close(v8::Boolean::New(ret));
    }

    datum FirstKey() {
        return gdbm_firstkey(db_);
    }
    static Handle<Value> FirstKey(const v8::Arguments& args) {
        HandleScope scope;

        CHECK_DB();
        datum key = Unwrap<GDBM>(args.This())->FirstKey();
        if (key.dptr) {
            Local<String> ret = String::New(key.dptr, key.dsize);
            free(key.dptr);
            return scope.Close(ret);
        } else {
            return scope.Close(Undefined());
        }
    }

    datum NextKey(char * str, int len) {
        datum key = {str, len};
        return gdbm_nextkey(db_, key);
    }
    static Handle<Value> NextKey(const v8::Arguments& args) {
        HandleScope scope;
        CHECK_DB();
        if (args.Length()<1) {
            THROW("Usage: db.nextkey(key)");
        }

        v8::Handle<v8::String> ckey = args[0]->ToString();
        datum nkey = Unwrap<GDBM>(args.This())->NextKey(
            *String::Utf8Value(ckey),
            ckey->Utf8Length()
        );
        if (nkey.dptr) {
            Local<Value> ret = String::New(nkey.dptr, nkey.dsize);
            free(nkey.dptr);
            return scope.Close(ret);
        } else {
            return scope.Close(Undefined());
        }
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

    NODE_DEFINE_CONSTANT(target, GDBM_READER);
    NODE_DEFINE_CONSTANT(target, GDBM_WRITER);
    NODE_DEFINE_CONSTANT(target, GDBM_WRCREAT);
    NODE_DEFINE_CONSTANT(target, GDBM_NEWDB);
    NODE_DEFINE_CONSTANT(target, GDBM_SYNC);
    NODE_DEFINE_CONSTANT(target, GDBM_NOLOCK);
#ifdef GDBM_NOMMAP
    NODE_DEFINE_CONSTANT(target, GDBM_NOMMAP);
#endif

    target->Set(v8::String::NewSymbol("gdbm_version"),
        v8::String::New(gdbm_version));
    target->Set(v8::String::NewSymbol("version"),
        v8::String::New(NODE_GDBM_VERSION));

    GDBM::Initialize(target);
}

