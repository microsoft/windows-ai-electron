#include "LanguageModelProjections.h"
#include "ContentSeverity.h"
#include "ProjectionHelper.h"
#include <shobjidl_core.h>
#include <windows.h>
#include <winrt/Windows.Data.Xml.Dom.h>
#include <cstdlib>

using namespace Windows::Data::Xml::Dom;

// Static member definitions
Napi::FunctionReference MyLanguageModelResponseResult::constructor;
Napi::FunctionReference MyAIFeatureReadyResult::constructor;
Napi::FunctionReference MyLanguageModel::constructor;

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
        return Napi::String::New(env, winrt::to_string(errorDisplayText));
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