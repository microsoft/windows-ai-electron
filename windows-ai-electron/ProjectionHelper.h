#pragma once

#include <napi.h>
#include <memory>

// Helper class for Promise-like object with progress support
class ProgressPromise {
private:
    Napi::Object m_object;
    std::shared_ptr<Napi::ThreadSafeFunction*> m_progressTsfn;

public:
    static ProgressPromise Create(Napi::Env env, Napi::Promise::Deferred deferred);
    ProgressPromise(Napi::Env env, Napi::Promise::Deferred deferred);
    
    Napi::Object GetPromiseObject() const;
    std::shared_ptr<Napi::ThreadSafeFunction*> GetProgressTsfn() const;
};