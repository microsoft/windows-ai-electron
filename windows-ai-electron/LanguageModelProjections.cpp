#include "LanguageModelProjections.h"
#include "ContentSeverity.h"
#include "ProjectionHelper.h"
#include <shobjidl_core.h>
#include <windows.h>
#include <winrt/Windows.Data.Xml.Dom.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <cstdlib>
#include "LimitedAccessFeature.h"

using namespace Windows::Data::Xml::Dom;

// Static member definitions
Napi::FunctionReference MyLanguageModelResponseResult::constructor;
Napi::FunctionReference MyAIFeatureReadyResult::constructor;
Napi::FunctionReference MyLanguageModel::constructor;
Napi::FunctionReference MyConversationItem::constructor;
Napi::FunctionReference MyTextSummarizer::constructor;
Napi::FunctionReference MyTextRewriter::constructor;
Napi::FunctionReference MyTextToTableConverter::constructor;
Napi::FunctionReference MyTextToTableResponseResult::constructor;
Napi::FunctionReference MyTextToTableRow::constructor;

// MyLanguageModelResponseResult Implementation
Napi::Object MyLanguageModelResponseResult::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "LanguageModelResponseResult", {
        InstanceAccessor("Text", &MyLanguageModelResponseResult::GetText, nullptr),
        InstanceAccessor("Status", &MyLanguageModelResponseResult::GetStatus, nullptr),
        InstanceAccessor("ExtendedError", &MyLanguageModelResponseResult::GetExtendedError, nullptr)
    });

    constructor = Napi::Persistent(func);
    exports.Set("LanguageModelResponseResult", func);
    return exports;
}

MyLanguageModelResponseResult::MyLanguageModelResponseResult(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyLanguageModelResponseResult>(info) {
    if (info.Length() == 1 && info[0].IsExternal()) {
        auto external = info[0].As<Napi::External<LanguageModelResponseResult>>();
        m_result = *external.Data();
    }
    // If no external data, m_result remains empty (std::nullopt)
}

bool MyLanguageModelResponseResult::HasResult() const {
    return m_result.has_value();
}

Napi::Value MyLanguageModelResponseResult::GetText(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        if (!m_result.has_value()) {
            Napi::Error::New(env, "No result data available").ThrowAsJavaScriptException();
            return env.Null();
        }
        auto text = m_result->Text();
        return Napi::String::New(env, winrt::to_string(text));
    } catch (const winrt::hresult_error& ex) {
        std::string errorMsg = "WinRT error getting text: " + winrt::to_string(ex.message());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (const std::exception& ex) {
        std::string errorMsg = "Error getting text: " + std::string(ex.what());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (...) {
        Napi::Error::New(env, "Unknown error getting text").ThrowAsJavaScriptException();
        return env.Null();
    }
}

Napi::Value MyLanguageModelResponseResult::GetStatus(const Napi::CallbackInfo& info) {
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

Napi::Value MyLanguageModelResponseResult::GetExtendedError(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        if (!m_result.has_value()) {
            Napi::Error::New(env, "No result data available").ThrowAsJavaScriptException();
            return env.Null();
        }
        auto extendedError = m_result->ExtendedError();
        if (!extendedError) {
            return env.Null();
        }
        // Workaround to get error message from HRESULT, code may need to be adjusted based on actual error handling needs
        winrt::hresult_error hresultError{ extendedError };
        std::string errorMessage = winrt::to_string(hresultError.message());
        if (!errorMessage.empty()) {
            return Napi::String::New(env, errorMessage);
        }
        return Napi::String::New(env, "Extended error present but message could not be extracted");
    } catch (const winrt::hresult_error& ex) {
        std::string errorMsg = "WinRT error getting extended error: " + winrt::to_string(ex.message());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (const std::exception& ex) {
        std::string errorMsg = "Error getting extended error: " + std::string(ex.what());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (...) {
        Napi::Error::New(env, "Unknown error getting extended error").ThrowAsJavaScriptException();
        return env.Null();
    }
}

// MyLanguageModelOptions Implementation
Napi::Object MyLanguageModelOptions::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "LanguageModelOptions", {
        InstanceAccessor("Temperature", &MyLanguageModelOptions::GetTemperature, &MyLanguageModelOptions::SetTemperature),
        InstanceAccessor("TopK", &MyLanguageModelOptions::GetTopK, &MyLanguageModelOptions::SetTopK),
        InstanceAccessor("TopP", &MyLanguageModelOptions::GetTopP, &MyLanguageModelOptions::SetTopP),
        InstanceAccessor("ContentFilterOptions", &MyLanguageModelOptions::GetContentFilterOptions, &MyLanguageModelOptions::SetContentFilterOptions)
    });

    exports.Set("LanguageModelOptions", func);
    return exports;
}

MyLanguageModelOptions::MyLanguageModelOptions(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyLanguageModelOptions>(info) {
    m_options = LanguageModelOptions();
}

LanguageModelOptions MyLanguageModelOptions::GetOptions() const {
    return m_options;
}

Napi::Value MyLanguageModelOptions::GetTemperature(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        float temperature = m_options.Temperature();
        return Napi::Number::New(env, temperature);
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
        return env.Null();
    }
}

void MyLanguageModelOptions::SetTemperature(const Napi::CallbackInfo& info, const Napi::Value& value) {
    Napi::Env env = info.Env();
    try {
        if (!value.IsNumber()) {
            Napi::TypeError::New(env, "Temperature must be a number").ThrowAsJavaScriptException();
            return;
        }
        float temperature = value.As<Napi::Number>().FloatValue();
        m_options.Temperature(temperature);
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
    }
}

Napi::Value MyLanguageModelOptions::GetTopK(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        uint32_t topK = m_options.TopK();
        return Napi::Number::New(env, topK);
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
        return env.Null();
    }
}

void MyLanguageModelOptions::SetTopK(const Napi::CallbackInfo& info, const Napi::Value& value) {
    Napi::Env env = info.Env();
    try {
        if (!value.IsNumber()) {
            Napi::TypeError::New(env, "TopK must be a number").ThrowAsJavaScriptException();
            return;
        }
        uint32_t topK = value.As<Napi::Number>().Uint32Value();
        m_options.TopK(topK);
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
    }
}

Napi::Value MyLanguageModelOptions::GetTopP(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        float topP = m_options.TopP();
        return Napi::Number::New(env, topP);
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
        return env.Null();
    }
}

void MyLanguageModelOptions::SetTopP(const Napi::CallbackInfo& info, const Napi::Value& value) {
    Napi::Env env = info.Env();
    try {
        if (!value.IsNumber()) {
            Napi::TypeError::New(env, "TopP must be a number").ThrowAsJavaScriptException();
            return;
        }
        float topP = value.As<Napi::Number>().FloatValue();
        m_options.TopP(topP);
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
    }
}

Napi::Value MyLanguageModelOptions::GetContentFilterOptions(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        auto contentFilter = m_options.ContentFilterOptions();
        if (contentFilter == nullptr) {
            return env.Null();
        }
        
        auto external = Napi::External<ContentFilterOptions>::New(env, &contentFilter);
        auto instance = MyContentFilterOptions::constructor.New({ external });
        return instance;
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
        return env.Null();
    }
}

void MyLanguageModelOptions::SetContentFilterOptions(const Napi::CallbackInfo& info, const Napi::Value& value) {
    Napi::Env env = info.Env();
    try {
        if (value.IsNull() || value.IsUndefined()) {
            m_options.ContentFilterOptions(nullptr);
            return;
        }
        if (value.IsObject()) {
            auto optionsWrapper = Napi::ObjectWrap<MyContentFilterOptions>::Unwrap(value.As<Napi::Object>());
            if (optionsWrapper) {
                m_options.ContentFilterOptions(optionsWrapper->GetOptions());
                return;
            }
        }
        Napi::TypeError::New(env, "ContentFilterOptions must be a ContentFilterOptions object or null/undefined").ThrowAsJavaScriptException();
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
    }
}

// MyAIFeatureReadyResult Implementation
Napi::Object MyAIFeatureReadyResult::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "AIFeatureReadyResult", {
        InstanceAccessor("Error", &MyAIFeatureReadyResult::GetError, nullptr),
        InstanceAccessor("ErrorDisplayText", &MyAIFeatureReadyResult::GetErrorDisplayText, nullptr),
        InstanceAccessor("ExtendedError", &MyAIFeatureReadyResult::GetExtendedError, nullptr),
        InstanceAccessor("Status", &MyAIFeatureReadyResult::GetStatus, nullptr)
    });

    constructor = Napi::Persistent(func);
    exports.Set("AIFeatureReadyResult", func);
    return exports;
}

MyAIFeatureReadyResult::MyAIFeatureReadyResult(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyAIFeatureReadyResult>(info) {
    if (info.Length() == 1 && info[0].IsExternal()) {
        auto external = info[0].As<Napi::External<AIFeatureReadyResult>>();
        m_result = *external.Data();
    }
    // If no external data, m_result remains empty (std::nullopt)
}

bool MyAIFeatureReadyResult::HasResult() const {
    return m_result.has_value();
}

Napi::Value MyAIFeatureReadyResult::GetError(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        if (!m_result.has_value()) {
            Napi::Error::New(env, "No result data available").ThrowAsJavaScriptException();
            return env.Null();
        }
        auto error = m_result->Error();
        return Napi::Number::New(env, static_cast<int32_t>(error));
    } catch (const winrt::hresult_error& ex) {
        std::string errorMsg = "WinRT error getting error: " + winrt::to_string(ex.message());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (const std::exception& ex) {
        std::string errorMsg = "Error getting error: " + std::string(ex.what());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (...) {
        Napi::Error::New(env, "Unknown error getting error").ThrowAsJavaScriptException();
        return env.Null();
    }
}

Napi::Value MyAIFeatureReadyResult::GetErrorDisplayText(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        if (!m_result.has_value()) {
            Napi::Error::New(env, "No result data available").ThrowAsJavaScriptException();
            return env.Null();
        }
        auto errorDisplayText = m_result->ErrorDisplayText();
        std::string combinedErrorText = winrt::to_string(errorDisplayText) + 
            " If EnsureReadyAsync or GetReadyState returned Failure, your machine is missing system requirements for running Windows AI APIs. Check https://learn.microsoft.com/windows/ai/apis/get-started for more information.";
        return Napi::String::New(env, combinedErrorText);
    } catch (const winrt::hresult_error& ex) {
        std::string errorMsg = "WinRT error getting error display text: " + winrt::to_string(ex.message());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (const std::exception& ex) {
        std::string errorMsg = "Error getting error display text: " + std::string(ex.what());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (...) {
        Napi::Error::New(env, "Unknown error getting error display text").ThrowAsJavaScriptException();
        return env.Null();
    }
}

Napi::Value MyAIFeatureReadyResult::GetExtendedError(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        if (!m_result.has_value()) {
            Napi::Error::New(env, "No result data available").ThrowAsJavaScriptException();
            return env.Null();
        }
        auto extendedError = m_result->ExtendedError();
        return Napi::Number::New(env, static_cast<int32_t>(extendedError));
    } catch (const winrt::hresult_error& ex) {
        std::string errorMsg = "WinRT error getting extended error: " + winrt::to_string(ex.message());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (const std::exception& ex) {
        std::string errorMsg = "Error getting extended error: " + std::string(ex.what());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (...) {
        Napi::Error::New(env, "Unknown error getting extended error").ThrowAsJavaScriptException();
        return env.Null();
    }
}

Napi::Value MyAIFeatureReadyResult::GetStatus(const Napi::CallbackInfo& info) {
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

// MyLanguageModel Implementation
Napi::Object MyLanguageModel::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "LanguageModel", {
        InstanceMethod("GenerateResponseAsync", &MyLanguageModel::MyGenerateResponseAsync),
        StaticMethod("CreateAsync", &MyLanguageModel::MyCreateAsync),
        StaticMethod("GetReadyState", &MyLanguageModel::MyGetReadyState),
        StaticMethod("EnsureReadyAsync", &MyLanguageModel::MyEnsureReadyAsync)
    });

    constructor = Napi::Persistent(func);
    exports.Set("LanguageModel", func);
    return exports;
}

Napi::Value MyLanguageModel::MyCreateAsync(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    auto deferred = Napi::Promise::Deferred::New(env);
    auto tsfn = Napi::ThreadSafeFunction::New(
        env,
        Napi::Function::New(env, [](const Napi::CallbackInfo&) {}),
        "CreateAsync",
        0,
        1 
    );

    // Create a shared pointer that automatically releases tsfn when done
    auto tsfn_guard = std::shared_ptr<void>(nullptr, [tsfn](void*) mutable { tsfn.Release(); });
    
    try {
        auto asyncOp = LanguageModel::CreateAsync();
        
        auto completionHandler = [deferred, tsfn, tsfn_guard](auto const& sender, auto const& status) mutable {
            auto callback = [deferred, sender, status](Napi::Env env, Napi::Function) {
                try {
                    if (status == winrt::Windows::Foundation::AsyncStatus::Completed) {
                        auto languageModel = sender.GetResults();
                        if (!languageModel) {
                            deferred.Reject(Napi::Error::New(env, "Failed to create LanguageModel instance.").Value());
                            return;
                        }
                        
                        auto persistentModel = std::make_shared<LanguageModel>(std::move(languageModel));
                        auto external = Napi::External<LanguageModel>::New(env, 
                            persistentModel.get(),
                            [persistentModel](Napi::Env env, LanguageModel* data) {
                                // persistentModel will be destroyed here, releasing the WinRT object
                            });

                        auto instance = MyLanguageModel::constructor.New({ external });
                        deferred.Resolve(instance);
                    } else {
                        deferred.Reject(Napi::Error::New(env, "LanguageModel creation was cancelled or failed.").Value());
                    }
                } catch (const winrt::hresult_error& ex) {
                    deferred.Reject(Napi::Error::New(env, winrt::to_string(ex.message())).Value());
                } catch (const std::exception& ex) {
                    deferred.Reject(Napi::Error::New(env, ex.what()).Value());
                } catch (...) {
                    deferred.Reject(Napi::Error::New(env, "Unknown error occurred in CreateAsync").Value());
                }
            };
            
            tsfn.BlockingCall(callback);
        };      
        asyncOp.Completed(completionHandler);
        return deferred.Promise();
        
    } catch (const winrt::hresult_error& ex) {
        deferred.Reject(Napi::Error::New(env, winrt::to_string(ex.message())).Value());
        return deferred.Promise();
    } catch (const std::exception& ex) {
        deferred.Reject(Napi::Error::New(env, ex.what()).Value());
        return deferred.Promise();
    } catch (...) {
        deferred.Reject(Napi::Error::New(env, "Unknown error occurred in CreateAsync").Value());
        return deferred.Promise();
    }
}

Napi::Value MyLanguageModel::MyGetReadyState(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    auto readyState = LanguageModel::GetReadyState();
    return Napi::Number::New(env, static_cast<int>(readyState));
}

Napi::Value MyLanguageModel::MyEnsureReadyAsync(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    auto deferred = Napi::Promise::Deferred::New(env);
    auto tsfn = Napi::ThreadSafeFunction::New(
        env,
        Napi::Function::New(env, [](const Napi::CallbackInfo&) {}),
        "EnsureReadyAsync",
        0,
        1 
    );

    // Create a shared pointer that automatically releases tsfn when done
    auto tsfn_guard = std::shared_ptr<void>(nullptr, [tsfn](void*) mutable { tsfn.Release(); });
    auto progressPromise = ProgressPromise::Create(env, deferred);
    auto progressTsfn = progressPromise.GetProgressTsfn();
    
    try {
        auto asyncOp = LanguageModel::EnsureReadyAsync();
        
        asyncOp.Progress([progressTsfn](auto const&, auto const& progressValue) {
            if (progressTsfn && *progressTsfn) {
                (*progressTsfn)->NonBlockingCall([progressValue](Napi::Env env, Napi::Function jsCallback) {
                    try {
                        jsCallback.Call({ env.Null(), Napi::Number::New(env, progressValue) });
                    } catch (...) {}
                });
            }
        });
        
        auto completionHandler = [deferred, tsfn, tsfn_guard](auto const& sender, auto const& status) mutable {
            auto callback = [deferred, sender, status](Napi::Env env, Napi::Function) {
                try {
                    if (status == winrt::Windows::Foundation::AsyncStatus::Completed) {
                        auto result = sender.GetResults();
                        auto external = Napi::External<AIFeatureReadyResult>::New(env, &result);
                        auto resultWrapper = MyAIFeatureReadyResult::constructor.New({ external });
                        deferred.Resolve(resultWrapper);
                    } else {
                        deferred.Reject(Napi::Error::New(env, "EnsureReadyAsync was cancelled or failed.").Value());
                    }
                } catch (const winrt::hresult_error& ex) {
                    deferred.Reject(Napi::Error::New(env, winrt::to_string(ex.message())).Value());
                } catch (const std::exception& ex) {
                    deferred.Reject(Napi::Error::New(env, ex.what()).Value());
                } catch (...) {
                    deferred.Reject(Napi::Error::New(env, "Unknown error occurred in EnsureReadyAsync").Value());
                }
            };
            
            tsfn.BlockingCall(callback);
        };      
        asyncOp.Completed(completionHandler);
        return progressPromise.GetPromiseObject();
        
    } catch (const winrt::hresult_error& ex) {
        deferred.Reject(Napi::Error::New(env, winrt::to_string(ex.message())).Value());
        return progressPromise.GetPromiseObject();
    } catch (const std::exception& ex) {
        deferred.Reject(Napi::Error::New(env, ex.what()).Value());
        return progressPromise.GetPromiseObject();
    } catch (...) {
        deferred.Reject(Napi::Error::New(env, "Unknown error occurred in EnsureReadyAsync").Value());
        return progressPromise.GetPromiseObject();
    }
}

MyLanguageModel::MyLanguageModel(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyLanguageModel>(info) {
    if (info.Length() == 0 || !info[0].IsExternal()) {
        Napi::Error::New(info.Env(), "Cannot instantiate LanguageModel directly. Use LanguageModel.CreateAsync()").ThrowAsJavaScriptException();
        return;
    }
    
    auto external = info[0].As<Napi::External<LanguageModel>>();
    m_languagemodel = external.Data();
}

Napi::Value MyLanguageModel::MyGenerateResponseAsync(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    // Check if Limited Access Feature has been unlocked
    if (!MyLimitedAccessFeatures::IsFeatureUnlocked()) {
        Napi::Error::New(env, "GenerateResponseAsync requires the Limited Access Feature to be unlocked. Call LimitedAccessFeatures.TryUnlockFeature() with a valid token before using this API. Request a token at: https://go.microsoft.com/fwlink/?linkid=2271232").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    if (info.Length() < 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "First parameter must be a string").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    std::string prompt = info[0].As<Napi::String>();
    if (prompt.empty()) {
        Napi::TypeError::New(env, "First parameter must be a non-empty string").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto deferred = Napi::Promise::Deferred::New(env);
    auto tsfn = Napi::ThreadSafeFunction::New(env, Napi::Function::New(env, [](const Napi::CallbackInfo&) {}), "GenerateResponseAsync", 0, 1);
    auto tsfn_guard = std::shared_ptr<void>(nullptr, [tsfn](void*) mutable { tsfn.Release(); });
    auto progressPromise = ProgressPromise::Create(env, deferred);
    auto progressTsfn = progressPromise.GetProgressTsfn();

    try {
        auto asyncOp = [&]() {
            if (info.Length() >= 2 && info[1].IsObject()) {
                auto optionsWrapper = Napi::ObjectWrap<MyLanguageModelOptions>::Unwrap(info[1].As<Napi::Object>());
                if (!optionsWrapper) throw std::runtime_error("Invalid options: Please provide a LanguageModelOptions object.");
                return m_languagemodel->GenerateResponseAsync(winrt::to_hstring(prompt), optionsWrapper->GetOptions());
            } else {
                return m_languagemodel->GenerateResponseAsync(winrt::to_hstring(prompt));
            }
        }();

        asyncOp.Progress([progressTsfn](auto const&, auto const& progressText) {
            if (progressTsfn && *progressTsfn) {
                auto progressStr = winrt::to_string(progressText);
                (*progressTsfn)->NonBlockingCall([progressStr](Napi::Env env, Napi::Function jsCallback) {
                    try {
                        jsCallback.Call({ env.Null(), Napi::String::New(env, progressStr) });
                    } catch (...) {}
                });
            }
        });
        
        asyncOp.Completed([deferred, tsfn, tsfn_guard](auto const& sender, auto const& status) mutable {
            tsfn.BlockingCall([deferred, sender, status](Napi::Env env, Napi::Function) {
                try {
                    if (status == winrt::Windows::Foundation::AsyncStatus::Completed) {
                        auto result = sender.GetResults();
                        auto external = Napi::External<LanguageModelResponseResult>::New(env, &result);
                        auto resultWrapper = MyLanguageModelResponseResult::constructor.New({ external });
                        deferred.Resolve(resultWrapper);
                    } else {
                        deferred.Reject(Napi::Error::New(env, "Operation failed or cancelled").Value());
                    }
                } catch (...) {
                    deferred.Reject(Napi::Error::New(env, "Unknown error occurred").Value());
                }
            });
        });

        return progressPromise.GetPromiseObject();
        
    } catch (const winrt::hresult_error& ex) {
        deferred.Reject(Napi::Error::New(env, winrt::to_string(ex.message())).Value());
        return progressPromise.GetPromiseObject();
    } catch (const std::exception& ex) {
        deferred.Reject(Napi::Error::New(env, ex.what()).Value());
        return progressPromise.GetPromiseObject();
    } catch (...) {
        deferred.Reject(Napi::Error::New(env, "Unknown error occurred in EnsureReadyAsync").Value());
        return progressPromise.GetPromiseObject();
    }
}

// MyConversationItem Implementation
Napi::Object MyConversationItem::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "ConversationItem", {
        InstanceAccessor("Message", &MyConversationItem::MyGetMessage, &MyConversationItem::MySetMessage),
        InstanceAccessor("Participant", &MyConversationItem::MyGetParticipant, &MyConversationItem::MySetParticipant)
    });

    constructor = Napi::Persistent(func);
    exports.Set("ConversationItem", func);
    return exports;
}

MyConversationItem::MyConversationItem(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyConversationItem>(info) {
    if (info.Length() == 1 && info[0].IsExternal()) {
        auto external = info[0].As<Napi::External<ConversationItem>>();
        m_item = *external.Data();
    } else {
        m_item = ConversationItem();
    }
}

Napi::Value MyConversationItem::MyGetMessage(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        auto message = m_item.Message();
        return Napi::String::New(env, winrt::to_string(message));
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
        return env.Null();
    }
}

void MyConversationItem::MySetMessage(const Napi::CallbackInfo& info, const Napi::Value& value) {
    Napi::Env env = info.Env();
    try {
        if (!value.IsString()) {
            Napi::TypeError::New(env, "Message must be a string").ThrowAsJavaScriptException();
            return;
        }
        std::string message = value.As<Napi::String>().Utf8Value();
        m_item.Message(winrt::to_hstring(message));
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
    }
}

Napi::Value MyConversationItem::MyGetParticipant(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        auto participant = m_item.Participant();
        return Napi::String::New(env, winrt::to_string(participant));
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
        return env.Null();
    }
}

void MyConversationItem::MySetParticipant(const Napi::CallbackInfo& info, const Napi::Value& value) {
    Napi::Env env = info.Env();
    try {
        if (!value.IsString()) {
            Napi::TypeError::New(env, "Participant must be a string").ThrowAsJavaScriptException();
            return;
        }
        std::string participant = value.As<Napi::String>().Utf8Value();
        m_item.Participant(winrt::to_hstring(participant));
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
    }
}

// MyTextSummarizer Implementation

Napi::Object MyTextSummarizer::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "TextSummarizer", {
        InstanceMethod("SummarizeAsync", &MyTextSummarizer::MySummarizeAsync),
        InstanceMethod("SummarizeConversationAsync", &MyTextSummarizer::MySummarizeConversationAsync),
        InstanceMethod("SummarizeParagraphAsync", &MyTextSummarizer::MySummarizeParagraphAsync),
        InstanceMethod("IsPromptLargerThanContext", &MyTextSummarizer::MyIsPromptLargerThanContext)
    });

    constructor = Napi::Persistent(func);
    exports.Set("TextSummarizer", func);
    return exports;
}

MyTextSummarizer::MyTextSummarizer(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyTextSummarizer>(info) {
    if (info.Length() == 0) {
        Napi::Error::New(info.Env(), "TextSummarizer constructor requires a LanguageModel instance").ThrowAsJavaScriptException();
        return;
    }
    
    // Check if first parameter is an External<TextSummarizer> (for internal use)
    if (info[0].IsExternal()) {
        auto external = info[0].As<Napi::External<TextSummarizer>>();
        m_summarizer = std::make_shared<TextSummarizer>(*external.Data());
        return;
    }
    
    // Check if first parameter is a MyLanguageModel instance (for direct instantiation)
    if (info[0].IsObject()) {
        auto languageModelObj = info[0].As<Napi::Object>();
        auto languageModelWrapper = Napi::ObjectWrap<MyLanguageModel>::Unwrap(languageModelObj);
        if (languageModelWrapper) {
            try {
                auto languageModel = languageModelWrapper->GetLanguageModel();
                if (languageModel) {
                    m_summarizer = std::make_shared<TextSummarizer>(*languageModel);
                    return;
                }
            } catch (const winrt::hresult_error& ex) {
                std::string errorMsg = "Failed to create TextSummarizer: " + winrt::to_string(ex.message());
                Napi::Error::New(info.Env(), errorMsg).ThrowAsJavaScriptException();
                return;
            } catch (const std::exception& ex) {
                std::string errorMsg = "Failed to create TextSummarizer: " + std::string(ex.what());
                Napi::Error::New(info.Env(), errorMsg).ThrowAsJavaScriptException();
                return;
            } catch (...) {
                Napi::Error::New(info.Env(), "Unknown error creating TextSummarizer").ThrowAsJavaScriptException();
                return;
            }
        }
    }
    
    Napi::Error::New(info.Env(), "TextSummarizer constructor requires a valid LanguageModel instance").ThrowAsJavaScriptException();
}

Napi::Value MyTextSummarizer::MySummarizeAsync(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "SummarizeAsync requires a string parameter").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    auto deferred = Napi::Promise::Deferred::New(env);
    auto tsfn = Napi::ThreadSafeFunction::New(env, Napi::Function::New(env, [](const Napi::CallbackInfo&) {}), "SummarizeAsync", 0, 1);
    auto tsfn_guard = std::shared_ptr<void>(nullptr, [tsfn](void*) mutable { tsfn.Release(); });
    auto progressPromise = ProgressPromise::Create(env, deferred);
    auto progressTsfn = progressPromise.GetProgressTsfn();

    try {
        std::string text = info[0].As<Napi::String>().Utf8Value();
        winrt::hstring wText = winrt::to_hstring(text);
        
        auto asyncOp = m_summarizer->SummarizeAsync(wText);
        
        asyncOp.Progress([progressTsfn](auto const&, auto const& progressText) {
            if (progressTsfn && *progressTsfn) {
                auto progressStr = winrt::to_string(progressText);
                (*progressTsfn)->NonBlockingCall([progressStr](Napi::Env env, Napi::Function jsCallback) {
                    try {
                        jsCallback.Call({ env.Null(), Napi::String::New(env, progressStr) });
                    } catch (...) {}
                });
            }
        });
        
        asyncOp.Completed([deferred, tsfn, tsfn_guard](auto const& sender, auto const& status) mutable {
            tsfn.BlockingCall([deferred, sender, status](Napi::Env env, Napi::Function) {
                try {
                    if (status == winrt::Windows::Foundation::AsyncStatus::Completed) {
                        auto result = sender.GetResults();
                        auto external = Napi::External<LanguageModelResponseResult>::New(env, &result);
                        auto resultWrapper = MyLanguageModelResponseResult::constructor.New({ external });
                        deferred.Resolve(resultWrapper);
                    } else {
                        deferred.Reject(Napi::Error::New(env, "SummarizeAsync was cancelled or failed").Value());
                    }
                } catch (const winrt::hresult_error& ex) {
                    deferred.Reject(Napi::Error::New(env, winrt::to_string(ex.message())).Value());
                } catch (const std::exception& ex) {
                    deferred.Reject(Napi::Error::New(env, ex.what()).Value());
                } catch (...) {
                    deferred.Reject(Napi::Error::New(env, "Unknown error occurred in SummarizeAsync").Value());
                }
            });
        });
        
        return progressPromise.GetPromiseObject();
        
    } catch (const winrt::hresult_error& ex) {
        deferred.Reject(Napi::Error::New(env, winrt::to_string(ex.message())).Value());
        return progressPromise.GetPromiseObject();
    } catch (const std::exception& ex) {
        deferred.Reject(Napi::Error::New(env, ex.what()).Value());
        return progressPromise.GetPromiseObject();
    } catch (...) {
        deferred.Reject(Napi::Error::New(env, "Unknown error occurred in SummarizeAsync").Value());
        return progressPromise.GetPromiseObject();
    }
}

Napi::Value MyTextSummarizer::MySummarizeParagraphAsync(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "SummarizeParagraphAsync requires a string parameter").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    auto deferred = Napi::Promise::Deferred::New(env);
    auto tsfn = Napi::ThreadSafeFunction::New(env, Napi::Function::New(env, [](const Napi::CallbackInfo&) {}), "SummarizeParagraphAsync", 0, 1);
    auto tsfn_guard = std::shared_ptr<void>(nullptr, [tsfn](void*) mutable { tsfn.Release(); });
    auto progressPromise = ProgressPromise::Create(env, deferred);
    auto progressTsfn = progressPromise.GetProgressTsfn();

    try {
        std::string text = info[0].As<Napi::String>().Utf8Value();
        winrt::hstring wText = winrt::to_hstring(text);
        
        auto asyncOp = m_summarizer->SummarizeParagraphAsync(wText);
        
        asyncOp.Progress([progressTsfn](auto const&, auto const& progressText) {
            if (progressTsfn && *progressTsfn) {
                auto progressStr = winrt::to_string(progressText);
                (*progressTsfn)->NonBlockingCall([progressStr](Napi::Env env, Napi::Function jsCallback) {
                    try {
                        jsCallback.Call({ env.Null(), Napi::String::New(env, progressStr) });
                    } catch (...) {}
                });
            }
        });
        
        asyncOp.Completed([deferred, tsfn, tsfn_guard](auto const& sender, auto const& status) mutable {
            tsfn.BlockingCall([deferred, sender, status](Napi::Env env, Napi::Function) {
                try {
                    if (status == winrt::Windows::Foundation::AsyncStatus::Completed) {
                        auto result = sender.GetResults();
                        auto external = Napi::External<LanguageModelResponseResult>::New(env, &result);
                        auto resultWrapper = MyLanguageModelResponseResult::constructor.New({ external });
                        deferred.Resolve(resultWrapper);
                    } else {
                        deferred.Reject(Napi::Error::New(env, "SummarizeParagraphAsync was cancelled or failed").Value());
                    }
                } catch (const winrt::hresult_error& ex) {
                    deferred.Reject(Napi::Error::New(env, winrt::to_string(ex.message())).Value());
                } catch (const std::exception& ex) {
                    deferred.Reject(Napi::Error::New(env, ex.what()).Value());
                } catch (...) {
                    deferred.Reject(Napi::Error::New(env, "Unknown error occurred in SummarizeParagraphAsync").Value());
                }
            });
        });
        
        return progressPromise.GetPromiseObject();
        
    } catch (const winrt::hresult_error& ex) {
        deferred.Reject(Napi::Error::New(env, winrt::to_string(ex.message())).Value());
        return progressPromise.GetPromiseObject();
    } catch (const std::exception& ex) {
        deferred.Reject(Napi::Error::New(env, ex.what()).Value());
        return progressPromise.GetPromiseObject();
    } catch (...) {
        deferred.Reject(Napi::Error::New(env, "Unknown error occurred in SummarizeParagraphAsync").Value());
        return progressPromise.GetPromiseObject();
    }
}

Napi::Value MyTextSummarizer::MySummarizeConversationAsync(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 2) {
        Napi::TypeError::New(env, "SummarizeConversationAsync requires messages array and options parameters").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    if (!info[0].IsArray()) {
        Napi::TypeError::New(env, "First parameter must be an array of ConversationItem objects").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    auto deferred = Napi::Promise::Deferred::New(env);
    auto tsfn = Napi::ThreadSafeFunction::New(env, Napi::Function::New(env, [](const Napi::CallbackInfo&) {}), "SummarizeConversationAsync", 0, 1);
    auto tsfn_guard = std::shared_ptr<void>(nullptr, [tsfn](void*) mutable { tsfn.Release(); });
    auto progressPromise = ProgressPromise::Create(env, deferred);
    auto progressTsfn = progressPromise.GetProgressTsfn();

    try {
        auto messagesArray = info[0].As<Napi::Array>();
        auto messages = winrt::single_threaded_vector<ConversationItem>();
        
        for (uint32_t i = 0; i < messagesArray.Length(); i++) {
            auto itemValue = messagesArray.Get(i);
            if (itemValue.IsObject()) {
                auto itemObj = itemValue.As<Napi::Object>();
                
                auto conversationItemWrapper = Napi::ObjectWrap<MyConversationItem>::Unwrap(itemObj);
                if (conversationItemWrapper) {
                    messages.Append(conversationItemWrapper->GetConversationItem());
                }
            }
        }
        
        ConversationSummaryOptions options;
        if (info[1].IsObject()) {
            auto optionsObj = info[1].As<Napi::Object>();
            if (optionsObj.Has("includeMessageCitations") && optionsObj.Get("includeMessageCitations").IsBoolean()) {
                options.IncludeMessageCitations(optionsObj.Get("includeMessageCitations").As<Napi::Boolean>().Value());
            }
            if (optionsObj.Has("includeParticipantAttribution") && optionsObj.Get("includeParticipantAttribution").IsBoolean()) {
                options.IncludeParticipantAttribution(optionsObj.Get("includeParticipantAttribution").As<Napi::Boolean>().Value());
            }
        }
        
        auto messagesView = messages.GetView();
        auto asyncOp = m_summarizer->SummarizeConversationAsync(messagesView, options);
        
        asyncOp.Progress([progressTsfn](auto const&, auto const& progressText) {
            if (progressTsfn && *progressTsfn) {
                auto progressStr = winrt::to_string(progressText);
                (*progressTsfn)->NonBlockingCall([progressStr](Napi::Env env, Napi::Function jsCallback) {
                    try {
                        jsCallback.Call({ env.Null(), Napi::String::New(env, progressStr) });
                    } catch (...) {}
                });
            }
        });
        
        asyncOp.Completed([deferred, tsfn, tsfn_guard](auto const& sender, auto const& status) mutable {
            tsfn.BlockingCall([deferred, sender, status](Napi::Env env, Napi::Function) {
                try {
                    if (status == winrt::Windows::Foundation::AsyncStatus::Completed) {
                        auto result = sender.GetResults();
                        auto external = Napi::External<LanguageModelResponseResult>::New(env, &result);
                        auto resultWrapper = MyLanguageModelResponseResult::constructor.New({ external });
                        deferred.Resolve(resultWrapper);
                    } else {
                        deferred.Reject(Napi::Error::New(env, "SummarizeConversationAsync was cancelled or failed").Value());
                    }
                } catch (const winrt::hresult_error& ex) {
                    deferred.Reject(Napi::Error::New(env, winrt::to_string(ex.message())).Value());
                } catch (const std::exception& ex) {
                    deferred.Reject(Napi::Error::New(env, ex.what()).Value());
                } catch (...) {
                    deferred.Reject(Napi::Error::New(env, "Unknown error occurred in SummarizeConversationAsync").Value());
                }
            });
        });
        
        return progressPromise.GetPromiseObject();
        
    } catch (const winrt::hresult_error& ex) {
        deferred.Reject(Napi::Error::New(env, winrt::to_string(ex.message())).Value());
        return progressPromise.GetPromiseObject();
    } catch (const std::exception& ex) {
        deferred.Reject(Napi::Error::New(env, ex.what()).Value());
        return progressPromise.GetPromiseObject();
    } catch (...) {
        deferred.Reject(Napi::Error::New(env, "Unknown error occurred in SummarizeConversationAsync").Value());
        return progressPromise.GetPromiseObject();
    }
}

Napi::Value MyTextSummarizer::MyIsPromptLargerThanContext(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1) {
        Napi::TypeError::New(env, "IsPromptLargerThanContext requires at least one parameter").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    try {
        uint64_t cutoffPosition = 0;
        bool result = false;
        
        if (info[0].IsArray() && info.Length() >= 2) {
            auto messagesArray = info[0].As<Napi::Array>();
            std::vector<ConversationItem> messages;
            
            for (uint32_t i = 0; i < messagesArray.Length(); i++) {
                auto itemValue = messagesArray.Get(i);
                if (itemValue.IsObject()) {
                    auto itemObj = itemValue.As<Napi::Object>();
                    
                    auto conversationItemWrapper = Napi::ObjectWrap<MyConversationItem>::Unwrap(itemObj);
                    if (conversationItemWrapper) {
                        messages.push_back(conversationItemWrapper->GetConversationItem());
                    }
                }
            }
            
            ConversationSummaryOptions options;
            if (info[1].IsObject()) {
                auto optionsObj = info[1].As<Napi::Object>();
                if (optionsObj.Has("includeMessageCitations") && optionsObj.Get("includeMessageCitations").IsBoolean()) {
                    options.IncludeMessageCitations(optionsObj.Get("includeMessageCitations").As<Napi::Boolean>().Value());
                }
                if (optionsObj.Has("includeParticipantAttribution") && optionsObj.Get("includeParticipantAttribution").IsBoolean()) {
                    options.IncludeParticipantAttribution(optionsObj.Get("includeParticipantAttribution").As<Napi::Boolean>().Value());
                }
            }
            
            winrt::array_view<ConversationItem const> messagesView(messages);
            result = m_summarizer->IsPromptLargerThanContext(messagesView, options, cutoffPosition);
        } 
        else {
            Napi::TypeError::New(env, "Invalid parameters. Expected either (string) or (ConversationItem[], ConversationSummaryOptions)").ThrowAsJavaScriptException();
            return env.Null();
        }
        
        auto resultObj = Napi::Object::New(env);
        resultObj.Set("isLarger", Napi::Boolean::New(env, result));
        resultObj.Set("cutoffPosition", Napi::BigInt::New(env, cutoffPosition));
        return resultObj;
        
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
        return env.Null();
    } catch (const std::exception& ex) {
        Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
        return env.Null();
    } catch (...) {
        Napi::Error::New(env, "Unknown error occurred in IsPromptLargerThanContext").ThrowAsJavaScriptException();
        return env.Null();
    }
}

// MyTextRewriter Implementation

Napi::Object MyTextRewriter::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "TextRewriter", {
        InstanceMethod("RewriteAsync", &MyTextRewriter::MyRewriteAsync)
    });

    constructor = Napi::Persistent(func);
    exports.Set("TextRewriter", func);
    return exports;
}

MyTextRewriter::MyTextRewriter(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyTextRewriter>(info) {
    if (info.Length() == 0) {
        Napi::Error::New(info.Env(), "TextRewriter constructor requires a LanguageModel instance").ThrowAsJavaScriptException();
        return;
    }
    
    // Check if first parameter is an External<TextRewriter> (for internal use)
    if (info[0].IsExternal()) {
        auto external = info[0].As<Napi::External<TextRewriter>>();
        m_rewriter = std::make_shared<TextRewriter>(*external.Data());
        return;
    }
    
    // Check if first parameter is a MyLanguageModel instance (for direct instantiation)
    if (info[0].IsObject()) {
        auto languageModelObj = info[0].As<Napi::Object>();
        auto languageModelWrapper = Napi::ObjectWrap<MyLanguageModel>::Unwrap(languageModelObj);
        if (languageModelWrapper) {
            try {
                auto languageModel = languageModelWrapper->GetLanguageModel();
                if (languageModel) {
                    m_rewriter = std::make_shared<TextRewriter>(*languageModel);
                    return;
                }
            } catch (const winrt::hresult_error& ex) {
                std::string errorMsg = "Failed to create TextRewriter: " + winrt::to_string(ex.message());
                Napi::Error::New(info.Env(), errorMsg).ThrowAsJavaScriptException();
                return;
            } catch (const std::exception& ex) {
                std::string errorMsg = "Failed to create TextRewriter: " + std::string(ex.what());
                Napi::Error::New(info.Env(), errorMsg).ThrowAsJavaScriptException();
                return;
            } catch (...) {
                Napi::Error::New(info.Env(), "Unknown error creating TextRewriter").ThrowAsJavaScriptException();
                return;
            }
        }
    }
    
    Napi::Error::New(info.Env(), "TextRewriter constructor requires a valid LanguageModel instance").ThrowAsJavaScriptException();
}

Napi::Value MyTextRewriter::MyRewriteAsync(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "RewriteAsync requires at least one string parameter").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    auto deferred = Napi::Promise::Deferred::New(env);
    auto tsfn = Napi::ThreadSafeFunction::New(env, Napi::Function::New(env, [](const Napi::CallbackInfo&) {}), "RewriteAsync", 0, 1);
    auto tsfn_guard = std::shared_ptr<void>(nullptr, [tsfn](void*) mutable { tsfn.Release(); });
    auto progressPromise = ProgressPromise::Create(env, deferred);
    auto progressTsfn = progressPromise.GetProgressTsfn();

    try {
        std::string text = info[0].As<Napi::String>().Utf8Value();
        winrt::hstring wText = winrt::to_hstring(text);
        
        // Determine which overload to use based on parameters
        auto asyncOp = [&]() {
            if (info.Length() >= 2) {
                // Debug: Check what type the second parameter is
                Napi::Value secondParam = info[1];
                
                // Check if second parameter is a number or can be converted to number
                if (secondParam.IsNumber()) {
                    // RewriteAsync(String, TextRewriteTone) overload
                    int32_t toneValue = secondParam.As<Napi::Number>().Int32Value();
                    TextRewriteTone tone = static_cast<TextRewriteTone>(toneValue);
                    return m_rewriter->RewriteAsync(wText, tone);
                } else {
                    throw std::runtime_error("Second parameter must be a TextRewriteTone value");
                }
            }
            // RewriteAsync(String) overload - uses default tone
            return m_rewriter->RewriteAsync(wText);
        }();
        
        asyncOp.Progress([progressTsfn](auto const&, auto const& progressText) {
            if (progressTsfn && *progressTsfn) {
                auto progressStr = winrt::to_string(progressText);
                (*progressTsfn)->NonBlockingCall([progressStr](Napi::Env env, Napi::Function jsCallback) {
                    try {
                        jsCallback.Call({ env.Null(), Napi::String::New(env, progressStr) });
                    } catch (...) {}
                });
            }
        });
        
        asyncOp.Completed([deferred, tsfn, tsfn_guard](auto const& sender, auto const& status) mutable {
            tsfn.BlockingCall([deferred, sender, status](Napi::Env env, Napi::Function) {
                try {
                    if (status == winrt::Windows::Foundation::AsyncStatus::Completed) {
                        auto result = sender.GetResults();
                        auto external = Napi::External<LanguageModelResponseResult>::New(env, &result);
                        auto resultWrapper = MyLanguageModelResponseResult::constructor.New({ external });
                        deferred.Resolve(resultWrapper);
                    } else {
                        deferred.Reject(Napi::Error::New(env, "RewriteAsync was cancelled or failed").Value());
                    }
                } catch (const winrt::hresult_error& ex) {
                    deferred.Reject(Napi::Error::New(env, winrt::to_string(ex.message())).Value());
                } catch (const std::exception& ex) {
                    deferred.Reject(Napi::Error::New(env, ex.what()).Value());
                } catch (...) {
                    deferred.Reject(Napi::Error::New(env, "Unknown error occurred in RewriteAsync").Value());
                }
            });
        });
        
        return progressPromise.GetPromiseObject();
        
    } catch (const winrt::hresult_error& ex) {
        deferred.Reject(Napi::Error::New(env, winrt::to_string(ex.message())).Value());
        return progressPromise.GetPromiseObject();
    } catch (const std::exception& ex) {
        deferred.Reject(Napi::Error::New(env, ex.what()).Value());
        return progressPromise.GetPromiseObject();
    } catch (...) {
        deferred.Reject(Napi::Error::New(env, "Unknown error occurred in RewriteAsync").Value());
        return progressPromise.GetPromiseObject();
    }
}

// MyTextToTableConverter Implementation
Napi::Object MyTextToTableConverter::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "TextToTableConverter", {
        InstanceMethod("ConvertAsync", &MyTextToTableConverter::MyConvertAsync)
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("TextToTableConverter", func);
    return exports;
}

MyTextToTableConverter::MyTextToTableConverter(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyTextToTableConverter>(info) {
    Napi::Env env = info.Env();
    
    if (info.Length() != 1 || !info[0].IsObject()) {
        Napi::TypeError::New(env, "TextToTableConverter constructor requires a LanguageModel object").ThrowAsJavaScriptException();
        return;
    }
    
    try {
        auto languageModelWrapper = Napi::ObjectWrap<MyLanguageModel>::Unwrap(info[0].As<Napi::Object>());
        auto languageModel = languageModelWrapper->GetLanguageModel();
        
        if (languageModel == nullptr) {
            Napi::Error::New(env, "Invalid LanguageModel object").ThrowAsJavaScriptException();
            return;
        }
        
        m_converter = std::make_shared<TextToTableConverter>(*languageModel);
        
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
    } catch (...) {
        Napi::Error::New(env, "Failed to create TextToTableConverter").ThrowAsJavaScriptException();
    }
}

Napi::Value MyTextToTableConverter::MyConvertAsync(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "ConvertAsync requires at least one string parameter").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    auto deferred = Napi::Promise::Deferred::New(env);
    auto tsfn = Napi::ThreadSafeFunction::New(env, Napi::Function::New(env, [](const Napi::CallbackInfo&) {}), "ConvertAsync", 0, 1);
    auto tsfn_guard = std::shared_ptr<void>(nullptr, [tsfn](void*) mutable { tsfn.Release(); });
    auto progressPromise = ProgressPromise::Create(env, deferred);
    auto progressTsfn = progressPromise.GetProgressTsfn();

    try {
        std::string text = info[0].As<Napi::String>().Utf8Value();
        winrt::hstring wText = winrt::to_hstring(text);
        
        // Call the ConvertAsync method
        auto asyncOp = m_converter->ConvertAsync(wText);
        
        asyncOp.Progress([progressTsfn](auto const&, auto const& progressText) {
            if (progressTsfn && *progressTsfn) {
                auto progressStr = winrt::to_string(progressText);
                (*progressTsfn)->NonBlockingCall([progressStr](Napi::Env env, Napi::Function jsCallback) {
                    try {
                        jsCallback.Call({ env.Null(), Napi::String::New(env, progressStr) });
                    } catch (...) {}
                });
            }
        });
        
        asyncOp.Completed([deferred, tsfn, tsfn_guard](auto const& sender, auto const& status) mutable {
            tsfn.BlockingCall([deferred, sender, status](Napi::Env env, Napi::Function) {
                try {
                    if (status == winrt::Windows::Foundation::AsyncStatus::Completed) {
                        auto result = sender.GetResults();
                        auto external = Napi::External<TextToTableResponseResult>::New(env, &result);
                        auto resultObj = MyTextToTableResponseResult::constructor.New({ external });
                        deferred.Resolve(resultObj);
                    } else {
                        deferred.Reject(Napi::Error::New(env, "ConvertAsync operation failed").Value());
                    }
                } catch (const winrt::hresult_error& ex) {
                    deferred.Reject(Napi::Error::New(env, winrt::to_string(ex.message())).Value());
                } catch (const std::exception& ex) {
                    deferred.Reject(Napi::Error::New(env, ex.what()).Value());
                } catch (...) {
                    deferred.Reject(Napi::Error::New(env, "Unknown error occurred in ConvertAsync completion").Value());
                }
            });
        });
        
        return progressPromise.GetPromiseObject();
        
    } catch (const winrt::hresult_error& ex) {
        deferred.Reject(Napi::Error::New(env, winrt::to_string(ex.message())).Value());
        return progressPromise.GetPromiseObject();
    } catch (const std::exception& ex) {
        deferred.Reject(Napi::Error::New(env, ex.what()).Value());
        return progressPromise.GetPromiseObject();
    } catch (...) {
        deferred.Reject(Napi::Error::New(env, "Unknown error occurred in ConvertAsync").Value());
        return progressPromise.GetPromiseObject();
    }
}

// MyTextToTableResponseResult Implementation
Napi::Object MyTextToTableResponseResult::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "TextToTableResponseResult", {
        InstanceAccessor("ExtendedError", &MyTextToTableResponseResult::GetExtendedError, nullptr),
        InstanceAccessor("Status", &MyTextToTableResponseResult::GetStatus, nullptr),
        InstanceMethod("GetRows", &MyTextToTableResponseResult::GetRows)
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("TextToTableResponseResult", func);
    return exports;
}

MyTextToTableResponseResult::MyTextToTableResponseResult(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyTextToTableResponseResult>(info) {
    Napi::Env env = info.Env();
    
    if (info.Length() == 1 && info[0].IsExternal()) {
        auto external = info[0].As<Napi::External<TextToTableResponseResult>>();
        m_result = *external.Data();
    }
}

bool MyTextToTableResponseResult::HasResult() const {
    return m_result.has_value();
}

Napi::Value MyTextToTableResponseResult::GetExtendedError(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (!HasResult()) {
        return env.Null();
    }
    
    try {
        auto extendedError = m_result.value().ExtendedError();
        return Napi::Number::New(env, static_cast<int32_t>(extendedError));
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
        return env.Null();
    }
}

Napi::Value MyTextToTableResponseResult::GetStatus(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (!HasResult()) {
        return env.Null();
    }
    
    try {
        auto status = m_result.value().Status();
        return Napi::Number::New(env, static_cast<int>(status));
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
        return env.Null();
    }
}

Napi::Value MyTextToTableResponseResult::GetRows(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (!HasResult()) {
        return env.Null();
    }
    
    try {
        auto rows = m_result.value().GetRows();
        auto jsArray = Napi::Array::New(env);
        
        uint32_t index = 0;
        for (auto const& row : rows) {
            auto external = Napi::External<TextToTableRow>::New(env, const_cast<TextToTableRow*>(&row));
            auto rowObj = MyTextToTableRow::constructor.New({ external });
            jsArray[index++] = rowObj;
        }
        
        return jsArray;
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
        return env.Null();
    }
}

// MyTextToTableRow Implementation
Napi::Object MyTextToTableRow::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "TextToTableRow", {
        InstanceMethod("GetColumns", &MyTextToTableRow::GetColumns)
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("TextToTableRow", func);
    return exports;
}

MyTextToTableRow::MyTextToTableRow(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyTextToTableRow>(info) {
    Napi::Env env = info.Env();
    
    if (info.Length() == 1 && info[0].IsExternal()) {
        auto external = info[0].As<Napi::External<TextToTableRow>>();
        m_row = *external.Data();
    }
}

bool MyTextToTableRow::HasRow() const {
    return m_row.has_value();
}

Napi::Value MyTextToTableRow::GetColumns(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (!HasRow()) {
        return env.Null();
    }
    
    try {
        auto columns = m_row.value().GetColumns();
        auto jsArray = Napi::Array::New(env);
        
        uint32_t index = 0;
        for (auto const& column : columns) {
            jsArray[index++] = Napi::String::New(env, winrt::to_string(column));
        }
        
        return jsArray;
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
        return env.Null();
    }
}