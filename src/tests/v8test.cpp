#include "../test.h"

#if HAS_V8

#include "libplatform/libplatform.h"
#include "v8.h"

using namespace v8;

// Note: V8 does not support custom allocator for all allocation.
// Use Isolate::GetHeapStatistics() for memory statistics.
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
    V8ParseResult() : heapUsage() {}
    ~V8ParseResult() {
        root.Reset();

#if USE_MEMORYSTAT
        Memory::Instance().FreeStat(heapUsage);
#endif
    }
    Persistent<Value> root;
    size_t heapUsage;
};

class V8StringResult : public StringResultBase {
public:
    V8StringResult() : heapUsage() {}
    ~V8StringResult() {
#if USE_MEMORYSTAT
        Memory::Instance().FreeStat(heapUsage);
#endif        
    }
    virtual const char* c_str() const { return s.c_str(); }

    std::string s;
    size_t heapUsage;
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

    virtual void SetUp() const {
        Isolate::CreateParams create_params;
        create_params.array_buffer_allocator = &allocator;
        isolate = Isolate::New(create_params);
        Isolate::Scope isolate_scope(isolate);
        HandleScope handle_scope(isolate);
        context_.Reset(isolate, Context::New(isolate));
    }

    virtual void TearDown() const {
        context_.Reset();
        isolate->Dispose();
    }

#if TEST_INFO
    virtual const char* GetName() const { return "V8 (C++)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        HandleScope handle_scope(isolate);
        Local<Context> context = Local<Context>::New(isolate, context_);
        Context::Scope context_scope(context);
        Isolate::Scope isolate_scope(isolate);

#if USE_MEMORYSTAT
        HeapStatistics hs;
        isolate->GetHeapStatistics(&hs);
#endif
        
        Local<Object> global = context->Global();
        Local<Object> JSON = Local<Object>::Cast(global->Get(context, String::NewFromUtf8(isolate, "JSON")).ToLocalChecked());
        Local<Function> parse = Local<Function>::Cast(JSON->Get(context, String::NewFromUtf8(isolate, "parse")).ToLocalChecked());
        Local<Value> argv[1] = { String::NewFromUtf8(isolate, json, NewStringType::kNormal, length).ToLocalChecked() };
        MaybeLocal<Value> result = parse->Call(context, global, 1, argv);
        if (!result.IsEmpty()) {
            V8ParseResult* pr = new V8ParseResult;
            pr->root.Reset(isolate, result.ToLocalChecked());

#if USE_MEMORYSTAT
            HeapStatistics hs2;
            isolate->GetHeapStatistics(&hs2);
            pr->heapUsage = hs2.used_heap_size() - hs.used_heap_size();
            Memory::Instance().MallocStat(pr->heapUsage);
#endif
            return pr;
        }
        else
            return 0;
    }

#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const V8ParseResult* pr = static_cast<const V8ParseResult*>(parseResult);
        HandleScope handle_scope(isolate);
        Local<Context> context = Local<Context>::New(isolate, context_);
        Context::Scope context_scope(context);
        Isolate::Scope isolate_scope(isolate);
        
#if USE_MEMORYSTAT
        HeapStatistics hs;
        isolate->GetHeapStatistics(&hs);
#endif

        Local<Object> global = context->Global();
        Local<Object> JSON = Local<Object>::Cast(global->Get(context, String::NewFromUtf8(isolate, "JSON")).ToLocalChecked());
        Local<Function> parse = Local<Function>::Cast(JSON->Get(context, String::NewFromUtf8(isolate, "stringify")).ToLocalChecked());
        Local<Value> argv[1] = { Local<Value>::New(isolate, pr->root) };
        MaybeLocal<Value> result = parse->Call(context, global, 1, argv);
        if (!result.IsEmpty()) {
            V8StringResult* sr = new V8StringResult;

#if USE_MEMORYSTAT
            HeapStatistics hs2;
            isolate->GetHeapStatistics(&hs2);
            sr->heapUsage = hs2.used_heap_size() - hs.used_heap_size();
            Memory::Instance().MallocStat(sr->heapUsage);
#endif

            String::Utf8Value u(result.ToLocalChecked());
            sr->s = std::string(*u, u.length());
            return sr;
        }
        else
            return 0;
    }
#endif

#if TEST_PRETTIFY
    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        const V8ParseResult* pr = static_cast<const V8ParseResult*>(parseResult);
        HandleScope handle_scope(isolate);
        Local<Context> context = Local<Context>::New(isolate, context_);
        Context::Scope context_scope(context);
        Isolate::Scope isolate_scope(isolate);
        
#if USE_MEMORYSTAT
        HeapStatistics hs;
        isolate->GetHeapStatistics(&hs);
#endif

        Local<Object> global = context->Global();
        Local<Object> JSON = Local<Object>::Cast(global->Get(context, String::NewFromUtf8(isolate, "JSON")).ToLocalChecked());
        Local<Function> parse = Local<Function>::Cast(JSON->Get(context, String::NewFromUtf8(isolate, "stringify")).ToLocalChecked());
        Local<Value> argv[3] = { Local<Value>::New(isolate, pr->root), Null(isolate), Integer::New(isolate, 4) };
        MaybeLocal<Value> result = parse->Call(context, global, 3, argv);
        if (!result.IsEmpty()) {
            V8StringResult* sr = new V8StringResult;

#if USE_MEMORYSTAT
            HeapStatistics hs2;
            isolate->GetHeapStatistics(&hs2);
            sr->heapUsage = hs2.used_heap_size() - hs.used_heap_size();
            Memory::Instance().MallocStat(sr->heapUsage);
#endif

            String::Utf8Value u(result.ToLocalChecked());
            sr->s = std::string(*u, u.length());
            return sr;
        }
        else
            return 0;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        HandleScope handle_scope(isolate);
        Local<Context> context = Local<Context>::New(isolate, context_);
        Context::Scope context_scope(context);
        Isolate::Scope isolate_scope(isolate);
        const V8ParseResult* pr = static_cast<const V8ParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(*stat, Local<Value>::New(isolate, pr->root));
        return true;
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        HandleScope handle_scope(isolate);
        Local<Context> context = Local<Context>::New(isolate, context_);
        Context::Scope context_scope(context);
        Isolate::Scope isolate_scope(isolate);
        
        Local<Object> global = context->Global();
        Local<Object> JSON = Local<Object>::Cast(global->Get(context, String::NewFromUtf8(isolate, "JSON")).ToLocalChecked());
        Local<Function> parse = Local<Function>::Cast(JSON->Get(context, String::NewFromUtf8(isolate, "parse")).ToLocalChecked());
        Local<Value> argv[1] = { String::NewFromUtf8(isolate, json, NewStringType::kNormal).ToLocalChecked() };
        MaybeLocal<Value> result = parse->Call(context, global, 1, argv);
        if (!result.IsEmpty()) {
            Local<Array> a = Local<Array>::Cast(result.ToLocalChecked());
            Local<Number> n = Local<Number>::Cast(a->Get(0));
            *d = n->Value();
            return true;
        }
        else
            return false;
    }

    virtual bool ParseString(const char* json, std::string& s) const {
        HandleScope handle_scope(isolate);
        Local<Context> context = Local<Context>::New(isolate, context_);
        Context::Scope context_scope(context);
        Isolate::Scope isolate_scope(isolate);
        
        Local<Object> global = context->Global();
        Local<Object> JSON = Local<Object>::Cast(global->Get(context, String::NewFromUtf8(isolate, "JSON")).ToLocalChecked());
        Local<Function> parse = Local<Function>::Cast(JSON->Get(context, String::NewFromUtf8(isolate, "parse")).ToLocalChecked());
        Local<Value> argv[1] = { String::NewFromUtf8(isolate, json, NewStringType::kNormal).ToLocalChecked() };
        MaybeLocal<Value> result = parse->Call(context, global, 1, argv);
        if (!result.IsEmpty()) {
            Local<Array> a = Local<Array>::Cast(result.ToLocalChecked());
            String::Utf8Value u(Local<String>::Cast(a->Get(0)));
            s = std::string(*u, u.length());
            return true;
        }
        else
            return false;
    }
#endif

    Platform* platform;
    mutable ArrayBufferAllocator allocator;
    mutable Isolate* isolate;
    mutable Persistent<Context> context_;
};

REGISTER_TEST(V8Test);

#endif
