#pragma once

#include <napi.h>
#include <optional>
#include <memory>

#include <winrt/Windows.Foundation.h>
#include <winrt/Microsoft.Windows.AI.ContentSafety.h>

using namespace winrt;
using namespace Microsoft::Windows::AI::ContentSafety;

// Forward declarations
class MyContentFilterOptions;
class MyImageContentFilterSeverity;
class MyTextContentFilterSeverity;

// Wrapper for ContentFilterOptions
class MyContentFilterOptions : public Napi::ObjectWrap<MyContentFilterOptions> {
public:
    static Napi::FunctionReference constructor;
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    
    MyContentFilterOptions(const Napi::CallbackInfo& info);
    ContentFilterOptions GetOptions() const;

private:
    ContentFilterOptions m_options;
    
    Napi::Value GetImageMaxAllowedSeverityLevel(const Napi::CallbackInfo& info);
    void SetImageMaxAllowedSeverityLevel(const Napi::CallbackInfo& info, const Napi::Value& value);
    Napi::Value GetPromptMaxAllowedSeverityLevel(const Napi::CallbackInfo& info);
    void SetPromptMaxAllowedSeverityLevel(const Napi::CallbackInfo& info, const Napi::Value& value);
    Napi::Value GetResponseMaxAllowedSeverityLevel(const Napi::CallbackInfo& info);
    void SetResponseMaxAllowedSeverityLevel(const Napi::CallbackInfo& info, const Napi::Value& value);
};

// Wrapper for ImageContentFilterSeverity
class MyImageContentFilterSeverity : public Napi::ObjectWrap<MyImageContentFilterSeverity> {
public:
    static Napi::FunctionReference constructor;
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    
    MyImageContentFilterSeverity(const Napi::CallbackInfo& info);
    ImageContentFilterSeverity GetOptions() const;

private:
    ImageContentFilterSeverity m_options;
    
    Napi::Value GetAdultContentLevel(const Napi::CallbackInfo& info);
    void SetAdultContentLevel(const Napi::CallbackInfo& info, const Napi::Value& value);
    Napi::Value GetGoryContentLevel(const Napi::CallbackInfo& info);
    void SetGoryContentLevel(const Napi::CallbackInfo& info, const Napi::Value& value);
    Napi::Value GetRacyContentLevel(const Napi::CallbackInfo& info);
    void SetRacyContentLevel(const Napi::CallbackInfo& info, const Napi::Value& value);
    Napi::Value GetViolentContentLevel(const Napi::CallbackInfo& info);
    void SetViolentContentLevel(const Napi::CallbackInfo& info, const Napi::Value& value);
};

// Wrapper for TextContentFilterSeverity
class MyTextContentFilterSeverity : public Napi::ObjectWrap<MyTextContentFilterSeverity> {
public:
    static Napi::FunctionReference constructor;
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    
    MyTextContentFilterSeverity(const Napi::CallbackInfo& info);
    TextContentFilterSeverity GetOptions() const;

private:
    TextContentFilterSeverity m_options;
    
    Napi::Value GetHate(const Napi::CallbackInfo& info);
    void SetHate(const Napi::CallbackInfo& info, const Napi::Value& value);
    Napi::Value GetSelfHarm(const Napi::CallbackInfo& info);
    void SetSelfHarm(const Napi::CallbackInfo& info, const Napi::Value& value);
    Napi::Value GetSexual(const Napi::CallbackInfo& info);
    void SetSexual(const Napi::CallbackInfo& info, const Napi::Value& value);
    Napi::Value GetViolent(const Napi::CallbackInfo& info);
    void SetViolent(const Napi::CallbackInfo& info, const Napi::Value& value);
};