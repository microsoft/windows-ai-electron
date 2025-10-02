#include <napi.h>
#include <shobjidl_core.h>
#include <windows.h>

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
        auto tsfn = Napi::ThreadSafeFunction::New(
            env,
            Napi::Function::New(env, [](const Napi::CallbackInfo&) {}),
            "GenerateResponseAsync",
            0,
            1 
        );
        auto tsfn_guard = std::shared_ptr<void>(nullptr, [tsfn](void*) mutable { tsfn.Release(); });
        auto progressTsfn = std::make_shared<Napi::ThreadSafeFunction*>(nullptr);

        try {
            auto asyncOp = [&]() {
                if (info.Length() >= 2 && info[1].IsObject()) {
                    try {
                        auto optionsObj = info[1].As<Napi::Object>();
                        auto optionsWrapper = Napi::ObjectWrap<MyLanguageModelOptions>::Unwrap(optionsObj);
                        if (!optionsWrapper) {
                            Napi::TypeError::New(env, "Failed to unwrap LanguageModelOptions").ThrowAsJavaScriptException();
                        }
                        auto options = optionsWrapper->GetOptions();
                        return m_languagemodel->GenerateResponseAsync(winrt::to_hstring(prompt), options);
                    } catch (...) {
                        Napi::TypeError::New(env, "Second parameter must be a LanguageModelOptions instance").ThrowAsJavaScriptException();
                    }
                } else {
                    return m_languagemodel->GenerateResponseAsync(winrt::to_hstring(prompt));
                }
            }();

            asyncOp.Progress([progressTsfn](auto const& sender, auto const& progressText) {
                if (progressTsfn && *progressTsfn) {
                    auto progressStr = winrt::to_string(progressText);
                    
                    (*progressTsfn)->NonBlockingCall([progressStr](Napi::Env env, Napi::Function jsCallback) {
                        try {
                            jsCallback.Call({ env.Null(), Napi::String::New(env, progressStr) });
                        } catch (const std::exception& e) {
                            printf("Progress callback error: %s\n", e.what());
                        }
                    });
                }
            });
            
            asyncOp.Completed([deferred, tsfn, tsfn_guard](auto const& sender, auto const& status) mutable {
                auto callback = [deferred, sender, status](Napi::Env env, Napi::Function) {
                    try {
                        if (status == winrt::Windows::Foundation::AsyncStatus::Completed) {
                            auto result = winrt::to_string(sender.GetResults().Text());
                            deferred.Resolve(Napi::String::New(env, result));
                        } else {
                            deferred.Reject(Napi::Error::New(env, "GenerateResponseAsync was cancelled or failed.").Value());
                        }
                    } catch (const winrt::hresult_error& ex) {
                        deferred.Reject(Napi::Error::New(env, winrt::to_string(ex.message())).Value());
                    } catch (const std::exception& ex) {
                        deferred.Reject(Napi::Error::New(env, ex.what()).Value());
                    } catch (...) {
                        deferred.Reject(Napi::Error::New(env, "Unknown error occurred in GenerateResponseAsync completion").Value());
                    }
                };
                
                tsfn.BlockingCall(callback);
            });
            
            auto promiseObj = Napi::Object::New(env);
            promiseObj.Set("_promise", deferred.Promise());
            promiseObj.Set("_progressTsfn", Napi::External<std::shared_ptr<Napi::ThreadSafeFunction*>>::New(env, 
                new std::shared_ptr<Napi::ThreadSafeFunction*>(progressTsfn),
                [](Napi::Env env, std::shared_ptr<Napi::ThreadSafeFunction*>* data) {
                    delete data;
                }));
            auto thenFunction = Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
                auto env = info.Env();
                auto self = info.This().As<Napi::Object>();
                auto promise = self.Get("_promise");
                auto thenMethod = promise.As<Napi::Object>().Get("then").As<Napi::Function>();
                
                std::vector<napi_value> args;
                for (size_t i = 0; i < info.Length(); i++) {
                    args.push_back(info[i]);
                }
                
                return thenMethod.Call(promise, args);
            });
            promiseObj.Set("then", thenFunction);
            auto catchFunction = Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
                auto env = info.Env();
                auto self = info.This().As<Napi::Object>();
                auto promise = self.Get("_promise");
                auto catchMethod = promise.As<Napi::Object>().Get("catch").As<Napi::Function>();
                
                std::vector<napi_value> args;
                for (size_t i = 0; i < info.Length(); i++) {
                    args.push_back(info[i]);
                }
                
                return catchMethod.Call(promise, args);
            });
            promiseObj.Set("catch", catchFunction);
            auto progressFunction = Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                auto env = info.Env();
                auto self = info.This().As<Napi::Object>();
                
                if (info.Length() < 1 || !info[0].IsFunction()) {
                    Napi::TypeError::New(env, "Expected callback function").ThrowAsJavaScriptException();
                    return env.Undefined();
                }
                
                auto callback = info[0].As<Napi::Function>();
                
                // Get the progress callback storage
                auto progressTsfnExternal = self.Get("_progressTsfn").As<Napi::External<std::shared_ptr<Napi::ThreadSafeFunction*>>>();
                auto progressTsfnPtr = progressTsfnExternal.Data();
                
                // Create the ThreadSafeFunction for this progress callback
                **progressTsfnPtr = new Napi::ThreadSafeFunction(
                    Napi::ThreadSafeFunction::New(
                        env,
                        callback,
                        "GenerateResponseProgress",
                        0,
                        1
                    )
                );
                
                return self; // Return self for chaining
            });
            promiseObj.Set("progress", progressFunction);

            return promiseObj;
            
        } catch (const winrt::hresult_error& ex) {
            auto errorPromise = Napi::Promise::Deferred::New(env);
            errorPromise.Reject(Napi::Error::New(env, winrt::to_string(ex.message())).Value());
            return errorPromise.Promise();
        } catch (const std::exception& ex) {
            auto errorPromise = Napi::Promise::Deferred::New(env);
            errorPromise.Reject(Napi::Error::New(env, ex.what()).Value());
            return errorPromise.Promise();
        } catch (...) {
            auto errorPromise = Napi::Promise::Deferred::New(env);
            errorPromise.Reject(Napi::Error::New(env, "Unknown error occurred in GenerateResponseAsync").Value());
            return errorPromise.Promise();
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
    
    exports = MyLanguageModel::Init(env, exports);
    return MyLanguageModelOptions::Init(env, exports);
}

NODE_API_MODULE(addon, Init)