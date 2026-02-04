#include "ImagingProjections.h"
#include "LanguageModelProjections.h"
#include "ProjectionHelper.h"
#include "ContentSeverity.h"
#include <shobjidl_core.h>
#include <windows.h>
#include <winrt/Windows.Data.Xml.Dom.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Graphics.Imaging.h>
#include <winrt/Microsoft.Graphics.Imaging.h>
#include <cstdlib>
#include <thread>

using namespace Windows::Data::Xml::Dom;

// Static member definitions
Napi::FunctionReference MyImageDescriptionResult::constructor;
Napi::FunctionReference MyRecognizedText::constructor;
Napi::FunctionReference MyRecognizedLine::constructor;
Napi::FunctionReference MyRecognizedWord::constructor;
Napi::FunctionReference MyRecognizedTextBoundingBox::constructor;
Napi::FunctionReference MyImageDescriptionGenerator::constructor;
Napi::FunctionReference MyTextRecognizer::constructor;

// MyImageDescriptionResult Implementation
Napi::Object MyImageDescriptionResult::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "ImageDescriptionResult", {
        InstanceAccessor("Description", &MyImageDescriptionResult::GetDescription, nullptr),
        InstanceAccessor("Status", &MyImageDescriptionResult::GetStatus, nullptr)
    });

    constructor = Napi::Persistent(func);
    exports.Set("ImageDescriptionResult", func);
    return exports;
}

MyImageDescriptionResult::MyImageDescriptionResult(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyImageDescriptionResult>(info) {
    if (info.Length() == 1 && info[0].IsExternal()) {
        auto external = info[0].As<Napi::External<ImageDescriptionResult>>();
        m_result = *external.Data();
    }
}

bool MyImageDescriptionResult::HasResult() const {
    return m_result.has_value();
}

Napi::Value MyImageDescriptionResult::GetDescription(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        if (!m_result.has_value()) {
            Napi::Error::New(env, "No result data available").ThrowAsJavaScriptException();
            return env.Null();
        }
        auto description = m_result->Description();
        return Napi::String::New(env, winrt::to_string(description));
    } catch (const winrt::hresult_error& ex) {
        std::string errorMsg = "WinRT error getting description: " + winrt::to_string(ex.message());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (const std::exception& ex) {
        std::string errorMsg = "Error getting description: " + std::string(ex.what());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (...) {
        Napi::Error::New(env, "Unknown error getting description").ThrowAsJavaScriptException();
        return env.Null();
    }
}

Napi::Value MyImageDescriptionResult::GetStatus(const Napi::CallbackInfo& info) {
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

void MyImageDescriptionResult::SetResult(const ImageDescriptionResult& result) {
    m_result = result;
}

// MyImageDescriptionGenerator Implementation
Napi::Object MyImageDescriptionGenerator::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "ImageDescriptionGenerator", {
        InstanceMethod("DescribeAsync", &MyImageDescriptionGenerator::MyDescribeAsync),
        InstanceMethod("Close", &MyImageDescriptionGenerator::MyClose),
        StaticMethod("CreateAsync", &MyImageDescriptionGenerator::MyCreateAsync),
        StaticMethod("GetReadyState", &MyImageDescriptionGenerator::MyGetReadyState),
        StaticMethod("EnsureReadyAsync", &MyImageDescriptionGenerator::MyEnsureReadyAsync)
    });

    constructor = Napi::Persistent(func);
    exports.Set("ImageDescriptionGenerator", func);
    return exports;
}

Napi::Value MyImageDescriptionGenerator::MyCreateAsync(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    auto deferred = Napi::Promise::Deferred::New(env);
    auto tsfn = Napi::ThreadSafeFunction::New(
        env,
        Napi::Function::New(env, [](const Napi::CallbackInfo&) {}),
        "CreateAsync",
        0,
        1 
    );

    auto tsfn_guard = std::shared_ptr<void>(nullptr, [tsfn](void*) mutable { tsfn.Release(); });
    
    try {
        auto asyncOp = ImageDescriptionGenerator::CreateAsync();
        
        auto completionHandler = [deferred, tsfn, tsfn_guard](auto const& sender, auto const& status) mutable {
            auto callback = [deferred, sender, status](Napi::Env env, Napi::Function) {
                try {
                    if (status == winrt::Windows::Foundation::AsyncStatus::Completed) {
                        auto generator = sender.GetResults();
                        if (!generator) {
                            deferred.Reject(Napi::Error::New(env, "Failed to create ImageDescriptionGenerator instance.").Value());
                            return;
                        }
                        
                        auto persistentGenerator = std::make_shared<ImageDescriptionGenerator>(std::move(generator));
                        auto external = Napi::External<ImageDescriptionGenerator>::New(env, 
                            persistentGenerator.get(),
                            [persistentGenerator](Napi::Env env, ImageDescriptionGenerator* data) {
                                // persistentGenerator will be destroyed here, releasing the WinRT object
                            });

                        auto instance = MyImageDescriptionGenerator::constructor.New({ external });
                        deferred.Resolve(instance);
                    } else {
                        deferred.Reject(Napi::Error::New(env, "ImageDescriptionGenerator creation was cancelled or failed.").Value());
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

Napi::Value MyImageDescriptionGenerator::MyGetReadyState(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    auto readyState = ImageDescriptionGenerator::GetReadyState();
    return Napi::Number::New(env, static_cast<int>(readyState));
}

Napi::Value MyImageDescriptionGenerator::MyEnsureReadyAsync(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    auto deferred = Napi::Promise::Deferred::New(env);
    auto tsfn = Napi::ThreadSafeFunction::New(
        env,
        Napi::Function::New(env, [](const Napi::CallbackInfo&) {}),
        "EnsureReadyAsync",
        0,
        1 
    );

    auto tsfn_guard = std::shared_ptr<void>(nullptr, [tsfn](void*) mutable { tsfn.Release(); });
    auto progressPromise = ProgressPromise::Create(env, deferred);
    auto progressTsfn = progressPromise.GetProgressTsfn();
    
    try {
        auto asyncOp = ImageDescriptionGenerator::EnsureReadyAsync();
        
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

MyImageDescriptionGenerator::MyImageDescriptionGenerator(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyImageDescriptionGenerator>(info) {
    if (info.Length() == 0 || !info[0].IsExternal()) {
        Napi::Error::New(info.Env(), "Cannot instantiate ImageDescriptionGenerator directly. Use ImageDescriptionGenerator.CreateAsync()").ThrowAsJavaScriptException();
        return;
    }
    
    auto external = info[0].As<Napi::External<ImageDescriptionGenerator>>();
    m_generator = external.Data();
}

Napi::Value MyImageDescriptionGenerator::MyDescribeAsync(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 3) {
        Napi::TypeError::New(env, "DescribeAsync requires filePath, descriptionKind, and contentFilterOptions parameters").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "First parameter must be a string (file path)").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    if (!info[1].IsNumber()) {
        Napi::TypeError::New(env, "Second parameter must be of type ImageDescriptionKind. Make sure parameter is a valid ImageDescriptionKind value.").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    if (!info[2].IsObject()) {
        Napi::TypeError::New(env, "Third parameter must be a ContentFilterOptions instance").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto deferred = Napi::Promise::Deferred::New(env);
    auto tsfn = Napi::ThreadSafeFunction::New(env, Napi::Function::New(env, [](const Napi::CallbackInfo&) {}), "DescribeAsync", 0, 1);
    auto tsfn_guard = std::shared_ptr<void>(nullptr, [tsfn](void*) mutable { tsfn.Release(); });
    
    auto progressPromise = ProgressPromise::Create(env, deferred);
    auto progressTsfn = progressPromise.GetProgressTsfn();

    try {
        std::string filePath = info[0].As<Napi::String>().Utf8Value();
        int32_t descriptionKind = info[1].As<Napi::Number>().Int32Value();
        
        // Extract ContentFilterOptions from the wrapper class
        auto contentFilterOptionsObj = info[2].As<Napi::Object>();
        auto contentFilterOptionsInstance = Napi::ObjectWrap<MyContentFilterOptions>::Unwrap(contentFilterOptionsObj);
        auto contentFilterOptions = contentFilterOptionsInstance->GetOptions();
        
        // Convert file path to Windows string
        std::wstring wFilePath(filePath.begin(), filePath.end());
        
        // Create async operation on background thread
        std::thread([deferred, tsfn, tsfn_guard, progressTsfn, wFilePath, descriptionKind, contentFilterOptions, generator = m_generator]() {
            try {
                auto storageFile = Windows::Storage::StorageFile::GetFileFromPathAsync(wFilePath).get();
                auto stream = storageFile.OpenAsync(Windows::Storage::FileAccessMode::Read).get();
                auto decoder = Windows::Graphics::Imaging::BitmapDecoder::CreateAsync(stream).get();
                auto softwareBitmap = decoder.GetSoftwareBitmapAsync().get();
                auto imageBuffer = Microsoft::Graphics::Imaging::ImageBuffer::CreateForSoftwareBitmap(softwareBitmap);

                ImageDescriptionKind kind = static_cast<ImageDescriptionKind>(descriptionKind);
                
                auto asyncOp = generator->DescribeAsync(imageBuffer, kind, contentFilterOptions);
                
                // Set up progress callback
                asyncOp.Progress([progressTsfn](auto const&, auto const& progressValue) {
                    if (progressTsfn && *progressTsfn) {
                        (*progressTsfn)->NonBlockingCall([progressValue](Napi::Env env, Napi::Function jsCallback) {
                            try {
                                jsCallback.Call({ env.Null(), Napi::String::New(env, winrt::to_string(progressValue)) });
                            } catch (...) {}
                        });
                    }
                });
                
                auto result = asyncOp.get();
                
                tsfn.BlockingCall([deferred, result](Napi::Env env, Napi::Function) {
                    auto resultObj = MyImageDescriptionResult::constructor.New({});
                    auto resultInstance = Napi::ObjectWrap<MyImageDescriptionResult>::Unwrap(resultObj);
                    resultInstance->SetResult(result);
                    deferred.Resolve(resultObj);
                });
                
            } catch (const winrt::hresult_error& ex) {
                tsfn.BlockingCall([deferred, message = winrt::to_string(ex.message())](Napi::Env env, Napi::Function) {
                    deferred.Reject(Napi::Error::New(env, message).Value());
                });
            } catch (const std::exception& ex) {
                tsfn.BlockingCall([deferred, message = std::string(ex.what())](Napi::Env env, Napi::Function) {
                    deferred.Reject(Napi::Error::New(env, message).Value());
                });
            } catch (...) {
                tsfn.BlockingCall([deferred](Napi::Env env, Napi::Function) {
                    deferred.Reject(Napi::Error::New(env, "Unknown error occurred in DescribeAsync").Value());
                });
            }
        }).detach();
        
        return progressPromise.GetPromiseObject();
        
    } catch (const winrt::hresult_error& ex) {
        deferred.Reject(Napi::Error::New(env, winrt::to_string(ex.message())).Value());
        return progressPromise.GetPromiseObject();
    } catch (const std::exception& ex) {
        deferred.Reject(Napi::Error::New(env, ex.what()).Value());
        return progressPromise.GetPromiseObject();
    } catch (...) {
        deferred.Reject(Napi::Error::New(env, "Unknown error occurred in DescribeAsync").Value());
        return progressPromise.GetPromiseObject();
    }
}

Napi::Value MyImageDescriptionGenerator::MyClose(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        if (m_generator) {
            m_generator->Close();
        }
        return env.Undefined();
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
        return env.Null();
    } catch (const std::exception& ex) {
        Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
        return env.Null();
    } catch (...) {
        Napi::Error::New(env, "Unknown error occurred in Close").ThrowAsJavaScriptException();
        return env.Null();
    }
}

// MyTextRecognizer Implementation
Napi::Object MyTextRecognizer::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "TextRecognizer", {
        InstanceMethod("RecognizeTextFromImageAsync", &MyTextRecognizer::MyRecognizeTextFromImageAsync),
        InstanceMethod("RecognizeTextFromImage", &MyTextRecognizer::MyRecognizeTextFromImage),
        InstanceMethod("Close", &MyTextRecognizer::MyClose),
        InstanceMethod("Dispose", &MyTextRecognizer::MyDispose),
        StaticMethod("CreateAsync", &MyTextRecognizer::MyCreateAsync),
        StaticMethod("GetReadyState", &MyTextRecognizer::MyGetReadyState),
        StaticMethod("EnsureReadyAsync", &MyTextRecognizer::MyEnsureReadyAsync)
    });

    constructor = Napi::Persistent(func);
    exports.Set("TextRecognizer", func);
    return exports;
}

Napi::Value MyTextRecognizer::MyCreateAsync(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    auto deferred = Napi::Promise::Deferred::New(env);
    auto tsfn = Napi::ThreadSafeFunction::New(
        env,
        Napi::Function::New(env, [](const Napi::CallbackInfo&) {}),
        "CreateAsync",
        0,
        1 
    );

    auto tsfn_guard = std::shared_ptr<void>(nullptr, [tsfn](void*) mutable { tsfn.Release(); });
    
    try {
        auto asyncOp = TextRecognizer::CreateAsync();
        
        auto completionHandler = [deferred, tsfn, tsfn_guard](auto const& sender, auto const& status) mutable {
            auto callback = [deferred, sender, status](Napi::Env env, Napi::Function) {
                try {
                    if (status == winrt::Windows::Foundation::AsyncStatus::Completed) {
                        auto recognizer = sender.GetResults();
                        if (!recognizer) {
                            deferred.Reject(Napi::Error::New(env, "Failed to create TextRecognizer instance.").Value());
                            return;
                        }
                        
                        auto persistentRecognizer = std::make_shared<TextRecognizer>(std::move(recognizer));
                        auto external = Napi::External<TextRecognizer>::New(env, 
                            persistentRecognizer.get(),
                            [persistentRecognizer](Napi::Env env, TextRecognizer* data) {
                                // persistentRecognizer will be destroyed here, releasing the WinRT object
                            });

                        auto instance = MyTextRecognizer::constructor.New({ external });
                        deferred.Resolve(instance);
                    } else {
                        deferred.Reject(Napi::Error::New(env, "TextRecognizer creation was cancelled or failed.").Value());
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

Napi::Value MyTextRecognizer::MyGetReadyState(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    auto readyState = TextRecognizer::GetReadyState();
    return Napi::Number::New(env, static_cast<int>(readyState));
}

Napi::Value MyTextRecognizer::MyEnsureReadyAsync(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    auto deferred = Napi::Promise::Deferred::New(env);
    auto tsfn = Napi::ThreadSafeFunction::New(
        env,
        Napi::Function::New(env, [](const Napi::CallbackInfo&) {}),
        "EnsureReadyAsync",
        0,
        1 
    );

    auto tsfn_guard = std::shared_ptr<void>(nullptr, [tsfn](void*) mutable { tsfn.Release(); });
    auto progressPromise = ProgressPromise::Create(env, deferred);
    auto progressTsfn = progressPromise.GetProgressTsfn();
    
    try {
        auto asyncOp = TextRecognizer::EnsureReadyAsync();
        
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

MyTextRecognizer::MyTextRecognizer(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyTextRecognizer>(info) {
    if (info.Length() == 0 || !info[0].IsExternal()) {
        Napi::Error::New(info.Env(), "Cannot instantiate TextRecognizer directly. Use TextRecognizer.CreateAsync()").ThrowAsJavaScriptException();
        return;
    }
    
    auto external = info[0].As<Napi::External<TextRecognizer>>();
    m_recognizer = external.Data();
}

Napi::Value MyTextRecognizer::MyRecognizeTextFromImageAsync(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1) {
        Napi::TypeError::New(env, "RecognizeTextFromImageAsync requires filePath parameter").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "First parameter must be a string (file path)").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    auto deferred = Napi::Promise::Deferred::New(env);
    auto tsfn = Napi::ThreadSafeFunction::New(env, Napi::Function::New(env, [](const Napi::CallbackInfo&) {}), "RecognizeTextFromImageAsync", 0, 1);
    auto tsfn_guard = std::shared_ptr<void>(nullptr, [tsfn](void*) mutable { tsfn.Release(); });

    try {
        std::string filePath = info[0].As<Napi::String>().Utf8Value();
        
        // Convert file path to Windows string
        std::wstring wFilePath(filePath.begin(), filePath.end());
        
        // Create async operation on background thread
        std::thread([deferred, tsfn, tsfn_guard, wFilePath, recognizer = m_recognizer]() {
            try {
                // Load the image file as a StorageFile
                auto storageFile = Windows::Storage::StorageFile::GetFileFromPathAsync(wFilePath).get();
                
                // Open the file for reading
                auto stream = storageFile.OpenAsync(Windows::Storage::FileAccessMode::Read).get();
                
                // Create a BitmapDecoder to decode the image
                auto decoder = Windows::Graphics::Imaging::BitmapDecoder::CreateAsync(stream).get();
                
                // Get the SoftwareBitmap
                auto softwareBitmap = decoder.GetSoftwareBitmapAsync().get();
                
                // Create ImageBuffer from SoftwareBitmap
                auto imageBuffer = Microsoft::Graphics::Imaging::ImageBuffer::CreateForSoftwareBitmap(softwareBitmap);
                
                // Call the actual Windows AI RecognizeTextFromImageAsync function
                auto asyncOp = recognizer->RecognizeTextFromImageAsync(imageBuffer);
                auto result = asyncOp.get();
                
                // Return result on main thread
                tsfn.BlockingCall([deferred, result](Napi::Env env, Napi::Function) {
                    auto resultObj = MyRecognizedText::constructor.New({});
                    auto resultInstance = Napi::ObjectWrap<MyRecognizedText>::Unwrap(resultObj);
                    resultInstance->SetResult(result);
                    deferred.Resolve(resultObj);
                });
                
            } catch (const winrt::hresult_error& ex) {
                tsfn.BlockingCall([deferred, message = winrt::to_string(ex.message())](Napi::Env env, Napi::Function) {
                    deferred.Reject(Napi::Error::New(env, message).Value());
                });
            } catch (const std::exception& ex) {
                tsfn.BlockingCall([deferred, message = std::string(ex.what())](Napi::Env env, Napi::Function) {
                    deferred.Reject(Napi::Error::New(env, message).Value());
                });
            } catch (...) {
                tsfn.BlockingCall([deferred](Napi::Env env, Napi::Function) {
                    deferred.Reject(Napi::Error::New(env, "Unknown error occurred in RecognizeTextFromImageAsync").Value());
                });
            }
        }).detach();
        
        return deferred.Promise();
        
    } catch (const winrt::hresult_error& ex) {
        deferred.Reject(Napi::Error::New(env, winrt::to_string(ex.message())).Value());
        return deferred.Promise();
    } catch (const std::exception& ex) {
        deferred.Reject(Napi::Error::New(env, ex.what()).Value());
        return deferred.Promise();
    } catch (...) {
        deferred.Reject(Napi::Error::New(env, "Unknown error occurred in RecognizeTextFromImageAsync").Value());
        return deferred.Promise();
    }
}

Napi::Value MyTextRecognizer::MyRecognizeTextFromImage(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1) {
        Napi::TypeError::New(env, "RecognizeTextFromImage requires filePath parameter").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "First parameter must be a string (file path)").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    try {
        std::string filePath = info[0].As<Napi::String>().Utf8Value();
        std::wstring wFilePath(filePath.begin(), filePath.end());
        auto storageFile = Windows::Storage::StorageFile::GetFileFromPathAsync(wFilePath).get();
        auto stream = storageFile.OpenAsync(Windows::Storage::FileAccessMode::Read).get();
        auto decoder = Windows::Graphics::Imaging::BitmapDecoder::CreateAsync(stream).get();
        auto softwareBitmap = decoder.GetSoftwareBitmapAsync().get();
        auto imageBuffer = Microsoft::Graphics::Imaging::ImageBuffer::CreateForSoftwareBitmap(softwareBitmap);
        
        auto result = m_recognizer->RecognizeTextFromImage(imageBuffer);
        
        auto resultObj = MyRecognizedText::constructor.New({});
        auto resultInstance = Napi::ObjectWrap<MyRecognizedText>::Unwrap(resultObj);
        resultInstance->SetResult(result);
        return resultObj;
        
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
        return env.Null();
    } catch (const std::exception& ex) {
        Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
        return env.Null();
    } catch (...) {
        Napi::Error::New(env, "Unknown error occurred in RecognizeTextFromImage").ThrowAsJavaScriptException();
        return env.Null();
    }
}

Napi::Value MyTextRecognizer::MyClose(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        if (m_recognizer) {
            m_recognizer->Close();
        }
        return env.Undefined();
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
        return env.Null();
    } catch (const std::exception& ex) {
        Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
        return env.Null();
    } catch (...) {
        Napi::Error::New(env, "Unknown error occurred in Close").ThrowAsJavaScriptException();
        return env.Null();
    }
}

Napi::Value MyTextRecognizer::MyDispose(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        if (m_recognizer) {
            // Note: In WinRT C++, Dispose() might not be directly available as a method
            // The underlying object implements IDisposable, but we typically use Close() or reset the pointer
            // For proper resource cleanup, we'll call Close() and clear the pointer
            m_recognizer->Close();
            m_recognizer = nullptr;
        }
        return env.Undefined();
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
        return env.Null();
    } catch (const std::exception& ex) {
        Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
        return env.Null();
    } catch (...) {
        Napi::Error::New(env, "Unknown error occurred in Dispose").ThrowAsJavaScriptException();
        return env.Null();
    }
}

// MyRecognizedText Implementation
Napi::Object MyRecognizedText::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "RecognizedText", {
        InstanceAccessor("Lines", &MyRecognizedText::GetLines, nullptr),
        InstanceAccessor("TextAngle", &MyRecognizedText::GetTextAngle, nullptr)
    });

    constructor = Napi::Persistent(func);
    exports.Set("RecognizedText", func);
    return exports;
}

MyRecognizedText::MyRecognizedText(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyRecognizedText>(info) {
    if (info.Length() == 1 && info[0].IsExternal()) {
        auto external = info[0].As<Napi::External<RecognizedText>>();
        m_result = *external.Data();
    }
}

bool MyRecognizedText::HasResult() const {
    return m_result.has_value();
}

Napi::Value MyRecognizedText::GetLines(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        if (!m_result.has_value()) {
            Napi::Error::New(env, "No result data available").ThrowAsJavaScriptException();
            return env.Null();
        }
        auto lines = m_result->Lines();
        auto array = Napi::Array::New(env, lines.size());
        
        for (uint32_t i = 0; i < lines.size(); i++) {
            auto line = lines[i];
            auto external = Napi::External<RecognizedLine>::New(env, &line);
            auto lineWrapper = MyRecognizedLine::constructor.New({ external });
            array.Set(i, lineWrapper);
        }
        
        return array;
    } catch (const winrt::hresult_error& ex) {
        std::string errorMsg = "WinRT error getting lines: " + winrt::to_string(ex.message());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (const std::exception& ex) {
        std::string errorMsg = "Error getting lines: " + std::string(ex.what());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (...) {
        Napi::Error::New(env, "Unknown error getting lines").ThrowAsJavaScriptException();
        return env.Null();
    }
}

Napi::Value MyRecognizedText::GetTextAngle(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        if (!m_result.has_value()) {
            Napi::Error::New(env, "No result data available").ThrowAsJavaScriptException();
            return env.Null();
        }
        
        // Get the actual TextAngle property from the Windows AI API
        auto angle = m_result->TextAngle();
        return Napi::Number::New(env, angle);
    } catch (const winrt::hresult_error& ex) {
        std::string errorMsg = "WinRT error getting text angle: " + winrt::to_string(ex.message());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (const std::exception& ex) {
        std::string errorMsg = "Error getting text angle: " + std::string(ex.what());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (...) {
        Napi::Error::New(env, "Unknown error getting text angle").ThrowAsJavaScriptException();
        return env.Null();
    }
}

void MyRecognizedText::SetResult(const RecognizedText& result) {
    m_result = result;
}

// MyRecognizedLine Implementation
Napi::Object MyRecognizedLine::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "RecognizedLine", {
        InstanceAccessor("BoundingBox", &MyRecognizedLine::GetBoundingBox, nullptr),
        InstanceAccessor("Style", &MyRecognizedLine::GetStyle, nullptr),
        InstanceAccessor("LineStyleConfidence", &MyRecognizedLine::GetLineStyleConfidence, nullptr),
        InstanceAccessor("Text", &MyRecognizedLine::GetText, nullptr),
        InstanceAccessor("Words", &MyRecognizedLine::GetWords, nullptr)
    });

    constructor = Napi::Persistent(func);
    exports.Set("RecognizedLine", func);
    return exports;
}

MyRecognizedLine::MyRecognizedLine(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyRecognizedLine>(info) {
    if (info.Length() == 1 && info[0].IsExternal()) {
        auto external = info[0].As<Napi::External<RecognizedLine>>();
        m_result = *external.Data();
    }
}

bool MyRecognizedLine::HasResult() const {
    return m_result.has_value();
}

Napi::Value MyRecognizedLine::GetBoundingBox(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        if (!m_result.has_value()) {
            Napi::Error::New(env, "No result data available").ThrowAsJavaScriptException();
            return env.Null();
        }
        auto boundingBox = m_result->BoundingBox();
        auto external = Napi::External<RecognizedTextBoundingBox>::New(env, &boundingBox);
        return MyRecognizedTextBoundingBox::constructor.New({ external });
    } catch (const winrt::hresult_error& ex) {
        std::string errorMsg = "WinRT error getting bounding box: " + winrt::to_string(ex.message());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (const std::exception& ex) {
        std::string errorMsg = "Error getting bounding box: " + std::string(ex.what());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (...) {
        Napi::Error::New(env, "Unknown error getting bounding box").ThrowAsJavaScriptException();
        return env.Null();
    }
}

Napi::Value MyRecognizedLine::GetStyle(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        if (!m_result.has_value()) {
            Napi::Error::New(env, "No result data available").ThrowAsJavaScriptException();
            return env.Null();
        }
        auto style = m_result->Style();
        return Napi::Number::New(env, static_cast<int>(style));
    } catch (const winrt::hresult_error& ex) {
        std::string errorMsg = "WinRT error getting style: " + winrt::to_string(ex.message());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (const std::exception& ex) {
        std::string errorMsg = "Error getting style: " + std::string(ex.what());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (...) {
        Napi::Error::New(env, "Unknown error getting style").ThrowAsJavaScriptException();
        return env.Null();
    }
}

Napi::Value MyRecognizedLine::GetLineStyleConfidence(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        if (!m_result.has_value()) {
            Napi::Error::New(env, "No result data available").ThrowAsJavaScriptException();
            return env.Null();
        }
        auto confidence = m_result->LineStyleConfidence();
        return Napi::Number::New(env, confidence);
    } catch (const winrt::hresult_error& ex) {
        std::string errorMsg = "WinRT error getting line style confidence: " + winrt::to_string(ex.message());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (const std::exception& ex) {
        std::string errorMsg = "Error getting line style confidence: " + std::string(ex.what());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (...) {
        Napi::Error::New(env, "Unknown error getting line style confidence").ThrowAsJavaScriptException();
        return env.Null();
    }
}

Napi::Value MyRecognizedLine::GetText(const Napi::CallbackInfo& info) {
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

Napi::Value MyRecognizedLine::GetWords(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        if (!m_result.has_value()) {
            Napi::Error::New(env, "No result data available").ThrowAsJavaScriptException();
            return env.Null();
        }
        auto words = m_result->Words();
        auto array = Napi::Array::New(env, words.size());
        
        for (uint32_t i = 0; i < words.size(); i++) {
            auto word = words[i];
            auto external = Napi::External<RecognizedWord>::New(env, &word);
            auto wordWrapper = MyRecognizedWord::constructor.New({ external });
            array.Set(i, wordWrapper);
        }
        
        return array;
    } catch (const winrt::hresult_error& ex) {
        std::string errorMsg = "WinRT error getting words: " + winrt::to_string(ex.message());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (const std::exception& ex) {
        std::string errorMsg = "Error getting words: " + std::string(ex.what());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (...) {
        Napi::Error::New(env, "Unknown error getting words").ThrowAsJavaScriptException();
        return env.Null();
    }
}

// MyRecognizedWord Implementation
Napi::Object MyRecognizedWord::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "RecognizedWord", {
        InstanceAccessor("BoundingBox", &MyRecognizedWord::GetBoundingBox, nullptr),
        InstanceAccessor("MatchConfidence", &MyRecognizedWord::GetConfidence, nullptr),
        InstanceAccessor("Text", &MyRecognizedWord::GetText, nullptr)
    });

    constructor = Napi::Persistent(func);
    exports.Set("RecognizedWord", func);
    return exports;
}

MyRecognizedWord::MyRecognizedWord(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyRecognizedWord>(info) {
    if (info.Length() == 1 && info[0].IsExternal()) {
        auto external = info[0].As<Napi::External<RecognizedWord>>();
        m_result = *external.Data();
    }
}

bool MyRecognizedWord::HasResult() const {
    return m_result.has_value();
}

Napi::Value MyRecognizedWord::GetBoundingBox(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        if (!m_result.has_value()) {
            Napi::Error::New(env, "No result data available").ThrowAsJavaScriptException();
            return env.Null();
        }
        auto boundingBox = m_result->BoundingBox();
        auto external = Napi::External<RecognizedTextBoundingBox>::New(env, &boundingBox);
        return MyRecognizedTextBoundingBox::constructor.New({ external });
    } catch (const winrt::hresult_error& ex) {
        std::string errorMsg = "WinRT error getting bounding box: " + winrt::to_string(ex.message());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (const std::exception& ex) {
        std::string errorMsg = "Error getting bounding box: " + std::string(ex.what());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (...) {
        Napi::Error::New(env, "Unknown error getting bounding box").ThrowAsJavaScriptException();
        return env.Null();
    }
}

Napi::Value MyRecognizedWord::GetConfidence(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        if (!m_result.has_value()) {
            Napi::Error::New(env, "No result data available").ThrowAsJavaScriptException();
            return env.Null();
        }
        
        // Get the actual MatchConfidence property from the Windows AI API
        auto confidence = m_result->MatchConfidence();
        return Napi::Number::New(env, confidence);
    } catch (const winrt::hresult_error& ex) {
        std::string errorMsg = "WinRT error getting match confidence: " + winrt::to_string(ex.message());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (const std::exception& ex) {
        std::string errorMsg = "Error getting match confidence: " + std::string(ex.what());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (...) {
        Napi::Error::New(env, "Unknown error getting match confidence").ThrowAsJavaScriptException();
        return env.Null();
    }
}

Napi::Value MyRecognizedWord::GetText(const Napi::CallbackInfo& info) {
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

// MyRecognizedTextBoundingBox Implementation
Napi::Object MyRecognizedTextBoundingBox::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "RecognizedTextBoundingBox", {
        InstanceAccessor("TopLeft", &MyRecognizedTextBoundingBox::GetTopLeft, nullptr),
        InstanceAccessor("TopRight", &MyRecognizedTextBoundingBox::GetTopRight, nullptr),
        InstanceAccessor("BottomLeft", &MyRecognizedTextBoundingBox::GetBottomLeft, nullptr),
        InstanceAccessor("BottomRight", &MyRecognizedTextBoundingBox::GetBottomRight, nullptr)
    });

    constructor = Napi::Persistent(func);
    exports.Set("RecognizedTextBoundingBox", func);
    return exports;
}

MyRecognizedTextBoundingBox::MyRecognizedTextBoundingBox(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyRecognizedTextBoundingBox>(info) {
    if (info.Length() == 1 && info[0].IsExternal()) {
        auto external = info[0].As<Napi::External<RecognizedTextBoundingBox>>();
        m_result = *external.Data();
    }
}

bool MyRecognizedTextBoundingBox::HasResult() const {
    return m_result.has_value();
}

Napi::Value MyRecognizedTextBoundingBox::GetTopLeft(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        if (!m_result.has_value()) {
            Napi::Error::New(env, "No result data available").ThrowAsJavaScriptException();
            return env.Null();
        }
        
        auto topLeft = m_result->TopLeft;
        auto pointObj = Napi::Object::New(env);
        pointObj.Set("X", Napi::Number::New(env, topLeft.X));
        pointObj.Set("Y", Napi::Number::New(env, topLeft.Y));
        return pointObj;
    } catch (const winrt::hresult_error& ex) {
        std::string errorMsg = "WinRT error getting TopLeft: " + winrt::to_string(ex.message());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (const std::exception& ex) {
        std::string errorMsg = "Error getting TopLeft: " + std::string(ex.what());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (...) {
        Napi::Error::New(env, "Unknown error getting TopLeft").ThrowAsJavaScriptException();
        return env.Null();
    }
}

Napi::Value MyRecognizedTextBoundingBox::GetTopRight(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        if (!m_result.has_value()) {
            Napi::Error::New(env, "No result data available").ThrowAsJavaScriptException();
            return env.Null();
        }
        
        auto topRight = m_result->TopRight;
        auto pointObj = Napi::Object::New(env);
        pointObj.Set("X", Napi::Number::New(env, topRight.X));
        pointObj.Set("Y", Napi::Number::New(env, topRight.Y));
        return pointObj;
    } catch (const winrt::hresult_error& ex) {
        std::string errorMsg = "WinRT error getting TopRight: " + winrt::to_string(ex.message());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (const std::exception& ex) {
        std::string errorMsg = "Error getting TopRight: " + std::string(ex.what());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (...) {
        Napi::Error::New(env, "Unknown error getting TopRight").ThrowAsJavaScriptException();
        return env.Null();
    }
}

Napi::Value MyRecognizedTextBoundingBox::GetBottomLeft(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        if (!m_result.has_value()) {
            Napi::Error::New(env, "No result data available").ThrowAsJavaScriptException();
            return env.Null();
        }
        
        auto bottomLeft = m_result->BottomLeft;
        auto pointObj = Napi::Object::New(env);
        pointObj.Set("X", Napi::Number::New(env, bottomLeft.X));
        pointObj.Set("Y", Napi::Number::New(env, bottomLeft.Y));
        return pointObj;
    } catch (const winrt::hresult_error& ex) {
        std::string errorMsg = "WinRT error getting BottomLeft: " + winrt::to_string(ex.message());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (const std::exception& ex) {
        std::string errorMsg = "Error getting BottomLeft: " + std::string(ex.what());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (...) {
        Napi::Error::New(env, "Unknown error getting BottomLeft").ThrowAsJavaScriptException();
        return env.Null();
    }
}

Napi::Value MyRecognizedTextBoundingBox::GetBottomRight(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        if (!m_result.has_value()) {
            Napi::Error::New(env, "No result data available").ThrowAsJavaScriptException();
            return env.Null();
        }
        
        auto bottomRight = m_result->BottomRight;
        auto pointObj = Napi::Object::New(env);
        pointObj.Set("X", Napi::Number::New(env, bottomRight.X));
        pointObj.Set("Y", Napi::Number::New(env, bottomRight.Y));
        return pointObj;
    } catch (const winrt::hresult_error& ex) {
        std::string errorMsg = "WinRT error getting BottomRight: " + winrt::to_string(ex.message());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (const std::exception& ex) {
        std::string errorMsg = "Error getting BottomRight: " + std::string(ex.what());
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    } catch (...) {
        Napi::Error::New(env, "Unknown error getting BottomRight").ThrowAsJavaScriptException();
        return env.Null();
    }
}

// Placeholder implementations for the remaining classes
// These would need to be fully implemented based on the actual API usage patterns

Napi::Object MyImageObjectExtractor::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "ImageObjectExtractor", {
        InstanceMethod("ExtractAsync", &MyImageObjectExtractor::MyExtractAsync),
        InstanceMethod("Close", &MyImageObjectExtractor::MyClose),
        StaticMethod("CreateAsync", &MyImageObjectExtractor::MyCreateAsync),
        StaticMethod("GetReadyState", &MyImageObjectExtractor::MyGetReadyState),
        StaticMethod("EnsureReadyAsync", &MyImageObjectExtractor::MyEnsureReadyAsync)
    });
    exports.Set("ImageObjectExtractor", func);
    return exports;
}

Napi::Object MyImageObjectExtractorHint::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "ImageObjectExtractorHint", {
        InstanceAccessor("X", &MyImageObjectExtractorHint::GetX, &MyImageObjectExtractorHint::SetX),
        InstanceAccessor("Y", &MyImageObjectExtractorHint::GetY, &MyImageObjectExtractorHint::SetY),
        InstanceAccessor("Width", &MyImageObjectExtractorHint::GetWidth, &MyImageObjectExtractorHint::SetWidth),
        InstanceAccessor("Height", &MyImageObjectExtractorHint::GetHeight, &MyImageObjectExtractorHint::SetHeight)
    });
    exports.Set("ImageObjectExtractorHint", func);
    return exports;
}

Napi::Object MyImageObjectRemover::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "ImageObjectRemover", {
        InstanceMethod("RemoveAsync", &MyImageObjectRemover::MyRemoveAsync),
        InstanceMethod("Close", &MyImageObjectRemover::MyClose),
        StaticMethod("CreateAsync", &MyImageObjectRemover::MyCreateAsync),
        StaticMethod("GetReadyState", &MyImageObjectRemover::MyGetReadyState),
        StaticMethod("EnsureReadyAsync", &MyImageObjectRemover::MyEnsureReadyAsync)
    });
    exports.Set("ImageObjectRemover", func);
    return exports;
}

Napi::Object MyImageScaler::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "ImageScaler", {
        InstanceMethod("ScaleAsync", &MyImageScaler::MyScaleAsync),
        InstanceMethod("Close", &MyImageScaler::MyClose),
        StaticMethod("CreateAsync", &MyImageScaler::MyCreateAsync),
        StaticMethod("GetReadyState", &MyImageScaler::MyGetReadyState),
        StaticMethod("EnsureReadyAsync", &MyImageScaler::MyEnsureReadyAsync)
    });
    exports.Set("ImageScaler", func);
    return exports;
}

// Placeholder method implementations for incomplete classes
// These methods need to be implemented to avoid linker errors

// MyImageObjectExtractor placeholder methods
MyImageObjectExtractor::MyImageObjectExtractor(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyImageObjectExtractor>(info) {
    // Placeholder constructor
}

Napi::Value MyImageObjectExtractor::MyCreateAsync(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    auto deferred = Napi::Promise::Deferred::New(env);
    deferred.Reject(Napi::Error::New(env, "ImageObjectExtractor.CreateAsync not yet implemented").Value());
    return deferred.Promise();
}

Napi::Value MyImageObjectExtractor::MyGetReadyState(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        auto readyState = ImageObjectExtractor::GetReadyState();
        return Napi::Number::New(env, static_cast<int>(readyState));
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
        return env.Null();
    } catch (const std::exception& ex) {
        Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
        return env.Null();
    } catch (...) {
        Napi::Error::New(env, "Unknown error occurred in GetReadyState").ThrowAsJavaScriptException();
        return env.Null();
    }
}

Napi::Value MyImageObjectExtractor::MyEnsureReadyAsync(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    auto deferred = Napi::Promise::Deferred::New(env);
    deferred.Reject(Napi::Error::New(env, "ImageObjectExtractor.EnsureReadyAsync not yet implemented").Value());
    return deferred.Promise();
}

Napi::Value MyImageObjectExtractor::MyExtractAsync(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    auto deferred = Napi::Promise::Deferred::New(env);
    deferred.Reject(Napi::Error::New(env, "ImageObjectExtractor.ExtractAsync not yet implemented").Value());
    return deferred.Promise();
}

Napi::Value MyImageObjectExtractor::MyClose(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    return env.Undefined();
}

// MyImageObjectExtractorHint placeholder methods
MyImageObjectExtractorHint::MyImageObjectExtractorHint(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyImageObjectExtractorHint>(info) {
    // Note: ImageObjectExtractorHint may not have a default constructor
    // This is a placeholder implementation
}

Napi::Value MyImageObjectExtractorHint::GetX(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), 0);
}

void MyImageObjectExtractorHint::SetX(const Napi::CallbackInfo& info, const Napi::Value& value) {
    // Placeholder setter
}

Napi::Value MyImageObjectExtractorHint::GetY(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), 0);
}

void MyImageObjectExtractorHint::SetY(const Napi::CallbackInfo& info, const Napi::Value& value) {
    // Placeholder setter
}

Napi::Value MyImageObjectExtractorHint::GetWidth(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), 0);
}

void MyImageObjectExtractorHint::SetWidth(const Napi::CallbackInfo& info, const Napi::Value& value) {
    // Placeholder setter
}

Napi::Value MyImageObjectExtractorHint::GetHeight(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), 0);
}

void MyImageObjectExtractorHint::SetHeight(const Napi::CallbackInfo& info, const Napi::Value& value) {
    // Placeholder setter
}

// MyImageObjectRemover placeholder methods
MyImageObjectRemover::MyImageObjectRemover(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyImageObjectRemover>(info) {
    if (info.Length() == 0 || !info[0].IsExternal()) {
        Napi::Error::New(info.Env(), "Cannot instantiate ImageObjectRemover directly. Use ImageObjectRemover.CreateAsync()").ThrowAsJavaScriptException();
        return;
    }
    
    auto external = info[0].As<Napi::External<ImageObjectRemover>>();
    m_remover = external.Data();
}

Napi::Value MyImageObjectRemover::MyCreateAsync(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    auto deferred = Napi::Promise::Deferred::New(env);
    deferred.Reject(Napi::Error::New(env, "ImageObjectRemover.CreateAsync not yet implemented").Value());
    return deferred.Promise();
}

Napi::Value MyImageObjectRemover::MyGetReadyState(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        auto readyState = ImageObjectRemover::GetReadyState();
        return Napi::Number::New(env, static_cast<int>(readyState));
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
        return env.Null();
    } catch (const std::exception& ex) {
        Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
        return env.Null();
    } catch (...) {
        Napi::Error::New(env, "Unknown error occurred in GetReadyState").ThrowAsJavaScriptException();
        return env.Null();
    }
}

Napi::Value MyImageObjectRemover::MyEnsureReadyAsync(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    auto deferred = Napi::Promise::Deferred::New(env);
    deferred.Reject(Napi::Error::New(env, "ImageObjectRemover.EnsureReadyAsync not yet implemented").Value());
    return deferred.Promise();
}

Napi::Value MyImageObjectRemover::MyRemoveAsync(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    auto deferred = Napi::Promise::Deferred::New(env);
    deferred.Reject(Napi::Error::New(env, "ImageObjectRemover.RemoveAsync not yet implemented").Value());
    return deferred.Promise();
}

Napi::Value MyImageObjectRemover::MyClose(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    return env.Undefined();
}

MyImageScaler::MyImageScaler(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyImageScaler>(info) {
    if (info.Length() == 0 || !info[0].IsExternal()) {
        Napi::Error::New(info.Env(), "Cannot instantiate ImageScaler directly. Use ImageScaler.CreateAsync()").ThrowAsJavaScriptException();
        return;
    }
    
    auto external = info[0].As<Napi::External<ImageScaler>>();
    m_scaler = external.Data();
}

Napi::Value MyImageScaler::MyCreateAsync(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    auto deferred = Napi::Promise::Deferred::New(env);
    deferred.Reject(Napi::Error::New(env, "ImageScaler.CreateAsync not yet implemented").Value());
    return deferred.Promise();
}

Napi::Value MyImageScaler::MyGetReadyState(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        auto readyState = ImageScaler::GetReadyState();
        return Napi::Number::New(env, static_cast<int>(readyState));
    } catch (const winrt::hresult_error& ex) {
        Napi::Error::New(env, winrt::to_string(ex.message())).ThrowAsJavaScriptException();
        return env.Null();
    } catch (const std::exception& ex) {
        Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
        return env.Null();
    } catch (...) {
        Napi::Error::New(env, "Unknown error occurred in GetReadyState").ThrowAsJavaScriptException();
        return env.Null();
    }
}

Napi::Value MyImageScaler::MyEnsureReadyAsync(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    auto deferred = Napi::Promise::Deferred::New(env);
    deferred.Reject(Napi::Error::New(env, "ImageScaler.EnsureReadyAsync not yet implemented").Value());
    return deferred.Promise();
}

Napi::Value MyImageScaler::MyScaleAsync(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    auto deferred = Napi::Promise::Deferred::New(env);
    deferred.Reject(Napi::Error::New(env, "ImageScaler.ScaleAsync not yet implemented").Value());
    return deferred.Promise();
}

Napi::Value MyImageScaler::MyClose(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    return env.Undefined();
}