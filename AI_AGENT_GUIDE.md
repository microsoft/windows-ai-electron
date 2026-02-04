# AI Agent Development Guide

## @microsoft/windows-ai-electron Repository

> **Purpose**: This guide provides AI agents with comprehensive context about the repository structure, coding patterns, architectural decisions, and development practices to ensure consistent and high-quality contributions.

---

## 📋 Repository Overview

### **Project Mission**

This is a Node.js native addon that wraps Microsoft Windows AI APIs, enabling Electron applications to access local AI capabilities with hardware acceleration. The addon provides JavaScript bindings for:

- **Language Models** (Microsoft.Windows.AI.Text namespace)
- **Content Safety** (Content filtering and moderation)
- **Imaging APIs** (Microsoft.Windows.AI.Imaging namespace)
  - Image description generation
  - Optical Character Recognition (OCR)
  - Object removal and image scaling (future)

### **Target Environment**

- **Platform**: Windows 11 (22H2+)
- **Hardware**: Copilot+ PC recommended
- **Runtime**: Node.js 18.x+, Electron applications
- **Build Tools**: Visual Studio 2022, node-gyp, Windows SDK

---

## 🏗️ Architecture Patterns

### **1. N-API C++ Addon Structure**

#### **Core Pattern: External<> Wrapper**

```cpp
// ✅ CORRECT: Always use External<> for WinRT object lifetime management
MyLanguageModel::MyLanguageModel(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyLanguageModel>(info) {
    if (info.Length() == 0 || !info[0].IsExternal()) {
        Napi::Error::New(info.Env(), "Cannot instantiate LanguageModel directly. Use LanguageModel.CreateAsync()").ThrowAsJavaScriptException();
        return;
    }

    auto external = info[0].As<Napi::External<LanguageModel>>();
    m_model = external.Data();
}

// ❌ WRONG: Never allow direct instantiation
MyLanguageModel::MyLanguageModel(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyLanguageModel>(info) {
    // No validation - this would cause crashes
}
```

#### **Static Constructor Pattern**

```cpp
// ✅ CORRECT: Always use this pattern for async creation
Napi::Value MyLanguageModel::MyCreateAsync(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    auto deferred = Napi::Promise::Deferred::New(env);
    auto tsfn = Napi::ThreadSafeFunction::New(env, ...);

    // Create persistent shared_ptr for lifetime management
    auto persistentModel = std::make_shared<LanguageModel>(std::move(model));
    auto external = Napi::External<LanguageModel>::New(env,
        persistentModel.get(),
        [persistentModel](Napi::Env env, LanguageModel* data) {
            // Automatic cleanup when External is destroyed
        });

    auto instance = MyLanguageModel::constructor.New({ external });
    deferred.Resolve(instance);
}
```

### **2. Progress Promise Pattern**

#### **For IAsyncOperationWithProgress<T, U> Operations**

```cpp
// ✅ CORRECT: Use ProgressPromise for streaming operations
Napi::Value MyLanguageModel::MyGenerateResponseAsync(const Napi::CallbackInfo& info) {
    auto progressPromise = ProgressPromise::Create(env, deferred);
    auto progressTsfn = progressPromise.GetProgressTsfn();

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

    return progressPromise.GetPromiseObject();
}
```

### **3. Parameter Validation Pattern**

#### **Always Validate Parameters**

```cpp
// ✅ CORRECT: Comprehensive parameter validation
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
        Napi::TypeError::New(env, "Second parameter must be of type ImageDescriptionKind").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[2].IsObject()) {
        Napi::TypeError::New(env, "Third parameter must be a ContentFilterOptions instance").ThrowAsJavaScriptException();
        return env.Null();
    }

    // Extract wrapper objects using Unwrap pattern
    auto contentFilterOptionsObj = info[2].As<Napi::Object>();
    auto contentFilterOptionsInstance = Napi::ObjectWrap<MyContentFilterOptions>::Unwrap(contentFilterOptionsObj);
    auto contentFilterOptions = contentFilterOptionsInstance->GetOptions();
}
```

---

## 🔒 Critical Rules & Anti-Patterns

### **❌ NEVER DO**

1. **Direct WinRT Object Instantiation in Constructors**

   ```cpp
   // ❌ WRONG: This will cause crashes
   MyClass::MyClass(const Napi::CallbackInfo& info) {
       m_object = SomeWinRTClass::CreateAsync().get(); // Blocking on UI thread!
   }
   ```

2. **Missing Parameter Validation**

   ```cpp
   // ❌ WRONG: No validation leads to crashes
   std::string filePath = info[0].As<Napi::String>().Utf8Value(); // What if info[0] is undefined?
   ```

3. **Synchronous WinRT Calls on Main Thread**

   ```cpp
   // ❌ WRONG: Blocking operations on main thread
   auto result = someAsyncOp.get(); // This blocks!
   ```

4. **Manual Memory Management for WinRT Objects**

   ```cpp
   // ❌ WRONG: Manual delete of WinRT objects
   delete m_winrtObject; // WinRT uses reference counting!
   ```

5. **Using Wrong Method Names for Wrapper Extraction**
   ```cpp
   // ❌ WRONG: Inconsistent method naming
   auto options = optionsInstance->GetContentFilterOptions(); // Should be GetOptions()
   ```

### **✅ ALWAYS DO**

1. **Use Background Threads for Async Operations**

   ```cpp
   // ✅ CORRECT: Always use std::thread for async work
   std::thread([deferred, tsfn, tsfn_guard, parameters...]() {
       try {
           // Async work here
           auto result = asyncOperation.get();
           tsfn.BlockingCall([deferred, result](Napi::Env env, Napi::Function) {
               deferred.Resolve(processResult(env, result));
           });
       } catch (...) {
           // Error handling
       }
   }).detach();
   ```

2. **Consistent Error Handling**

   ```cpp
   // ✅ CORRECT: Three-tier error handling
   } catch (const winrt::hresult_error& ex) {
       std::string errorMsg = "WinRT error: " + winrt::to_string(ex.message());
       Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
   } catch (const std::exception& ex) {
       std::string errorMsg = "Error: " + std::string(ex.what());
       Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
   } catch (...) {
       Napi::Error::New(env, "Unknown error occurred").ThrowAsJavaScriptException();
   }
   ```

3. **Proper Resource Cleanup**
   ```cpp
   // ✅ CORRECT: Always provide Close/Dispose methods
   Napi::Value MyClass::MyClose(const Napi::CallbackInfo& info) {
       Napi::Env env = info.Env();
       try {
           if (m_object) {
               m_object->Close();
           }
           return env.Undefined();
       } catch (...) {
           // Error handling
       }
   }
   ```

---

## 📁 File Organization Patterns

### **Header Files (.h)**

```cpp
// Standard organization pattern
#pragma once
#include <napi.h>
#include <optional>
#include <winrt/Microsoft.AI.LanguageModel.h>

using namespace Microsoft::AI::LanguageModel;

class MyLanguageModel : public Napi::ObjectWrap<MyLanguageModel> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::FunctionReference constructor;

    // Constructor (private, only called via External<>)
    MyLanguageModel(const Napi::CallbackInfo& info);

    // Static methods (for creation)
    static Napi::Value MyCreateAsync(const Napi::CallbackInfo& info);
    static Napi::Value MyGetReadyState(const Napi::CallbackInfo& info);

    // Instance methods
    Napi::Value MyGenerateResponseAsync(const Napi::CallbackInfo& info);
    Napi::Value MyClose(const Napi::CallbackInfo& info);

private:
    LanguageModel* m_model = nullptr;
};
```

### **Implementation Files (.cpp)**

```cpp
// Standard organization pattern
#include "LanguageModelProjections.h"
#include "ProjectionHelper.h"
#include "ContentSeverity.h"
// ... other includes

// Static member definitions FIRST
Napi::FunctionReference MyLanguageModel::constructor;

// Init method
Napi::Object MyLanguageModel::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "LanguageModel", {
        StaticMethod("CreateAsync", &MyLanguageModel::MyCreateAsync),
        StaticMethod("GetReadyState", &MyLanguageModel::MyGetReadyState),
        InstanceMethod("GenerateResponseAsync", &MyLanguageModel::MyGenerateResponseAsync),
        InstanceMethod("Close", &MyLanguageModel::MyClose)
    });

    constructor = Napi::Persistent(func);
    exports.Set("LanguageModel", func);
    return exports;
}

// Constructor
// Static methods
// Instance methods
// Helper methods (private)
```

---

## 🎯 API Design Patterns

### **1. Windows AI API Mapping**

#### **Namespace Mapping**

- `Microsoft.Windows.AI.Text` → `LanguageModelProjections.h/cpp`
- `Microsoft.Windows.AI.Imaging` → `ImagingProjections.h/cpp`
- Content Safety → `ContentSeverity.h/cpp`

#### **Method Naming Convention**

```cpp
// WinRT: LanguageModel.GenerateResponseAsync()
// C++ Wrapper: MyLanguageModel::MyGenerateResponseAsync()
// JavaScript: languageModel.GenerateResponseAsync()

// Pattern: My[ClassName]::My[MethodName]
```

### **2. Constructor Patterns**

#### **For Classes That Can't Be Directly Instantiated**

```cpp
// ✅ CORRECT: Static factory pattern
MyImageDescriptionGenerator::MyImageDescriptionGenerator(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyImageDescriptionGenerator>(info) {
    if (info.Length() == 0 || !info[0].IsExternal()) {
        Napi::Error::New(info.Env(), "Cannot instantiate ImageDescriptionGenerator directly. Use ImageDescriptionGenerator.CreateAsync()").ThrowAsJavaScriptException();
        return;
    }

    auto external = info[0].As<Napi::External<ImageDescriptionGenerator>>();
    m_generator = external.Data();
}
```

#### **For Simple Data Classes**

```cpp
// ✅ CORRECT: Allow direct instantiation
MyLanguageModelOptions::MyLanguageModelOptions(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyLanguageModelOptions>(info) {
    // Initialize with default values
    m_options = LanguageModelOptions();
}
```

### **3. Progress Streaming Pattern**

```javascript
// ✅ This is the target JavaScript API pattern
const promise = model.GenerateResponseAsync(prompt, options);

// Progress callback for streaming
promise.progress((error, progressText) => {
  if (error) {
    console.error("Progress error:", error);
    return;
  }
  console.log("Streaming text:", progressText);
});

// Final result
const finalResult = await promise;
console.log("Complete:", finalResult.Text);
```

---

## 🔧 Development Workflow

### **1. Adding New Windows AI APIs**

1. **Research the API**: Check Windows AI documentation for parameter types, return values, async patterns
2. **Add to appropriate projection file**:
   - Language models → `LanguageModelProjections.h/cpp`
   - Imaging → `ImagingProjections.h/cpp`
   - Content safety → `ContentSeverity.h/cpp`
3. **Follow the constructor pattern**: External<> for complex objects, direct for simple data
4. **Implement parameter validation**: Always validate all parameters with clear error messages
5. **Use appropriate async pattern**: ProgressPromise for progress operations, regular Promise for simple async
6. **Add TypeScript definitions**: Update `index.d.ts` with new types
7. **Update README.md**: Add usage examples and documentation

### **2. Testing Approach**

#### **Manual Testing Pattern**

```javascript
// Always test the happy path AND error conditions
const windowsAI = require("./build/Release/windows-ai-electron.node");

async function testFeature() {
  try {
    // 1. Check readiness
    const state = windowsAI.FeatureName.GetReadyState();
    console.log("Ready state:", state);

    // 2. Test creation
    const instance = await windowsAI.FeatureName.CreateAsync();

    // 3. Test main functionality
    const result = await instance.MainMethod(validParams);
    console.log("Result:", result);

    // 4. Test error conditions
    try {
      await instance.MainMethod(invalidParams);
    } catch (error) {
      console.log("Expected error:", error.message);
    }

    // 5. Cleanup
    instance.Close();
  } catch (error) {
    console.error("Unexpected error:", error);
  }
}
```

### **3. Build and Debug**

#### **Build Commands**

```bash
# Clean build
npx build-all

# Debug build (in binding.gyp, set 'Debug' configuration)
# Provides better stack traces and debugging symbols
```

#### **Common Build Issues**

- **Missing Windows SDK**: Run `npx winapp restore`
- **Wrong architecture**: Check `--arch=arm64` vs `--arch=x64` in package.json
- **Missing dependencies**: Ensure Visual Studio 2022 C++ tools installed

---

## 📚 Code Examples & Snippets

### **Complete New Class Implementation Template**

```cpp
// Header file pattern
class MyNewFeature : public Napi::ObjectWrap<MyNewFeature> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::FunctionReference constructor;

    MyNewFeature(const Napi::CallbackInfo& info);

    // Static methods
    static Napi::Value MyCreateAsync(const Napi::CallbackInfo& info);
    static Napi::Value MyGetReadyState(const Napi::CallbackInfo& info);
    static Napi::Value MyEnsureReadyAsync(const Napi::CallbackInfo& info);

    // Instance methods
    Napi::Value MyMainMethod(const Napi::CallbackInfo& info);
    Napi::Value MyClose(const Napi::CallbackInfo& info);

private:
    NewFeature* m_feature = nullptr;
};

// Implementation pattern
Napi::FunctionReference MyNewFeature::constructor;

Napi::Object MyNewFeature::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "NewFeature", {
        StaticMethod("CreateAsync", &MyNewFeature::MyCreateAsync),
        StaticMethod("GetReadyState", &MyNewFeature::MyGetReadyState),
        StaticMethod("EnsureReadyAsync", &MyNewFeature::MyEnsureReadyAsync),
        InstanceMethod("MainMethod", &MyNewFeature::MyMainMethod),
        InstanceMethod("Close", &MyNewFeature::MyClose)
    });

    constructor = Napi::Persistent(func);
    exports.Set("NewFeature", func);
    return exports;
}

MyNewFeature::MyNewFeature(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyNewFeature>(info) {
    if (info.Length() == 0 || !info[0].IsExternal()) {
        Napi::Error::New(info.Env(), "Cannot instantiate NewFeature directly. Use NewFeature.CreateAsync()").ThrowAsJavaScriptException();
        return;
    }

    auto external = info[0].As<Napi::External<NewFeature>>();
    m_feature = external.Data();
}

// Follow existing patterns for all other methods...
```

---

## 🚨 Common Gotchas & Solutions

### **1. SetInstanceData Conflicts**

**Problem**: Multiple calls to `SetInstanceData` crash the addon
**Solution**: Use static constructors with External<> pattern instead

### **2. WinRT Async Deadlocks**

**Problem**: `.get()` calls on main thread cause deadlocks
**Solution**: Always use `std::thread` for async operations

### **3. Memory Leaks in ThreadSafeFunctions**

**Problem**: TSFNs not properly released
**Solution**: Use RAII with shared_ptr guards:

```cpp
auto tsfn_guard = std::shared_ptr<void>(nullptr, [tsfn](void*) mutable { tsfn.Release(); });
```

### **4. Parameter Type Mismatches**

**Problem**: JavaScript numbers vs. C++ enums
**Solution**: Always cast and validate:

```cpp
int32_t enumValue = info[1].As<Napi::Number>().Int32Value();
MyEnum typedEnum = static_cast<MyEnum>(enumValue);
```

### **5. Progress Callback Errors**

**Problem**: Progress callbacks throw and break the chain
**Solution**: Always wrap in try-catch:

```cpp
jsCallback.Call({ env.Null(), progressValue });
} catch (...) {} // Silent catch for progress callbacks
```

---

## 📖 Documentation Standards

### **README.md Sections Required**

1. **API documentation** with all classes and methods
2. **Usage examples** for each major feature
3. **Error handling** patterns
4. **Supported file formats** and requirements
5. **Troubleshooting** section

### **TypeScript Definitions Requirements**

1. **All classes** must have corresponding interfaces
2. **Enums** must match C++ enum values exactly
3. **Progress promises** must use the ProgressPromise<T> interface
4. **Both ES6 and CommonJS** export patterns supported

### **Code Comments Standards**

```cpp
// ✅ GOOD: Explain WHY, not WHAT
// Use External<> pattern to prevent SetInstanceData conflicts
auto external = Napi::External<LanguageModel>::New(env, persistentModel.get());

// ❌ BAD: Explains the obvious
// Create an external
auto external = Napi::External<LanguageModel>::New(env, persistentModel.get());
```

---

## 🎯 Success Metrics

A successful contribution should:

- ✅ Follow all architectural patterns consistently
- ✅ Include comprehensive parameter validation
- ✅ Provide proper error handling with clear messages
- ✅ Update TypeScript definitions
- ✅ Include usage examples in README
- ✅ Pass manual testing with valid and invalid inputs
- ✅ Properly manage WinRT object lifetimes
- ✅ Use appropriate async patterns (ProgressPromise vs Promise)

---

_This guide should be updated whenever new patterns emerge or architectural decisions change. It serves as the single source of truth for development practices in this repository._
