#include <napi.h>
#include <shobjidl_core.h>
#include <windows.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Data.Xml.Dom.h>
#include <winrt/Microsoft.Windows.AI.Text.h>
#include <cstdlib>

using namespace winrt;
using namespace Microsoft::Windows::AI;
using namespace Microsoft::Windows::AI::Text;
using namespace Windows::Data::Xml::Dom;

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
        try {
            auto languageModel = LanguageModel::CreateAsync().get();
            if (!languageModel) {
                Napi::Error::New(env, "Failed to create LanguageModel instance.").ThrowAsJavaScriptException();
                return env.Null();
            }
            auto persistentModel = std::make_shared<LanguageModel>(std::move(languageModel));
            auto external = Napi::External<std::shared_ptr<LanguageModel>>::New(env, 
                new std::shared_ptr<LanguageModel>(persistentModel),
                [](Napi::Env env, std::shared_ptr<LanguageModel>* data) {
                    delete data;
                });
            
            auto constructor = env.GetInstanceData<Napi::FunctionReference>();
            return constructor->New({ external });
            
        } catch (const winrt::hresult_error& ex) {
            Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
            return env.Null();
        } catch (const std::exception& ex) {
            Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
            return env.Null();
        } catch (...) {
            Napi::Error::New(env, "Unknown error occurred in CreateAsync").ThrowAsJavaScriptException();
            return env.Null();
        }
    }

    static Napi::Value MyGetReadyState(const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
        auto readyState = LanguageModel::GetReadyState();
        return Napi::Number::New(env, static_cast<int>(readyState));
    }

    static void MyEnsureReadyAsync(const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
        try {
            LanguageModel::EnsureReadyAsync().get();
        } catch (const winrt::hresult_error& ex) {
            Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
        } catch (const std::exception& ex) {
            Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
        } catch (...) {
            Napi::Error::New(env, "Unknown error occurred in EnsureReadyAsync").ThrowAsJavaScriptException();
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
        try {
            std::string prompt = info[0].As<Napi::String>();
            if (prompt.empty()) {
                Napi::Error::New(env, "The prompt cannot be empty. Please provide a valid prompt.").ThrowAsJavaScriptException();
                return env.Null();
            }
            auto responseResult = m_languagemodel->GenerateResponseAsync(winrt::to_hstring(prompt)).get();
            auto result = winrt::to_string(responseResult.Text());
            return Napi::String::New(env, result);
        } catch (const winrt::hresult_error& ex) {
            Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
            return env.Null();
        } catch (const std::exception& ex) {
            Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
            return env.Null();
        } catch (...) {
            Napi::Error::New(env, "Unknown error occurred in GenerateResponseAsync").ThrowAsJavaScriptException();
            return env.Null();
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
    
    return MyLanguageModel::Init(env, exports);
}

NODE_API_MODULE(addon, Init)