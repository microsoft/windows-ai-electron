#pragma once

#include <napi.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <memory>

using namespace winrt;
using namespace Windows::ApplicationModel;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

// Forward declarations
class MyLimitedAccessFeatureRequestResult;

// MyLimitedAccessFeatures class
class MyLimitedAccessFeatures : public Napi::ObjectWrap<MyLimitedAccessFeatures> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::FunctionReference constructor;
    static bool s_featureUnlocked;

    MyLimitedAccessFeatures(const Napi::CallbackInfo& info);

    // Static methods
    static Napi::Value TryUnlockFeature(const Napi::CallbackInfo& info);
    static bool IsFeatureUnlocked();

private:
    // No instance members needed for static-only class
};

// MyLimitedAccessFeatureRequestResult class
class MyLimitedAccessFeatureRequestResult : public Napi::ObjectWrap<MyLimitedAccessFeatureRequestResult> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::FunctionReference constructor;

    MyLimitedAccessFeatureRequestResult(const Napi::CallbackInfo& info);

    // Properties
    Napi::Value GetFeatureId(const Napi::CallbackInfo& info);
    Napi::Value GetStatus(const Napi::CallbackInfo& info);
    Napi::Value GetEstimatedRemovalDate(const Napi::CallbackInfo& info);

private:
    std::optional<LimitedAccessFeatureRequestResult> m_result;
};