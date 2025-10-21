#include <napi.h>
#include "LanguageModelProjections.h"
#include "ImagingProjections.h"
#include "ContentSeverity.h"

using namespace winrt;
using namespace Microsoft::Windows::AI;
using namespace Microsoft::Windows::AI::Text;
using namespace Microsoft::Windows::AI::Imaging;
using namespace Microsoft::Windows::AI::ContentSafety;

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
    
    // Add SeverityLevel enum
    Napi::Object severityLevel = Napi::Object::New(env);
    severityLevel.Set("Minimum", Napi::Number::New(env, 10));
    severityLevel.Set("Low", Napi::Number::New(env, 11));
    severityLevel.Set("Medium", Napi::Number::New(env, 12));
    severityLevel.Set("High", Napi::Number::New(env, 13));
    
    exports.Set("SeverityLevel", severityLevel);
    
    // Add TextRewriteTone enum
    Napi::Object textRewriteTone = Napi::Object::New(env);
    textRewriteTone.Set("Default", Napi::Number::New(env, 0));
    textRewriteTone.Set("General", Napi::Number::New(env, 1));
    textRewriteTone.Set("Casual", Napi::Number::New(env, 2));
    textRewriteTone.Set("Formal", Napi::Number::New(env, 3)); 
    // Note: Windows AI API documentation lists 'Concise' option as well, but this option does not exist
    
    exports.Set("TextRewriteTone", textRewriteTone);
    
    // Add AIFeatureReadyResultState enum
    Napi::Object aiFeatureReadyResultState = Napi::Object::New(env);
    aiFeatureReadyResultState.Set("InProgress", Napi::Number::New(env, 0));
    aiFeatureReadyResultState.Set("Success", Napi::Number::New(env, 1));
    aiFeatureReadyResultState.Set("Failure", Napi::Number::New(env, 2));
    
    exports.Set("AIFeatureReadyResultState", aiFeatureReadyResultState);
    
    // Add ImageDescriptionKind enum
    Napi::Object imageDescriptionKind = Napi::Object::New(env);
    imageDescriptionKind.Set("BriefDescription", Napi::Number::New(env, 0));
    imageDescriptionKind.Set("DetailedDescription", Napi::Number::New(env, 1));
    imageDescriptionKind.Set("DiagramDescription", Napi::Number::New(env, 2));
    imageDescriptionKind.Set("AccessibleDescription", Napi::Number::New(env, 3));
    
    exports.Set("ImageDescriptionKind", imageDescriptionKind);
    
    // Add ImageDescriptionResultStatus enum
    Napi::Object imageDescriptionResultStatus = Napi::Object::New(env);
    imageDescriptionResultStatus.Set("Success", Napi::Number::New(env, 0));
    imageDescriptionResultStatus.Set("Failure", Napi::Number::New(env, 1));
    imageDescriptionResultStatus.Set("ContentFiltered", Napi::Number::New(env, 2));
    
    exports.Set("ImageDescriptionResultStatus", imageDescriptionResultStatus);
    
    // Add RecognizedLineStyle enum
    Napi::Object recognizedLineStyle = Napi::Object::New(env);
    recognizedLineStyle.Set("None", Napi::Number::New(env, 0));
    recognizedLineStyle.Set("Handwritten", Napi::Number::New(env, 1));
    recognizedLineStyle.Set("Printed", Napi::Number::New(env, 2));
    
    exports.Set("RecognizedLineStyle", recognizedLineStyle);
    
    exports = MyLanguageModel::Init(env, exports);
    exports = MyLanguageModelResponseResult::Init(env, exports);
    exports = MyLanguageModelOptions::Init(env, exports);
    exports = MyContentFilterOptions::Init(env, exports);
    exports = MyImageContentFilterSeverity::Init(env, exports);
    exports = MyTextContentFilterSeverity::Init(env, exports);
    exports = MyAIFeatureReadyResult::Init(env, exports);
    exports = MyConversationItem::Init(env, exports);
    exports = MyTextSummarizer::Init(env, exports);
    exports = MyTextRewriter::Init(env, exports);
    
    exports = MyImageDescriptionGenerator::Init(env, exports);
    exports = MyImageDescriptionResult::Init(env, exports);
    exports = MyTextRecognizer::Init(env, exports);
    exports = MyRecognizedText::Init(env, exports);
    exports = MyRecognizedLine::Init(env, exports);
    exports = MyRecognizedWord::Init(env, exports);
    exports = MyRecognizedTextBoundingBox::Init(env, exports);
    exports = MyImageObjectExtractor::Init(env, exports);
    exports = MyImageObjectExtractorHint::Init(env, exports);
    exports = MyImageObjectRemover::Init(env, exports);
    
    return MyImageScaler::Init(env, exports);
}

NODE_API_MODULE(addon, Init)