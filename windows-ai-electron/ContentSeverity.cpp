#include "ContentSeverity.h"
#include "ProjectionHelper.h"
#include <shobjidl_core.h>
#include <windows.h>
#include <winrt/Windows.Data.Xml.Dom.h>
#include <cstdlib>

using namespace Windows::Data::Xml::Dom;

// Static member definitions
Napi::FunctionReference MyContentFilterOptions::constructor;
Napi::FunctionReference MyImageContentFilterSeverity::constructor;
Napi::FunctionReference MyTextContentFilterSeverity::constructor;

// MyContentFilterOptions Implementation
Napi::Object MyContentFilterOptions::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "ContentFilterOptions", {
        InstanceAccessor("imageMaxAllowedSeverityLevel", &MyContentFilterOptions::GetImageMaxAllowedSeverityLevel, &MyContentFilterOptions::SetImageMaxAllowedSeverityLevel),
        InstanceAccessor("promptMaxAllowedSeverityLevel", &MyContentFilterOptions::GetPromptMaxAllowedSeverityLevel, &MyContentFilterOptions::SetPromptMaxAllowedSeverityLevel),
        InstanceAccessor("responseMaxAllowedSeverityLevel", &MyContentFilterOptions::GetResponseMaxAllowedSeverityLevel, &MyContentFilterOptions::SetResponseMaxAllowedSeverityLevel)
    });

    constructor = Napi::Persistent(func);
    exports.Set("ContentFilterOptions", func);
    return exports;
}

MyContentFilterOptions::MyContentFilterOptions(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyContentFilterOptions>(info) {
    if (info.Length() == 1 && info[0].IsExternal()) {
        auto external = info[0].As<Napi::External<ContentFilterOptions>>();
        m_options = *external.Data();
    } else {
        m_options = ContentFilterOptions();
    }
}

ContentFilterOptions MyContentFilterOptions::GetOptions() const {
    return m_options;
}

Napi::Value MyContentFilterOptions::GetImageMaxAllowedSeverityLevel(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        auto severityObj = m_options.ImageMaxAllowedSeverityLevel();
        if (!severityObj) {
            return env.Null();
        }
        auto external = Napi::External<ImageContentFilterSeverity>::New(env, &severityObj);
        auto instance = MyImageContentFilterSeverity::constructor.New({ external });
        return instance;
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
        return env.Null();
    }
}

void MyContentFilterOptions::SetImageMaxAllowedSeverityLevel(const Napi::CallbackInfo& info, const Napi::Value& value) {
    Napi::Env env = info.Env();
    try {
        if (value.IsNull() || value.IsUndefined()) {
            m_options.ImageMaxAllowedSeverityLevel(nullptr);
            return;
        }
        if (value.IsObject()) {
            auto severityWrapper = Napi::ObjectWrap<MyImageContentFilterSeverity>::Unwrap(value.As<Napi::Object>());
            if (severityWrapper) {
                m_options.ImageMaxAllowedSeverityLevel(severityWrapper->GetOptions());
                return;
            }
        }
        Napi::TypeError::New(env, "ImageMaxAllowedSeverityLevel must be an ImageContentFilterSeverity object or null/undefined").ThrowAsJavaScriptException();
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
    }
}

Napi::Value MyContentFilterOptions::GetPromptMaxAllowedSeverityLevel(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        auto severityObj = m_options.PromptMaxAllowedSeverityLevel();
        if (!severityObj) {
            return env.Null();
        }
        auto external = Napi::External<TextContentFilterSeverity>::New(env, &severityObj);
        auto instance = MyTextContentFilterSeverity::constructor.New({ external });
        return instance;
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
        return env.Null();
    }
}

void MyContentFilterOptions::SetPromptMaxAllowedSeverityLevel(const Napi::CallbackInfo& info, const Napi::Value& value) {
    Napi::Env env = info.Env();
    try {
        if (value.IsNull() || value.IsUndefined()) {
            m_options.PromptMaxAllowedSeverityLevel(nullptr);
            return;
        }
        if (value.IsObject()) {
            auto severityWrapper = Napi::ObjectWrap<MyTextContentFilterSeverity>::Unwrap(value.As<Napi::Object>());
            if (severityWrapper) {
                m_options.PromptMaxAllowedSeverityLevel(severityWrapper->GetOptions());
                return;
            }
        }
        Napi::TypeError::New(env, "PromptMaxAllowedSeverityLevel must be a TextContentFilterSeverity object or null/undefined").ThrowAsJavaScriptException();
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
    }
}

Napi::Value MyContentFilterOptions::GetResponseMaxAllowedSeverityLevel(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        auto severityObj = m_options.ResponseMaxAllowedSeverityLevel();
        if (!severityObj) {
            return env.Null();
        }
        auto external = Napi::External<TextContentFilterSeverity>::New(env, &severityObj);
        auto instance = MyTextContentFilterSeverity::constructor.New({ external });
        return instance;
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
        return env.Null();
    }
}

void MyContentFilterOptions::SetResponseMaxAllowedSeverityLevel(const Napi::CallbackInfo& info, const Napi::Value& value) {
    Napi::Env env = info.Env();
    try {
        if (value.IsNull() || value.IsUndefined()) {
            m_options.ResponseMaxAllowedSeverityLevel(nullptr);
            return;
        }
        if (value.IsObject()) {
            auto severityWrapper = Napi::ObjectWrap<MyTextContentFilterSeverity>::Unwrap(value.As<Napi::Object>());
            if (severityWrapper) {
                m_options.ResponseMaxAllowedSeverityLevel(severityWrapper->GetOptions());
                return;
            }
        }
        Napi::TypeError::New(env, "ResponseMaxAllowedSeverityLevel must be a TextContentFilterSeverity object or null/undefined").ThrowAsJavaScriptException();
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
    }
}

// MyImageContentFilterSeverity Implementation
Napi::Object MyImageContentFilterSeverity::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "ImageContentFilterSeverity", {
        InstanceAccessor("AdultContentLevel", &MyImageContentFilterSeverity::GetAdultContentLevel, &MyImageContentFilterSeverity::SetAdultContentLevel),
        InstanceAccessor("GoryContentLevel", &MyImageContentFilterSeverity::GetGoryContentLevel, &MyImageContentFilterSeverity::SetGoryContentLevel),
        InstanceAccessor("RacyContentLevel", &MyImageContentFilterSeverity::GetRacyContentLevel, &MyImageContentFilterSeverity::SetRacyContentLevel),
        InstanceAccessor("ViolentContentLevel", &MyImageContentFilterSeverity::GetViolentContentLevel, &MyImageContentFilterSeverity::SetViolentContentLevel)
    });

    constructor = Napi::Persistent(func);
    exports.Set("ImageContentFilterSeverity", func);
    return exports;
}

MyImageContentFilterSeverity::MyImageContentFilterSeverity(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyImageContentFilterSeverity>(info) {
    if (info.Length() == 1 && info[0].IsExternal()) {
        auto external = info[0].As<Napi::External<ImageContentFilterSeverity>>();
        m_options = *external.Data();
    } else if (info.Length() == 1 && info[0].IsNumber()) {
        int severityLevel = info[0].As<Napi::Number>().Int32Value();
        m_options = ImageContentFilterSeverity(static_cast<SeverityLevel>(severityLevel));
    } else {
        m_options = ImageContentFilterSeverity();
    }
}

ImageContentFilterSeverity MyImageContentFilterSeverity::GetOptions() const {
    return m_options;
}

Napi::Value MyImageContentFilterSeverity::GetAdultContentLevel(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        auto level = m_options.AdultContentLevel();
        return Napi::Number::New(env, static_cast<int>(level));
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
        return env.Null();
    }
}

void MyImageContentFilterSeverity::SetAdultContentLevel(const Napi::CallbackInfo& info, const Napi::Value& value) {
    Napi::Env env = info.Env();
    try {
        if (!value.IsNumber()) {
            Napi::TypeError::New(env, "AdultContentLevel must be a number").ThrowAsJavaScriptException();
            return;
        }
        int level = value.As<Napi::Number>().Int32Value();
        m_options.AdultContentLevel(static_cast<SeverityLevel>(level));
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
    }
}

Napi::Value MyImageContentFilterSeverity::GetGoryContentLevel(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        auto level = m_options.GoryContentLevel();
        return Napi::Number::New(env, static_cast<int>(level));
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
        return env.Null();
    }
}

void MyImageContentFilterSeverity::SetGoryContentLevel(const Napi::CallbackInfo& info, const Napi::Value& value) {
    Napi::Env env = info.Env();
    try {
        if (!value.IsNumber()) {
            Napi::TypeError::New(env, "GoryContentLevel must be a number").ThrowAsJavaScriptException();
            return;
        }
        int level = value.As<Napi::Number>().Int32Value();
        m_options.GoryContentLevel(static_cast<SeverityLevel>(level));
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
    }
}

Napi::Value MyImageContentFilterSeverity::GetRacyContentLevel(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        auto level = m_options.RacyContentLevel();
        return Napi::Number::New(env, static_cast<int>(level));
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
        return env.Null();
    }
}

void MyImageContentFilterSeverity::SetRacyContentLevel(const Napi::CallbackInfo& info, const Napi::Value& value) {
    Napi::Env env = info.Env();
    try {
        if (!value.IsNumber()) {
            Napi::TypeError::New(env, "RacyContentLevel must be a number").ThrowAsJavaScriptException();
            return;
        }
        int level = value.As<Napi::Number>().Int32Value();
        m_options.RacyContentLevel(static_cast<SeverityLevel>(level));
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
    }
}

Napi::Value MyImageContentFilterSeverity::GetViolentContentLevel(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        auto level = m_options.ViolentContentLevel();
        return Napi::Number::New(env, static_cast<int>(level));
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
        return env.Null();
    }
}

void MyImageContentFilterSeverity::SetViolentContentLevel(const Napi::CallbackInfo& info, const Napi::Value& value) {
    Napi::Env env = info.Env();
    try {
        if (!value.IsNumber()) {
            Napi::TypeError::New(env, "ViolentContentLevel must be a number").ThrowAsJavaScriptException();
            return;
        }
        int level = value.As<Napi::Number>().Int32Value();
        m_options.ViolentContentLevel(static_cast<SeverityLevel>(level));
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
    }
}

// MyTextContentFilterSeverity Implementation
Napi::Object MyTextContentFilterSeverity::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "TextContentFilterSeverity", {
        InstanceAccessor("Hate", &MyTextContentFilterSeverity::GetHate, &MyTextContentFilterSeverity::SetHate),
        InstanceAccessor("SelfHarm", &MyTextContentFilterSeverity::GetSelfHarm, &MyTextContentFilterSeverity::SetSelfHarm),
        InstanceAccessor("Sexual", &MyTextContentFilterSeverity::GetSexual, &MyTextContentFilterSeverity::SetSexual),
        InstanceAccessor("Violent", &MyTextContentFilterSeverity::GetViolent, &MyTextContentFilterSeverity::SetViolent)
    });

    constructor = Napi::Persistent(func);
    exports.Set("TextContentFilterSeverity", func);
    return exports;
}

MyTextContentFilterSeverity::MyTextContentFilterSeverity(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyTextContentFilterSeverity>(info) {
    if (info.Length() == 1 && info[0].IsExternal()) {
        auto external = info[0].As<Napi::External<TextContentFilterSeverity>>();
        m_options = *external.Data();
    } else if (info.Length() == 1 && info[0].IsNumber()) {
        int severityLevel = info[0].As<Napi::Number>().Int32Value();
        m_options = TextContentFilterSeverity(static_cast<SeverityLevel>(severityLevel));
    } else {
        m_options = TextContentFilterSeverity();
    }
}

TextContentFilterSeverity MyTextContentFilterSeverity::GetOptions() const {
    return m_options;
}

Napi::Value MyTextContentFilterSeverity::GetHate(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        auto level = m_options.Hate();
        return Napi::Number::New(env, static_cast<int>(level));
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
        return env.Null();
    }
}

void MyTextContentFilterSeverity::SetHate(const Napi::CallbackInfo& info, const Napi::Value& value) {
    Napi::Env env = info.Env();
    try {
        if (!value.IsNumber()) {
            Napi::TypeError::New(env, "Hate must be a number").ThrowAsJavaScriptException();
            return;
        }
        int level = value.As<Napi::Number>().Int32Value();
        m_options.Hate(static_cast<SeverityLevel>(level));
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
    }
}

Napi::Value MyTextContentFilterSeverity::GetSelfHarm(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        auto level = m_options.SelfHarm();
        return Napi::Number::New(env, static_cast<int>(level));
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
        return env.Null();
    }
}

void MyTextContentFilterSeverity::SetSelfHarm(const Napi::CallbackInfo& info, const Napi::Value& value) {
    Napi::Env env = info.Env();
    try {
        if (!value.IsNumber()) {
            Napi::TypeError::New(env, "SelfHarm must be a number").ThrowAsJavaScriptException();
            return;
        }
        int level = value.As<Napi::Number>().Int32Value();
        m_options.SelfHarm(static_cast<SeverityLevel>(level));
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
    }
}

Napi::Value MyTextContentFilterSeverity::GetSexual(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        auto level = m_options.Sexual();
        return Napi::Number::New(env, static_cast<int>(level));
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
        return env.Null();
    }
}

void MyTextContentFilterSeverity::SetSexual(const Napi::CallbackInfo& info, const Napi::Value& value) {
    Napi::Env env = info.Env();
    try {
        if (!value.IsNumber()) {
            Napi::TypeError::New(env, "Sexual must be a number").ThrowAsJavaScriptException();
            return;
        }
        int level = value.As<Napi::Number>().Int32Value();
        m_options.Sexual(static_cast<SeverityLevel>(level));
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
    }
}

Napi::Value MyTextContentFilterSeverity::GetViolent(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        auto level = m_options.Violent();
        return Napi::Number::New(env, static_cast<int>(level));
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
        return env.Null();
    }
}

void MyTextContentFilterSeverity::SetViolent(const Napi::CallbackInfo& info, const Napi::Value& value) {
    Napi::Env env = info.Env();
    try {
        if (!value.IsNumber()) {
            Napi::TypeError::New(env, "Violent must be a number").ThrowAsJavaScriptException();
            return;
        }
        int level = value.As<Napi::Number>().Int32Value();
        m_options.Violent(static_cast<SeverityLevel>(level));
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
    }
}