#include <napi.h>
#include <shobjidl_core.h>
#include <windows.h>
#include <optional>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Data.Xml.Dom.h>
#include <winrt/Microsoft.Windows.AI.Text.h>
#include <winrt/Microsoft.Windows.AI.ContentSafety.h>
#include <cstdlib>

using namespace winrt;
using namespace Microsoft::Windows::AI;
using namespace Microsoft::Windows::AI::Text;
using namespace Microsoft::Windows::AI::ContentSafety;
using namespace Windows::Data::Xml::Dom;

// Helper class for Promise-like object with progress support
class ProgressPromise {
private:
    Napi::Object m_object;
    std::shared_ptr<Napi::ThreadSafeFunction*> m_progressTsfn;

public:
    static ProgressPromise Create(Napi::Env env, Napi::Promise::Deferred deferred) {
        return ProgressPromise(env, deferred);
    }

    ProgressPromise(Napi::Env env, Napi::Promise::Deferred deferred) {
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

    Napi::Object GetObject() const {
        return m_object;
    }

    std::shared_ptr<Napi::ThreadSafeFunction*> GetProgressTsfn() const {
        return m_progressTsfn;
    }
};

class MyLanguageModelResponseResult : public Napi::ObjectWrap<MyLanguageModelResponseResult> {
public:
    static Napi::FunctionReference constructor;

    static Napi::Object Init(Napi::Env env, Napi::Object exports) {
        Napi::Function func = DefineClass(env, "LanguageModelResponseResult", {
            InstanceAccessor("Text", &MyLanguageModelResponseResult::GetText, nullptr),
            InstanceAccessor("Status", &MyLanguageModelResponseResult::GetStatus, nullptr),
            InstanceAccessor("ExtendedError", &MyLanguageModelResponseResult::GetExtendedError, nullptr)
        });

        constructor = Napi::Persistent(func);
        exports.Set("LanguageModelResponseResult", func);
        return exports;
    }

    MyLanguageModelResponseResult(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyLanguageModelResponseResult>(info) {
        if (info.Length() == 1 && info[0].IsExternal()) {
            auto external = info[0].As<Napi::External<LanguageModelResponseResult>>();
            m_result = *external.Data();
        }
        // If no external data, m_result remains empty (std::nullopt)
    }

    bool HasResult() const {
        return m_result.has_value();
    }

private:
    std::optional<LanguageModelResponseResult> m_result;

    Napi::Value GetText(const Napi::CallbackInfo& info) {
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

    Napi::Value GetStatus(const Napi::CallbackInfo& info) {
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

    Napi::Value GetExtendedError(const Napi::CallbackInfo& info) {
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
            // workaround to get error message from HRESULT, code may need to be adjusted based on actual error handling needs
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
};

class MyLanguageModelOptions : public Napi::ObjectWrap<MyLanguageModelOptions> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports) {
        Napi::Function func = DefineClass(env, "LanguageModelOptions", {
            InstanceAccessor("temperature", &MyLanguageModelOptions::GetTemperature, &MyLanguageModelOptions::SetTemperature),
            InstanceAccessor("topK", &MyLanguageModelOptions::GetTopK, &MyLanguageModelOptions::SetTopK),
            InstanceAccessor("topP", &MyLanguageModelOptions::GetTopP, &MyLanguageModelOptions::SetTopP),
            InstanceAccessor("contentFilterOptions", &MyLanguageModelOptions::GetContentFilterOptions, &MyLanguageModelOptions::SetContentFilterOptions)
        });

        exports.Set("LanguageModelOptions", func);
        return exports;
    }

    MyLanguageModelOptions(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyLanguageModelOptions>(info) {
        m_options = LanguageModelOptions();
    }

    LanguageModelOptions GetOptions() const {
        return m_options;
    }

private:
    LanguageModelOptions m_options;

    Napi::Value GetTemperature(const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
        try {
            float temperature = m_options.Temperature();
            return Napi::Number::New(env, temperature);
        } catch (const winrt::hresult_error& ex) {
            Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
            return env.Null();
        }
    }

    void SetTemperature(const Napi::CallbackInfo& info, const Napi::Value& value) {
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

    Napi::Value GetTopK(const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
        try {
            uint32_t topK = m_options.TopK();
            return Napi::Number::New(env, topK);
        } catch (const winrt::hresult_error& ex) {
            Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
            return env.Null();
        }
    }

    void SetTopK(const Napi::CallbackInfo& info, const Napi::Value& value) {
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

    Napi::Value GetTopP(const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
        try {
            float topP = m_options.TopP();
            return Napi::Number::New(env, topP);
        } catch (const winrt::hresult_error& ex) {
            Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
            return env.Null();
        }
    }

    void SetTopP(const Napi::CallbackInfo& info, const Napi::Value& value) {
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

    Napi::Value GetContentFilterOptions(const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
        try {
            auto contentFilter = m_options.ContentFilterOptions();
            if (contentFilter == nullptr) {
                return env.Null();
            }
            // For now, return null as we'd need to implement a wrapper for ContentFilterOptions
            // This can be expanded later if needed
            return env.Null();
        } catch (const winrt::hresult_error& ex) {
            Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
            return env.Null();
        }
    }

    void SetContentFilterOptions(const Napi::CallbackInfo& info, const Napi::Value& value) {
        Napi::Env env = info.Env();
        try {
            if (value.IsNull() || value.IsUndefined()) {
                m_options.ContentFilterOptions(nullptr);
                return;
            }
            // For now, we'll accept null/undefined values
            // This can be expanded later to accept ContentFilterOptions objects
            Napi::TypeError::New(env, "ContentFilterOptions setting not yet implemented").ThrowAsJavaScriptException();
        } catch (const winrt::hresult_error& ex) {
            Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
        }
    }
};

// Static member definitions
Napi::FunctionReference MyLanguageModelResponseResult::constructor;

class MyLanguageModel : public Napi::ObjectWrap<MyLanguageModel> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports) {
        Napi::Function func = DefineClass(env, "LanguageModel", {
            InstanceMethod("GenerateResponseAsync", &MyLanguageModel::MyGenerateResponseAsync),
            StaticMethod("CreateAsync", &MyLanguageModel::MyCreateAsync),
            StaticMethod("GetReadyState", &MyLanguageModel::MyGetReadyState),
            StaticMethod("EnsureReadyAsync", &MyLanguageModel::MyEnsureReadyAsync)
        });

        Napi::FunctionReference* constructor = new Napi::FunctionReference();
        *constructor = Napi::Persistent(func);
        env.SetInstanceData(constructor);

        exports.Set("LanguageModel", func);
        return exports;
    }

    static Napi::Value MyCreateAsync(const Napi::CallbackInfo& info) {
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
                            auto external = Napi::External<std::shared_ptr<LanguageModel>>::New(env, 
                                new std::shared_ptr<LanguageModel>(persistentModel),
                                [](Napi::Env env, std::shared_ptr<LanguageModel>* data) {
                                    delete data;
                                });
                            
                            auto constructor = env.GetInstanceData<Napi::FunctionReference>();
                            if (!constructor) {
                                deferred.Reject(Napi::Error::New(env, "Constructor not found in instance data").Value());
                                return;
                            }
                            auto instance = constructor->New({ external });
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

    static Napi::Value MyGetReadyState(const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
        auto readyState = LanguageModel::GetReadyState();
        return Napi::Number::New(env, static_cast<int>(readyState));
    }

    static Napi::Value MyEnsureReadyAsync(const Napi::CallbackInfo& info) {
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
        
        try {
            auto asyncOp = LanguageModel::EnsureReadyAsync();
            
            auto completionHandler = [deferred, tsfn, tsfn_guard](auto const& sender, auto const& status) mutable {
                auto callback = [deferred, sender, status](Napi::Env env, Napi::Function) {
                    try {
                        if (status == winrt::Windows::Foundation::AsyncStatus::Completed) {
                            deferred.Resolve(Napi::Boolean::New(env, true)); // need to fix
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
            return deferred.Promise();
            
        } catch (const winrt::hresult_error& ex) {
            deferred.Reject(Napi::Error::New(env, winrt::to_string(ex.message())).Value());
            return deferred.Promise();
        } catch (const std::exception& ex) {
            deferred.Reject(Napi::Error::New(env, ex.what()).Value());
            return deferred.Promise();
        } catch (...) {
            deferred.Reject(Napi::Error::New(env, "Unknown error occurred in EnsureReadyAsync").Value());
            return deferred.Promise();
        }
    }

    MyLanguageModel(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyLanguageModel>(info) {
        if (info.Length() == 0 || !info[0].IsExternal()) {
            Napi::Error::New(info.Env(), "Cannot instantiate LanguageModel directly. Use LanguageModel.CreateAsync()").ThrowAsJavaScriptException();
            return;
        }
        
        auto external = info[0].As<Napi::External<std::shared_ptr<LanguageModel>>>();
        m_languagemodel = *external.Data();
    }

private:
    std::shared_ptr<LanguageModel> m_languagemodel;
    
    Napi::Value MyGenerateResponseAsync(const Napi::CallbackInfo& info) {
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

            return progressPromise.GetObject();
            
        } catch (const winrt::hresult_error& ex) {
            deferred.Reject(Napi::Error::New(env, winrt::to_string(ex.message())).Value());
            return progressPromise.GetObject();
        } catch (const std::exception& ex) {
            deferred.Reject(Napi::Error::New(env, ex.what()).Value());
            return progressPromise.GetObject();
        } catch (...) {
            deferred.Reject(Napi::Error::New(env, "Unknown error occurred in EnsureReadyAsync").Value());
            return progressPromise.GetObject();
        }
    }
};

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    Napi::Object aiFeatureReadyState = Napi::Object::New(env);
    aiFeatureReadyState.Set("Ready", Napi::Number::New(env, 0));
    aiFeatureReadyState.Set("NotReady", Napi::Number::New(env, 1));
    aiFeatureReadyState.Set("NotSupportedOnCurrentSystem", Napi::Number::New(env, 2));
    aiFeatureReadyState.Set("DisabledByUser", Napi::Number::New(env, 3));
    
    exports.Set("AIFeatureReadyState", aiFeatureReadyState);
    
    // Add LanguageModelResponseStatus enum
    Napi::Object responseStatus = Napi::Object::New(env);
    responseStatus.Set("Complete", Napi::Number::New(env, 0));
    responseStatus.Set("InProgress", Napi::Number::New(env, 1));
    responseStatus.Set("BlockedByPolicy", Napi::Number::New(env, 2));
    responseStatus.Set("PromptLargerThanContext", Napi::Number::New(env, 3));
    responseStatus.Set("PromptBlockedByContentModeration", Napi::Number::New(env, 4));
    responseStatus.Set("ResponseBlockedByContentModeration", Napi::Number::New(env, 5));
    responseStatus.Set("Error", Napi::Number::New(env, 6));
    
    exports.Set("LanguageModelResponseStatus", responseStatus);
    
    exports = MyLanguageModel::Init(env, exports);
    exports = MyLanguageModelResponseResult::Init(env, exports);
    return MyLanguageModelOptions::Init(env, exports);
}

NODE_API_MODULE(addon, Init)