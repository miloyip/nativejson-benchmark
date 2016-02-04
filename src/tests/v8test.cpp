#include "../test.h"

#if 1//HAS_V8

#include "libplatform/libplatform.h"
#include "v8.h"

using namespace v8;

class ArrayBufferAllocator : public v8::ArrayBuffer::Allocator {
public:
    virtual void* Allocate(size_t length) {
        void* data = AllocateUninitialized(length);
        return data == NULL ? data : memset(data, 0, length);
    }
    virtual void* AllocateUninitialized(size_t length) { return malloc(length); }
    virtual void Free(void* data, size_t) { free(data); }
};

static void GenStat(Stat& stat, Local<Value> v) {
    // Array must be appear earlier than Object, because array inherits from  object in V8.
    if (v->IsArray()) {
        Local<Array> a = Local<Array>::Cast(v);
        for (uint32_t i = 0; i < a->Length(); i++)
            GenStat(stat, a->Get(i));
        stat.arrayCount++;
        stat.elementCount += a->Length();
    }
    else if (v->IsObject()) {
        Local<Object> o = Local<Object>::Cast(v);
        Local<Array> p = o->GetOwnPropertyNames();
        for (uint32_t i = 0; i < p->Length(); i++) {
            Local<Value> k = p->Get(i);
            Local<String> key = Local<String>::Cast(k);
            Local<Value> value = o->Get(key);
            GenStat(stat, value);

            // Keys in V8 can be non-string, so explicitly convert the key to string.
            Local<String> keyString = key->ToString();
            stat.stringLength += keyString->Length();
        }
        stat.objectCount++;
        stat.memberCount += p->Length();
        stat.stringCount += p->Length();
    }
    else if (v->IsString()) {
        Local<String> s = Local<String>::Cast(v);
        stat.stringCount++;
        stat.stringLength += s->Length();
    }
    else if (v->IsNumber())
        stat.numberCount++;
    else if (v->IsTrue())
        stat.trueCount++;
    else if (v->IsFalse())
        stat.falseCount++;
    else if (v->IsNull())
        stat.nullCount++;
}

class V8ParseResult : public ParseResultBase {
public:
    V8ParseResult() {
        Isolate::CreateParams create_params;
        create_params.array_buffer_allocator = &allocator;
        isolate = Isolate::New(create_params);
        Isolate::Scope isolate_scope(isolate);
        HandleScope handle_scope(isolate);
        context_.Reset(isolate, Context::New(isolate));
    }

    ~V8ParseResult() {
        root.Reset();
        context_.Reset();
        isolate->Dispose();
    }

    ArrayBufferAllocator allocator;
    Isolate* isolate;
    Persistent<Context> context_;
    Persistent<Value> root;
};

class V8StringResult : public StringResultBase {
public:
    virtual const char* c_str() const { return s.c_str(); }

    std::string s;
};

class V8Test : public TestBase {
public:
    V8Test() {
        V8::InitializeICU();
        V8::InitializeExternalStartupData("");
        platform = platform::CreateDefaultPlatform();
        V8::InitializePlatform(platform);
        V8::Initialize();
    }

    ~V8Test() {
        V8::Dispose();
        V8::ShutdownPlatform();
        delete platform;
    }

#if TEST_INFO
    virtual const char* GetName() const { return "V8 (C++)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        V8ParseResult* pr = new V8ParseResult;

        {
            Isolate* isolate = pr->isolate;

            HandleScope handle_scope(isolate);
            Local<Context> context = Local<Context>::New(pr->isolate, pr->context_);
            Context::Scope context_scope(context);
            Isolate::Scope isolate_scope(isolate);

            Local<Object> global = context->Global();
            Local<Object> JSON = Local<Object>::Cast(global->Get(context, String::NewFromUtf8(isolate, "JSON")).ToLocalChecked());
            Local<Function> parse = Local<Function>::Cast(JSON->Get(context, String::NewFromUtf8(isolate, "parse")).ToLocalChecked());
            Local<Value> argv[1] = { String::NewFromUtf8(isolate, json, NewStringType::kNormal, length).ToLocalChecked() };
            MaybeLocal<Value> result = parse->Call(context, global, 1, argv);
            if (!result.IsEmpty()) {
                pr->root.Reset(isolate, result.ToLocalChecked());
                return pr;
            }
        }

        delete pr;
        return 0;
    }

#endif

// #if TEST_STRINGIFY
//     virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
//         const V8ParseResult* pr = static_cast<const V8ParseResult*>(parseResult);
//         V8StringResult* sr = new V8StringResult;
//         sr->s = toJson(pr->root);
//         return sr;
//     }
// #endif

// #if TEST_PRETTIFY
//     virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
//         const V8ParseResult* pr = static_cast<const V8ParseResult*>(parseResult);
//         V8StringResult* sr = new V8StringResult;
//         sr->s = toPrettyJson(pr->root);
//         return sr;
//     }
// #endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const V8ParseResult* pr = static_cast<const V8ParseResult*>(parseResult);
        Isolate* isolate = pr->isolate;
        HandleScope handle_scope(isolate);
        Local<Context> context = Local<Context>::New(isolate, pr->context_);
        Context::Scope context_scope(context);
        Isolate::Scope isolate_scope(isolate);
        memset(stat, 0, sizeof(Stat));
        GenStat(*stat, Local<Value>::New(isolate, pr->root));
        return true;
    }
#endif

// #if TEST_CONFORMANCE
//     virtual bool ParseDouble(const char* json, double* d) const {
//         try {
//             dynamic v = parseJson(StringPiece(json));
//             *d = v[0].getDouble();
//             return true;
//         }
//         catch (...) {
//         }
//         return false;
//     }

//     virtual bool ParseString(const char* json, std::string& s) const {
//         try {
//             dynamic v = parseJson(StringPiece(json));
//             s = v[0].getString().toStdString();
//             return true;
//         }
//         catch (...) {
//         }
//         return false;
//     }
// #endif

    Platform* platform;
};

REGISTER_TEST(V8Test);

#endif
