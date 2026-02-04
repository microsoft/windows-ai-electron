#include "LimitedAccessFeature.h"
#include <winrt/Windows.ApplicationModel.h>

using namespace winrt;
using namespace Windows::ApplicationModel;

// Static member definitions
Napi::FunctionReference MyLimitedAccessFeatures::constructor;
Napi::FunctionReference MyLimitedAccessFeatureRequestResult::constructor;

// MyLimitedAccessFeatures Implementation
Napi::Object MyLimitedAccessFeatures::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "LimitedAccessFeatures", {
        StaticMethod("TryUnlockFeature", &MyLimitedAccessFeatures::TryUnlockFeature)
    });

    constructor = Napi::Persistent(func);
    exports.Set("LimitedAccessFeatures", func);
    return exports;
}

MyLimitedAccessFeatures::MyLimitedAccessFeatures(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyLimitedAccessFeatures>(info) {
    // This is a static-only class, so constructor doesn't need to do anything special
}

Napi::Value MyLimitedAccessFeatures::TryUnlockFeature(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 3 || !info[0].IsString() || !info[1].IsString() || !info[2].IsString()) {
        Napi::TypeError::New(env, "TryUnlockFeature requires three string parameters (featureId, token, developerSignature)").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    try {
        std::string featureIdStr = info[0].As<Napi::String>().Utf8Value();
        std::string tokenStr = info[1].As<Napi::String>().Utf8Value();
        std::string devSigStr = info[2].As<Napi::String>().Utf8Value();
        
        winrt::hstring featureId = winrt::to_hstring(featureIdStr);
        winrt::hstring token = winrt::to_hstring(tokenStr);
        winrt::hstring developerSignature = winrt::to_hstring(devSigStr);
        
        // Call the TryUnlockFeature(String, String, String) overload
        LimitedAccessFeatureRequestResult result = LimitedAccessFeatures::TryUnlockFeature(featureId, token, developerSignature);
        
        // Create wrapper for the result
        auto external = Napi::External<LimitedAccessFeatureRequestResult>::New(env, &result);
        auto resultWrapper = MyLimitedAccessFeatureRequestResult::constructor.New({ external });
        return resultWrapper;
        
    } catch (const winrt::hresult_error& ex) {
        std::string errorMsg = "WinRT error in TryUnlockFeature: " + winrt::to_string(ex.message());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (const std::exception& ex) {
        std::string errorMsg = "Error in TryUnlockFeature: " + std::string(ex.what());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (...) {
        Napi::Error::New(env, "Unknown error occurred in TryUnlockFeature").ThrowAsJavaScriptException();
        return env.Null();
    }
}

// MyLimitedAccessFeatureRequestResult Implementation
Napi::Object MyLimitedAccessFeatureRequestResult::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "LimitedAccessFeatureRequestResult", {
        InstanceAccessor("FeatureId", &MyLimitedAccessFeatureRequestResult::GetFeatureId, nullptr),
        InstanceAccessor("Status", &MyLimitedAccessFeatureRequestResult::GetStatus, nullptr),
        InstanceAccessor("EstimatedRemovalDate", &MyLimitedAccessFeatureRequestResult::GetEstimatedRemovalDate, nullptr)
    });

    constructor = Napi::Persistent(func);
    exports.Set("LimitedAccessFeatureRequestResult", func);
    return exports;
}

MyLimitedAccessFeatureRequestResult::MyLimitedAccessFeatureRequestResult(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyLimitedAccessFeatureRequestResult>(info) {
    if (info.Length() == 1 && info[0].IsExternal()) {
        auto external = info[0].As<Napi::External<LimitedAccessFeatureRequestResult>>();
        m_result = *external.Data();
    }
    // If no external data, m_result remains empty (std::nullopt)
}

Napi::Value MyLimitedAccessFeatureRequestResult::GetFeatureId(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        if (!m_result.has_value()) {
            Napi::Error::New(env, "No result data available").ThrowAsJavaScriptException();
            return env.Null();
        }
        auto featureId = m_result->FeatureId();
        return Napi::String::New(env, winrt::to_string(featureId));
    } catch (const winrt::hresult_error& ex) {
        std::string errorMsg = "WinRT error getting feature ID: " + winrt::to_string(ex.message());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (const std::exception& ex) {
        std::string errorMsg = "Error getting feature ID: " + std::string(ex.what());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (...) {
        Napi::Error::New(env, "Unknown error getting feature ID").ThrowAsJavaScriptException();
        return env.Null();
    }
}

Napi::Value MyLimitedAccessFeatureRequestResult::GetStatus(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        if (!m_result.has_value()) {
            Napi::Error::New(env, "No result data available").ThrowAsJavaScriptException();
            return env.Null();
        }
        auto status = m_result->Status();
        return Napi::Number::New(env, static_cast<int>(status));
    } catch (const winrt::hresult_error& ex) {
        std::string errorMsg = "WinRT error getting status: " + winrt::to_string(ex.message());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (const std::exception& ex) {
        std::string errorMsg = "Error getting status: " + std::string(ex.what());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (...) {
        Napi::Error::New(env, "Unknown error getting status").ThrowAsJavaScriptException();
        return env.Null();
    }
}

Napi::Value MyLimitedAccessFeatureRequestResult::GetEstimatedRemovalDate(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        if (!m_result.has_value()) {
            Napi::Error::New(env, "No result data available").ThrowAsJavaScriptException();
            return env.Null();
        }
        auto estimatedRemovalDate = m_result->EstimatedRemovalDate();
        if (!estimatedRemovalDate) {
            return env.Null();
        }
        
        // Convert DateTime to JavaScript Date
        // Windows DateTime is 100-nanosecond intervals since January 1, 1601 UTC
        // JavaScript Date expects milliseconds since January 1, 1970 UTC
        
        // Convert to time_t (seconds since 1970)
        auto timePoint = winrt::clock::to_time_t(estimatedRemovalDate.Value());
        
        // Convert to milliseconds for JavaScript
        double jsTime = static_cast<double>(timePoint) * 1000.0;
        
        // Create JavaScript Date object
        auto jsDate = env.Global().Get("Date").As<Napi::Function>();
        return jsDate.New({ Napi::Number::New(env, jsTime) });
        
    } catch (const winrt::hresult_error& ex) {
        std::string errorMsg = "WinRT error getting estimated removal date: " + winrt::to_string(ex.message());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (const std::exception& ex) {
        std::string errorMsg = "Error getting estimated removal date: " + std::string(ex.what());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (...) {
        Napi::Error::New(env, "Unknown error getting estimated removal date").ThrowAsJavaScriptException();
        return env.Null();
    }
}