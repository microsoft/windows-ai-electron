#pragma once

#include <napi.h>
#include <optional>
#include <memory>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Graphics.Imaging.h>
#include <winrt/Microsoft.Windows.AI.Imaging.h>
#include <winrt/Microsoft.Windows.AI.ContentSafety.h>

#include "ProjectionHelper.h"

using namespace winrt;
using namespace Microsoft::Windows::AI;
using namespace Microsoft::Windows::AI::Imaging;
using namespace Microsoft::Windows::AI::ContentSafety;

// Forward declarations
class MyImageDescriptionGenerator;
class MyImageDescriptionResult;
class MyTextRecognizer;
class MyRecognizedText;
class MyRecognizedLine;
class MyRecognizedWord;
class MyRecognizedTextBoundingBox;
class MyImageObjectExtractor;
class MyImageObjectExtractorHint;
class MyImageObjectRemover;
class MyImageScaler;

// Wrapper for ImageDescriptionGenerator
class MyImageDescriptionGenerator : public Napi::ObjectWrap<MyImageDescriptionGenerator> {
public:
    static Napi::FunctionReference constructor;
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    
    static Napi::Value MyCreateAsync(const Napi::CallbackInfo& info);
    static Napi::Value MyGetReadyState(const Napi::CallbackInfo& info);
    static Napi::Value MyEnsureReadyAsync(const Napi::CallbackInfo& info);
    
    MyImageDescriptionGenerator(const Napi::CallbackInfo& info);

private:
    ImageDescriptionGenerator* m_generator;
    
    Napi::Value MyDescribeAsync(const Napi::CallbackInfo& info);
    Napi::Value MyClose(const Napi::CallbackInfo& info);
};

// Wrapper for ImageDescriptionResult
class MyImageDescriptionResult : public Napi::ObjectWrap<MyImageDescriptionResult> {
public:
    static Napi::FunctionReference constructor;
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    
    MyImageDescriptionResult(const Napi::CallbackInfo& info);
    bool HasResult() const;
    void SetResult(const ImageDescriptionResult& result);

private:
    std::optional<ImageDescriptionResult> m_result;
    
    Napi::Value GetDescription(const Napi::CallbackInfo& info);
    Napi::Value GetStatus(const Napi::CallbackInfo& info);
};

// Wrapper for TextRecognizer
class MyTextRecognizer : public Napi::ObjectWrap<MyTextRecognizer> {
public:
    static Napi::FunctionReference constructor;
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    
    static Napi::Value MyCreateAsync(const Napi::CallbackInfo& info);
    static Napi::Value MyGetReadyState(const Napi::CallbackInfo& info);
    static Napi::Value MyEnsureReadyAsync(const Napi::CallbackInfo& info);
    
    MyTextRecognizer(const Napi::CallbackInfo& info);

private:
    TextRecognizer* m_recognizer;
    
    Napi::Value MyRecognizeTextFromImageAsync(const Napi::CallbackInfo& info);
    Napi::Value MyRecognizeTextFromImage(const Napi::CallbackInfo& info);
    Napi::Value MyClose(const Napi::CallbackInfo& info);
    Napi::Value MyDispose(const Napi::CallbackInfo& info);
};

// Wrapper for RecognizedText
class MyRecognizedText : public Napi::ObjectWrap<MyRecognizedText> {
public:
    static Napi::FunctionReference constructor;
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    
    MyRecognizedText(const Napi::CallbackInfo& info);
    bool HasResult() const;
    void SetResult(const RecognizedText& result);

private:
    std::optional<RecognizedText> m_result;
    
    Napi::Value GetLines(const Napi::CallbackInfo& info);
    Napi::Value GetTextAngle(const Napi::CallbackInfo& info);
};

// Wrapper for RecognizedLine
class MyRecognizedLine : public Napi::ObjectWrap<MyRecognizedLine> {
public:
    static Napi::FunctionReference constructor;
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    
    MyRecognizedLine(const Napi::CallbackInfo& info);
    bool HasResult() const;

private:
    std::optional<RecognizedLine> m_result;
    
    Napi::Value GetBoundingBox(const Napi::CallbackInfo& info);
    Napi::Value GetStyle(const Napi::CallbackInfo& info);
    Napi::Value GetLineStyleConfidence(const Napi::CallbackInfo& info);
    Napi::Value GetText(const Napi::CallbackInfo& info);
    Napi::Value GetWords(const Napi::CallbackInfo& info);
};

// Wrapper for RecognizedWord
class MyRecognizedWord : public Napi::ObjectWrap<MyRecognizedWord> {
public:
    static Napi::FunctionReference constructor;
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    
    MyRecognizedWord(const Napi::CallbackInfo& info);
    bool HasResult() const;

private:
    std::optional<RecognizedWord> m_result;
    
    Napi::Value GetBoundingBox(const Napi::CallbackInfo& info);
    Napi::Value GetConfidence(const Napi::CallbackInfo& info);
    Napi::Value GetText(const Napi::CallbackInfo& info);
};

// Wrapper for RecognizedTextBoundingBox
class MyRecognizedTextBoundingBox : public Napi::ObjectWrap<MyRecognizedTextBoundingBox> {
public:
    static Napi::FunctionReference constructor;
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    
    MyRecognizedTextBoundingBox(const Napi::CallbackInfo& info);
    bool HasResult() const;

private:
    std::optional<RecognizedTextBoundingBox> m_result;
    
    Napi::Value GetTopLeft(const Napi::CallbackInfo& info);
    Napi::Value GetTopRight(const Napi::CallbackInfo& info);
    Napi::Value GetBottomLeft(const Napi::CallbackInfo& info);
    Napi::Value GetBottomRight(const Napi::CallbackInfo& info);
};

// Wrapper for ImageObjectExtractor
class MyImageObjectExtractor : public Napi::ObjectWrap<MyImageObjectExtractor> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    
    static Napi::Value MyCreateAsync(const Napi::CallbackInfo& info);
    static Napi::Value MyGetReadyState(const Napi::CallbackInfo& info);
    static Napi::Value MyEnsureReadyAsync(const Napi::CallbackInfo& info);
    
    MyImageObjectExtractor(const Napi::CallbackInfo& info);

private:
    ImageObjectExtractor* m_extractor;
    
    Napi::Value MyExtractAsync(const Napi::CallbackInfo& info);
    Napi::Value MyClose(const Napi::CallbackInfo& info);
};

// Wrapper for ImageObjectExtractorHint
class MyImageObjectExtractorHint : public Napi::ObjectWrap<MyImageObjectExtractorHint> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    
    MyImageObjectExtractorHint(const Napi::CallbackInfo& info);

private:
    // Note: Using optional to handle WinRT type construction issues
    std::optional<ImageObjectExtractorHint> m_hint;
    
    Napi::Value GetX(const Napi::CallbackInfo& info);
    void SetX(const Napi::CallbackInfo& info, const Napi::Value& value);
    Napi::Value GetY(const Napi::CallbackInfo& info);
    void SetY(const Napi::CallbackInfo& info, const Napi::Value& value);
    Napi::Value GetWidth(const Napi::CallbackInfo& info);
    void SetWidth(const Napi::CallbackInfo& info, const Napi::Value& value);
    Napi::Value GetHeight(const Napi::CallbackInfo& info);
    void SetHeight(const Napi::CallbackInfo& info, const Napi::Value& value);
};

// Wrapper for ImageObjectRemover
class MyImageObjectRemover : public Napi::ObjectWrap<MyImageObjectRemover> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    
    static Napi::Value MyCreateAsync(const Napi::CallbackInfo& info);
    static Napi::Value MyGetReadyState(const Napi::CallbackInfo& info);
    static Napi::Value MyEnsureReadyAsync(const Napi::CallbackInfo& info);
    
    MyImageObjectRemover(const Napi::CallbackInfo& info);

private:
    ImageObjectRemover* m_remover;
    
    Napi::Value MyRemoveAsync(const Napi::CallbackInfo& info);
    Napi::Value MyClose(const Napi::CallbackInfo& info);
};

// Wrapper for ImageScaler
class MyImageScaler : public Napi::ObjectWrap<MyImageScaler> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    
    static Napi::Value MyCreateAsync(const Napi::CallbackInfo& info);
    static Napi::Value MyGetReadyState(const Napi::CallbackInfo& info);
    static Napi::Value MyEnsureReadyAsync(const Napi::CallbackInfo& info);
    
    MyImageScaler(const Napi::CallbackInfo& info);

private:
    ImageScaler* m_scaler;
    
    Napi::Value MyScaleAsync(const Napi::CallbackInfo& info);
    Napi::Value MyClose(const Napi::CallbackInfo& info);
};