#include "ProjectionHelper.h"

// ProgressPromise Implementation
ProgressPromise ProgressPromise::Create(Napi::Env env, Napi::Promise::Deferred deferred) {
    return ProgressPromise(env, deferred);
}

ProgressPromise::ProgressPromise(Napi::Env env, Napi::Promise::Deferred deferred) {
    m_progressTsfn = std::make_shared<Napi::ThreadSafeFunction*>(nullptr);
    m_object = Napi::Object::New(env);
    
    // Store the real promise and progress callback storage
    m_object.Set("_promise", deferred.Promise());
    m_object.Set("_progressTsfn", Napi::External<std::shared_ptr<Napi::ThreadSafeFunction*>>::New(env, 
        new std::shared_ptr<Napi::ThreadSafeFunction*>(m_progressTsfn),
        [](Napi::Env env, std::shared_ptr<Napi::ThreadSafeFunction*>* data) { delete data; }));
    
    // Add .then() method that delegates to the underlying promise
    m_object.Set("then", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
        auto env = info.Env();
        auto promise = info.This().As<Napi::Object>().Get("_promise");
        auto method = promise.As<Napi::Object>().Get("then").As<Napi::Function>();
        std::vector<napi_value> args;
        for (size_t i = 0; i < info.Length(); i++) args.push_back(info[i]);
        return method.Call(promise, args);
    }));
    
    // Add .catch() method that delegates to the underlying promise
    m_object.Set("catch", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
        auto env = info.Env();
        auto promise = info.This().As<Napi::Object>().Get("_promise");
        auto method = promise.As<Napi::Object>().Get("catch").As<Napi::Function>();
        std::vector<napi_value> args;
        for (size_t i = 0; i < info.Length(); i++) args.push_back(info[i]);
        return method.Call(promise, args);
    }));
    
    // Add .progress() method
    m_object.Set("progress", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
        auto env = info.Env();
        auto self = info.This().As<Napi::Object>();
        if (info.Length() < 1 || !info[0].IsFunction()) {
            Napi::TypeError::New(env, "Expected callback function").ThrowAsJavaScriptException();
            return env.Undefined();
        }
        auto progressTsfnExternal = self.Get("_progressTsfn").As<Napi::External<std::shared_ptr<Napi::ThreadSafeFunction*>>>();
        **progressTsfnExternal.Data() = new Napi::ThreadSafeFunction(
            Napi::ThreadSafeFunction::New(env, info[0].As<Napi::Function>(), "GenerateResponseProgress", 0, 1)
        );
        return self;
    }));
}

Napi::Object ProgressPromise::GetPromiseObject() const {
    return m_object;
}

std::shared_ptr<Napi::ThreadSafeFunction*> ProgressPromise::GetProgressTsfn() const {
    return m_progressTsfn;
}