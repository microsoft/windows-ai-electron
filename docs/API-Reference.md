# Supported API's

This package supports many of the API's within the [Microsoft.Windows.AI.Text](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text?view=windows-app-sdk-1.8), [Microsoft.Windows.AI.Imaging](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging?view=windows-app-sdk-1.8), and [Microsoft.Windows.AI](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai?view=windows-app-sdk-1.8) namespaces. This package is designed to expose a JavaScript API surface of the native Windows AI API's. Class, method, enum, and parameter names in this package are designed to replicate their native counterparts in WinAppSDK.

Below is the list of supported API's for the `@microsoft/winapp-windows-ai` package. For usage information, see the [Usage Guide](Usage.md).

### Core Classes

#### `LanguageModel`

Main class for text generation and AI model interactions. Maps to WinAppSDK [Microsoft.Windows.AI.Text.LanguageModel](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.languagemodel?view=windows-app-sdk-1.8)

**Static Methods:**

- `CreateAsync()` - Asynchronously creates a new LanguageModel instance. Maps to [LanguageModel.CreateAsync()](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.languagemodel.createasync?view=windows-app-sdk-1.8)
- `GetReadyState()` - Returns the current AI feature ready state. Maps to [LanguageModel.GetReadyState()](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.languagemodel.getreadystate?view=windows-app-sdk-1.8)
- `EnsureReadyAsync()` - Ensures AI features are ready for use. Maps to [LanguageModel.EnsureReadyAsync()](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.languagemodel.ensurereadyasync?view=windows-app-sdk-1.8)

**Instance Methods:**

> [!IMPORTANT]  
> This method uses a Windows API which is a part of a [Limited Access Feature](https://learn.microsoft.com/en-us/uwp/api/windows.applicationmodel.limitedaccessfeatures?view=winrt-26100). To request an unlock token, please use the [LAF Access Token Request Form](https://go.microsoft.com/fwlink/?linkid=2271232&c1cid=04x409). To use this method, you must first call [LimitedAccessFeature.TryUnlockToken](#limitedaccessfeatures). See [Usage.md](Usage.md) for usage examples.

- <code>GenerateResponseAsync(string, <a href="#languagemodeloptions">LanguageModelOptions</a>?)</code> - Generates text response from a prompt. Maps to [LanguageModel.GenerateResponseAsync()](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.languagemodel.generateresponseasync?view=windows-app-sdk-1.8)

#### `LanguageModelOptions`

Configuration options for language model behavior. Maps to WinAppSDK [Microsoft.Windows.AI.Text.LanguageModelOptions](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.languagemodeloptions?view=windows-app-sdk-1.8)

**Properties:**

- `Temperature` (number) - Controls randomness in generation (0.0-1.0). Maps to [LanguageModelOptions.Temperature](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.languagemodeloptions.temperature?view=windows-app-sdk-1.8)
- `TopK` (number) - Limits vocabulary for next token selection. Maps to [LanguageModelOptions.TopK](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.languagemodeloptions.topk?view=windows-app-sdk-1.8)
- `TopP` (number) - Nucleus sampling threshold (0.0-1.0). Maps to [LanguageModelOptions.TopP](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.languagemodeloptions.topp?view=windows-app-sdk-1.8)
- `ContentFilterOptions` ([ConftentFilterOptions](#contentfilteroptions)) - Content safety settings. Maps to [LanguageModelOptions.ContentFilterOptions](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.languagemodeloptions.contentfilteroptions?view=windows-app-sdk-1.8)

#### `LanguageModelResponseResult`

Contains the result of a text generation operation. Maps to WinAppSDK [Microsoft.Windows.AI.Text.LanguageModelResponseResult](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.languagemodelresponseresult?view=windows-app-sdk-1.8)

**Properties:**

- `Text` (string) - The generated text response. Maps to [LanguageModelResponseResult.Text](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.languagemodelresponseresult.text?view=windows-app-sdk-1.8)
- `Status` (number) - Status code from LanguageModelResponseStatus enum. Maps to [LanguageModelResponseResult.Status](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.languagemodelresponseresult.status?view=windows-app-sdk-1.8)
- `ExtendedError` (string) - Extended error information if available. Maps to [LanguageModelResponseResult.ExtendedError](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.languagemodelresponseresult.extendederror?view=windows-app-sdk-1.8)

#### `TextSummarizer`

Main class for AI-powered text and conversation summarization. Maps to WinAppSDK [Microsoft.Windows.AI.Text.TextSummarizer](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.textsummarizer?view=windows-app-sdk-1.8)

**Constructor:**

- <code>new TextSummarizer(<a href="#languagemodel">LanguageModel</a>)</code> - Creates a TextSummarizer with the provided LanguageModel instance. Maps to [TextSummarizer(LanguageModel)](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.textsummarizer.-ctor?view=windows-app-sdk-1.8)

**Instance Methods:**

- `SummarizeAsync(string)` - Asynchronously summarizes the provided text. Maps to [TextSummarizer.SummarizeAsync(String)](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.textsummarizer.summarizeasync?view=windows-app-sdk-1.8)
- `SummarizeParagraphAsync(string)` - Asynchronously summarizes a paragraph with paragraph-specific optimization. Maps to [TextSummarizer.SummarizeParagraphAsync(String)](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.textsummarizer.summarizeparagraphasync?view=windows-app-sdk-1.8)
- <code>SummarizeConversationAsync(<a href="#conversationitem">ConversationItem</a>[], <a href="#conversationsummaryoptions">ConversationSummaryOptions</a>)</code> - Asynchronously summarizes a conversation from an array of ConversationItem objects. Maps to [TextSummarizer.SummarizeConversationAsync(IVectorView<ConversationItem>, ConversationSummaryOptions)](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.textsummarizer.summarizeconversationasync?view=windows-app-sdk-1.8)
- `IsPromptLargerThanContext(string)` - Checks if text prompt exceeds context window (returns boolean). Maps to [TextSummarizer.IsPromptLargerThanContext(String)](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.textsummarizer.ispromptlargerthancontext?view=windows-app-sdk-1.8)
- <code>IsPromptLargerThanContext(<a href="#conversationitem">ConversationItem</a>[], <a href="#conversationsummaryoptions">ConversationSummaryOptions</a>)</code> - Checks if conversation prompt exceeds context window (returns object with isLarger boolean and cutoffPosition number). Maps to [TextSummarizer.IsPromptLargerThanContext(IVectorView<ConversationItem>, ConversationSummaryOptions, UInt64)](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.textsummarizer.ispromptlargerthancontext?view=windows-app-sdk-1.8)

#### `ConversationItem`

Represents a single message in a conversation for summarization. Maps to WinAppSDK [Microsoft.Windows.AI.Text.ConversationItem](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.conversationitem?view=windows-app-sdk-1.8)

**Constructor:**

- `new ConversationItem()` - Creates a new conversation item. Maps to [ConversationItem()](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.conversationitem.-ctor?view=windows-app-sdk-1.8)

**Properties:**

- `Message` (string) - The message content. Maps to [ConversationItem.Message](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.conversationitem.message?view=windows-app-sdk-1.8)
- `Participant` (string) - The participant who sent the message (e.g., "User", "Assistant", "Support"). Maps to [ConversationItem.Participant](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.conversationitem.participant?view=windows-app-sdk-1.8)

#### `ConversationSummaryOptions`

Configuration options for conversation summarization. Maps to WinAppSDK [Microsoft.Windows.AI.Text.ConversationSummaryOptions](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.conversationsummaryoptions?view=windows-app-sdk-1.8)

**Properties:**

- `includeMessageCitations` (boolean) - Whether to include references to specific messages in the summary. Maps to [ConversationSummaryOptions.IncludeMessageCitations](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.conversationsummaryoptions.includemessagecitations?view=windows-app-sdk-1.8)
- `includeParticipantAttribution` (boolean) - Whether to attribute parts of the summary to specific participants. Maps to [ConversationSummaryOptions.IncludeParticipantAttribution](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.conversationsummaryoptions.includeparticipantattribution?view=windows-app-sdk-1.8)

#### `TextRewriter`

Main class for AI-powered text rewriting and tone adjustment. Maps to WinAppSDK [Microsoft.Windows.AI.Text.TextRewriter](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.textrewriter?view=windows-app-sdk-1.8)

**Constructor:**

- <code>new TextRewriter(<a href="#languagemodel">LanguageModel</a>)</code> - Creates a TextRewriter with the provided LanguageModel instance. Maps to [TextRewriter(LanguageModel)](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.textrewriter.-ctor?view=windows-app-sdk-1.8)

**Instance Methods:**

- `RewriteAsync(string)` - Asynchronously rewrites the provided text using the default tone. Maps to [TextRewriter.RewriteAsync(String)](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.textrewriter.rewriteasync?view=windows-app-sdk-1.8)
- <code>RewriteAsync(string, <a href="#textrewritetone">TextRewriteTone</a>)</code> - Asynchronously rewrites the provided text using the specified TextRewriteTone. Maps to [TextRewriter.RewriteAsync(String, TextRewriteTone)](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.textrewriter.rewriteasync?view=windows-app-sdk-1.8)

#### `TextToTableConverter`

Main class for AI-powered conversion of text content into structured table format. Maps to WinAppSDK [Microsoft.Windows.AI.Text.TextToTableConverter](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.texttotableconverter?view=windows-app-sdk-1.8)

**Constructor:**

- <code>new TextToTableConverter(<a href="#languagemodel">LanguageModel</a>)</code> - Creates a TextToTableConverter with the provided LanguageModel instance. Maps to [TextToTableConverter(LanguageModel)](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.texttotableconverter.-ctor?view=windows-app-sdk-1.8)

**Instance Methods:**

- `ConvertAsync(string)` - Asynchronously converts the provided text into a structured table format, returns <a href="#texttotableresponseresult">TextToTableResponseResult</a>. Maps to [TextToTableConverter.ConvertAsync(String)](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.texttotableconverter.convertasync?view=windows-app-sdk-1.8)

#### `TextToTableResponseResult`

Result object containing structured table data from text conversion operations. Maps to WinAppSDK [Microsoft.Windows.AI.Text.TextToTableResponseResult](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.texttotableresponseresult?view=windows-app-sdk-1.8)

**Properties:**

- `ExtendedError` (string) - Extended error information as hex string (e.g., "0x80004005") if available. Maps to [TextToTableResponseResult.ExtendedError](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.texttotableresponseresult.extendederror?view=windows-app-sdk-1.8)
- `Status` (number) - Status code from the conversion operation. Maps to [TextToTableResponseResult.Status](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.texttotableresponseresult.status?view=windows-app-sdk-1.8)

**Methods:**

- `GetRows()` - Returns an array of <a href="#texttotablerow">TextToTableRow</a> objects representing the table structure. Maps to [TextToTableResponseResult.GetRows()](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.texttotableresponseresult.getrows?view=windows-app-sdk-1.8)

#### `TextToTableRow`

Represents a single row in a converted table structure. Maps to WinAppSDK [Microsoft.Windows.AI.Text.TextToTableRow](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.texttotablerow?view=windows-app-sdk-1.8)

**Methods:**

- `GetColumns()` - Returns an array of strings representing the column values for this row. Maps to [TextToTableRow.GetColumns()](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.texttotablerow.getcolumns?view=windows-app-sdk-1.8)

#### `AIFeatureReadyResult`

Result object for AI feature readiness operations. Maps to WinAppSDK [Microsoft.Windows.AI.AIFeatureReadyResult](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.aifeaturereadyresult?view=windows-app-sdk-1.8)

**Properties:**

- `Error` (number) - Error code if operation failed. Maps to [AIFeatureReadyResult.Error](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.aifeaturereadyresult.error?view=windows-app-sdk-1.8)
- `ErrorDisplayText` (string) - Human-readable error description. Maps to [AIFeatureReadyResult.ErrorDisplayText](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.aifeaturereadyresult.errordisplaytext?view=windows-app-sdk-1.8)
- `ExtendedError` (number) - Extended error code. Maps to [AIFeatureReadyResult.ExtendedError](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.aifeaturereadyresult.extendederror?view=windows-app-sdk-1.8)
- `Status` (number) - Operation status from [`AIFeatureReadyResultState`](#aifeaturereadyresultstate) enum. Maps to [AIFeatureReadyResult.Status](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.aifeaturereadyresult.status?view=windows-app-sdk-1.8)

### Imaging Classes

#### `ImageDescriptionGenerator`

Main class for generating descriptions and captions for images. Maps to WinAppSDK [Microsoft.Windows.AI.Imaging.ImageDescriptionGenerator](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.imagedescriptiongenerator?view=windows-app-sdk-1.8)

**Static Methods:**

- `CreateAsync()` - Asynchronously creates a new ImageDescriptionGenerator instance. Maps to [ImageDescriptionGenerator.CreateAsync()](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.imagedescriptiongenerator.createasync?view=windows-app-sdk-1.8)
- `GetReadyState()` - Returns the current AI feature ready state for image description. Maps to [ImageDescriptionGenerator.GetReadyState()](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.imagedescriptiongenerator.getreadystate?view=windows-app-sdk-1.8)
- `EnsureReadyAsync()` - Ensures image description features are ready for use. Maps to [ImageDescriptionGenerator.EnsureReadyAsync()](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.imagedescriptiongenerator.ensurereadyasync?view=windows-app-sdk-1.8)

**Instance Methods:**

- <code>DescribeAsync(string, <a href="#imagedescriptionkind">ImageDescriptionKind</a>, <a href="#contentfilteroptions">ContentFilterOptions</a>)</code> - Generates description for an image, file path must be the absolute path to the image. Maps to [ImageDescriptionGenerator.DescribeAsync(String, ImageDescriptionKind, ContentFilterOptions)](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.imagedescriptiongenerator.describeasync?view=windows-app-sdk-1.8)
- `Close()` - Closes the generator and releases resources. Maps to [ImageDescriptionGenerator.Close()](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.imagedescriptiongenerator.close?view=windows-app-sdk-1.8)

#### `ImageDescriptionResult`

Contains the result of an image description operation. Maps to WinAppSDK [Microsoft.Windows.AI.Imaging.ImageDescriptionResult](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.imagedescriptionresult?view=windows-app-sdk-1.8)

**Properties:**

- `Description` (string) - The generated image description. Maps to [ImageDescriptionResult.Description](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.imagedescriptionresult.description?view=windows-app-sdk-1.8)
- `Status` (<a href="#imagedescriptionresultstatus">ImageDescriptionResultStatus</a>) - Status code from ImageDescriptionResultStatus enum. Maps to [ImageDescriptionResult.Status](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.imagedescriptionresult.status?view=windows-app-sdk-1.8)

#### `TextRecognizer`

Main class for optical character recognition (OCR) on images. Maps to WinAppSDK [Microsoft.Windows.AI.Imaging.TextRecognizer](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.textrecognizer?view=windows-app-sdk-1.8)

**Static Methods:**

- `CreateAsync()` - Asynchronously creates a new TextRecognizer instance. Maps to [TextRecognizer.CreateAsync()](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.textrecognizer.createasync?view=windows-app-sdk-1.8)
- `GetReadyState()` - Returns the current AI feature ready state for text recognition. Maps to [TextRecognizer.GetReadyState()](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.textrecognizer.getreadystate?view=windows-app-sdk-1.8)
- `EnsureReadyAsync()` - Ensures text recognition features are ready for use. Maps to [TextRecognizer.EnsureReadyAsync()](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.textrecognizer.ensurereadyasync?view=windows-app-sdk-1.8)

**Instance Methods:**

- `RecognizeTextFromImageAsync(string)` - Asynchronously recognizes text in an image, file path must be the absolute path to the image. Maps to [TextRecognizer.RecognizeTextFromImageAsync(String)](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.textrecognizer.recognizetextfromimageasync?view=windows-app-sdk-1.8)
- `RecognizeTextFromImage(string)` - Synchronously recognizes text in an image, file path must be the absolute path to the image. Maps to [TextRecognizer.RecognizeTextFromImage(String)](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.textrecognizer.recognizetextfromimage?view=windows-app-sdk-1.8)
- `Close()` - Closes the recognizer and releases resources. Maps to [TextRecognizer.Close()](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.textrecognizer.close?view=windows-app-sdk-1.8)
- `Dispose()` - Disposes the recognizer and cleans up resources. Maps to [TextRecognizer.Dispose()](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.textrecognizer.dispose?view=windows-app-sdk-1.8)

#### `RecognizedText`

Contains the complete text recognition results from an image. Maps to WinAppSDK [Microsoft.Windows.AI.Imaging.RecognizedText](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.recognizedtext?view=windows-app-sdk-1.8)

**Properties:**

- `Lines` (<a href="#recognizedline">RecognizedLine</a>[]) - Array of recognized text lines. Maps to [RecognizedText.Lines](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.recognizedtext.lines?view=windows-app-sdk-1.8)
- `TextAngle` (number) - Angle of the text in the image. Maps to [RecognizedText.TextAngle](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.recognizedtext.textangle?view=windows-app-sdk-1.8)

#### `RecognizedLine`

Represents a single line of recognized text. Maps to WinAppSDK [Microsoft.Windows.AI.Imaging.RecognizedLine](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.recognizedline?view=windows-app-sdk-1.8)

**Properties:**

- `BoundingBox` (<a href="#recognizedtextboundingbox">RecognizedTextBoundingBox</a>) - Bounding box coordinates for the line. Maps to [RecognizedLine.BoundingBox](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.recognizedline.boundingbox?view=windows-app-sdk-1.8)
- `Style` (<a href="#recognizedlinestyle">RecognizedLineStyle</a>) - Text style information from RecognizedLineStyle enum. Maps to [RecognizedLine.Style](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.recognizedline.style?view=windows-app-sdk-1.8)
- `LineStyleConfidence` (number) - Confidence level for the line style. Maps to [RecognizedLine.LineStyleConfidence](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.recognizedline.linestyleconfidence?view=windows-app-sdk-1.8)
- `Text` (string) - The recognized text content. Maps to [RecognizedLine.Text](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.recognizedline.text?view=windows-app-sdk-1.8)
- `Words` (<a href="#recognizedword">RecognizedWord</a>[]) - Array of individual words in the line. Maps to [RecognizedLine.Words](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.recognizedline.words?view=windows-app-sdk-1.8)

#### `RecognizedWord`

Represents a single recognized word. Maps to WinAppSDK [Microsoft.Windows.AI.Imaging.RecognizedWord](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.recognizedword?view=windows-app-sdk-1.8)

**Properties:**

- `BoundingBox` (<a href="#recognizedtextboundingbox">RecognizedTextBoundingBox</a>) - Bounding box coordinates for the word. Maps to [RecognizedWord.BoundingBox](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.recognizedword.boundingbox?view=windows-app-sdk-1.8)
- `MatchConfidence` (number) - Confidence level for the word recognition. Maps to [RecognizedWord.MatchConfidence](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.recognizedword.matchconfidence?view=windows-app-sdk-1.8)
- `Text` (string) - The recognized word text. Maps to [RecognizedWord.Text](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.recognizedword.text?view=windows-app-sdk-1.8)

#### `RecognizedTextBoundingBox`

Defines the bounding box coordinates for recognized text elements. Maps to WinAppSDK [Microsoft.Windows.AI.Imaging.RecognizedTextBoundingBox](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.recognizedtextboundingbox?view=windows-app-sdk-1.8)

**Properties:**

- `TopLeft` (object) - Top-left corner coordinates {X, Y}. Maps to [RecognizedTextBoundingBox.TopLeft](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.recognizedtextboundingbox.topleft?view=windows-app-sdk-1.8)
- `TopRight` (object) - Top-right corner coordinates {X, Y}. Maps to [RecognizedTextBoundingBox.TopRight](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.recognizedtextboundingbox.topright?view=windows-app-sdk-1.8)
- `BottomLeft` (object) - Bottom-left corner coordinates {X, Y}. Maps to [RecognizedTextBoundingBox.BottomLeft](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.recognizedtextboundingbox.bottomleft?view=windows-app-sdk-1.8)
- `BottomRight` (object) - Bottom-right corner coordinates {X, Y}. Maps to [RecognizedTextBoundingBox.BottomRight](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.recognizedtextboundingbox.bottomright?view=windows-app-sdk-1.8)

### Content Safety Classes

#### `ContentFilterOptions`

Configuration for content filtering policies. Maps to WinAppSDK [Microsoft.Windows.AI.ContentSafety.ContentFilterOptions](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.contentsafety.contentfilteroptions?view=windows-app-sdk-1.8)

**Properties:**

- `imageMaxAllowedSeverityLevel` (<a href="#imagecontentfilterseverity">ImageContentFilterSeverity</a>) - Maximum allowed severity for image content. Maps to [ContentFilterOptions.ImageMaxAllowedSeverityLevel](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.contentsafety.contentfilteroptions.imagemaxallowedseveritylevel?view=windows-app-sdk-1.8)
- `promptMaxAllowedSeverityLevel` (<a href="#textcontentfilterseverity">TextContentFilterSeverity</a>) - Maximum allowed severity for prompt text. Maps to [ContentFilterOptions.PromptMaxAllowedSeverityLevel](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.contentsafety.contentfilteroptions.promptmaxallowedseveritylevel?view=windows-app-sdk-1.8)
- `responseMaxAllowedSeverityLevel` (<a href="#textcontentfilterseverity">TextContentFilterSeverity</a>) - Maximum allowed severity for response text. Maps to [ContentFilterOptions.ResponseMaxAllowedSeverityLevel](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.contentsafety.contentfilteroptions.responsemaxallowedseveritylevel?view=windows-app-sdk-1.8)

#### `ImageContentFilterSeverity`

Severity levels for different types of image content. Maps to WinAppSDK [Microsoft.Windows.AI.ContentSafety.ImageContentFilterSeverity](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.contentsafety.imagecontentfilterseverity?view=windows-app-sdk-1.8)

**Properties:**

- `AdultContentLevel` (number) - Severity level for adult content. Maps to [ImageContentFilterSeverity.AdultContentLevel](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.contentsafety.imagecontentfilterseverity.adultcontentlevel?view=windows-app-sdk-1.8)
- `GoryContentLevel` (number) - Severity level for gory content. Maps to [ImageContentFilterSeverity.GoryContentLevel](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.contentsafety.imagecontentfilterseverity.gorycontentlevel?view=windows-app-sdk-1.8)
- `RacyContentLevel` (number) - Severity level for racy content. Maps to [ImageContentFilterSeverity.RacyContentLevel](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.contentsafety.imagecontentfilterseverity.racycontentlevel?view=windows-app-sdk-1.8)
- `ViolentContentLevel` (number) - Severity level for violent content. Maps to [ImageContentFilterSeverity.ViolentContentLevel](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.contentsafety.imagecontentfilterseverity.violentcontentlevel?view=windows-app-sdk-1.8)

#### `TextContentFilterSeverity`

Severity levels for different types of text content. Maps to WinAppSDK [Microsoft.Windows.AI.ContentSafety.TextContentFilterSeverity](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.contentsafety.textcontentfilterseverity?view=windows-app-sdk-1.8)

**Properties:**

- `Hate` (number) - Severity level for hate speech. Maps to [TextContentFilterSeverity.Hate](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.contentsafety.textcontentfilterseverity.hate?view=windows-app-sdk-1.8)
- `SelfHarm` (number) - Severity level for self-harm content. Maps to [TextContentFilterSeverity.SelfHarm](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.contentsafety.textcontentfilterseverity.selfharm?view=windows-app-sdk-1.8)
- `Sexual` (number) - Severity level for sexual content. Maps to [TextContentFilterSeverity.Sexual](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.contentsafety.textcontentfilterseverity.sexual?view=windows-app-sdk-1.8)
- `Violent` (number) - Severity level for violent content. Maps to [TextContentFilterSeverity.Violent](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.contentsafety.textcontentfilterseverity.violent?view=windows-app-sdk-1.8)

### Limited Access Features Classes

> [!IMPORTANT]  
> This class is currently needed to be able to use `GenerateResponseAsync`. To use this class, you will need to request an unlock token, please use the [LAF Access Token Request Form](https://go.microsoft.com/fwlink/?linkid=2271232&c1cid=04x409).

#### `LimitedAccessFeatures`

Provides access to Windows limited access features that require special permissions or tokens. Maps to WinAppSDK [Windows.ApplicationModel.LimitedAccessFeatures](https://learn.microsoft.com/en-us/uwp/api/windows.applicationmodel.limitedaccessfeatures?view=winrt-26100)

**Static Methods:**

> [!IMPORTANT]  
> When attempting to gain access to `GenerateResponseAsync`, the feature ID should be `com.microsoft.windows.ai.languagemodel` and the developer signature should be `<insert-app-id> has registered their use of com.microsoft.windows.ai.languagemodel with Microsoft and agrees to the terms of use.`.

- `TryUnlockFeature(string, string, string)` - Attempts to unlock a limited access feature using feature ID, token, and developer signature. Returns <a href="#limitedaccessfeaturerequestresult">LimitedAccessFeatureRequestResult</a>. Maps to [LimitedAccessFeatures.TryUnlockFeature(String, String, String)](https://learn.microsoft.com/en-us/uwp/api/windows.applicationmodel.limitedaccessfeatures.tryunlockfeature?view=winrt-26100)

#### `LimitedAccessFeatureRequestResult`

Contains the result of a limited access feature unlock request. Maps to WinAppSDK [Windows.ApplicationModel.LimitedAccessFeatureRequestResult](https://learn.microsoft.com/en-us/uwp/api/windows.applicationmodel.limitedaccessfeaturerequestresult?view=winrt-26100)

**Properties:**

- `FeatureId` (string) - The identifier of the requested feature. Maps to [LimitedAccessFeatureRequestResult.FeatureId](https://learn.microsoft.com/en-us/uwp/api/windows.applicationmodel.limitedaccessfeaturerequestresult.featureid?view=winrt-26100)
- `Status` (<a href="#limitedaccessfeaturestatus">LimitedAccessFeatureStatus</a>) - The status of the unlock request. Maps to [LimitedAccessFeatureRequestResult.Status](https://learn.microsoft.com/en-us/uwp/api/windows.applicationmodel.limitedaccessfeaturerequestresult.status?view=winrt-26100)
- `EstimatedRemovalDate` (Date | null) - Estimated date when the feature will be removed, if applicable. Maps to [LimitedAccessFeatureRequestResult.EstimatedRemovalDate](https://learn.microsoft.com/en-us/uwp/api/windows.applicationmodel.limitedaccessfeaturerequestresult.estimatedremovaldate?view=winrt-26100)

### Enums and Constants

#### `AIFeatureReadyState`

Maps to WinAppSDK [Microsoft.Windows.AI.AIFeatureReadyState](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.aifeaturereadystate?view=windows-app-sdk-1.8)

- `Ready` (0) - AI features are ready for use. Maps to [AIFeatureReadyState.Ready](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.aifeaturereadystate?view=windows-app-sdk-1.8)
- `NotReady` (1) - AI features are not ready yet. Maps to [AIFeatureReadyState.NotReady](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.aifeaturereadystate?view=windows-app-sdk-1.8)
- `NotSupportedOnCurrentSystem` (2) - AI not supported on this system. Maps to [AIFeatureReadyState.NotSupportedOnCurrentSystem](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.aifeaturereadystate?view=windows-app-sdk-1.8)
- `DisabledByUser` (3) - AI disabled by user settings. Maps to [AIFeatureReadyState.DisabledByUser](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.aifeaturereadystate?view=windows-app-sdk-1.8)

#### `LanguageModelResponseStatus`

Maps to WinAppSDK [Microsoft.Windows.AI.Text.LanguageModelResponseStatus](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.languagemodelresponsestatus?view=windows-app-sdk-1.8)

- `Complete` (0) - Generation completed successfully. Maps to [LanguageModelResponseStatus.Complete](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.languagemodelresponsestatus?view=windows-app-sdk-1.8)
- `InProgress` (1) - Generation is still in progress. Maps to [LanguageModelResponseStatus.InProgress](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.languagemodelresponsestatus?view=windows-app-sdk-1.8)
- `BlockedByPolicy` (2) - Request blocked by policy. Maps to [LanguageModelResponseStatus.BlockedByPolicy](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.languagemodelresponsestatus?view=windows-app-sdk-1.8)
- `PromptLargerThanContext` (3) - Prompt exceeds context length. Maps to [LanguageModelResponseStatus.PromptLargerThanContext](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.languagemodelresponsestatus?view=windows-app-sdk-1.8)
- `PromptBlockedByContentModeration` (4) - Prompt blocked by content filter. Maps to [LanguageModelResponseStatus.PromptBlockedByContentModeration](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.languagemodelresponsestatus?view=windows-app-sdk-1.8)
- `ResponseBlockedByContentModeration` (5) - Response blocked by content filter. Maps to [LanguageModelResponseStatus.ResponseBlockedByContentModeration](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.languagemodelresponsestatus?view=windows-app-sdk-1.8)
- `Error` (6) - An error occurred during generation. Maps to [LanguageModelResponseStatus.Error](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.languagemodelresponsestatus?view=windows-app-sdk-1.8)

#### `SeverityLevel`

Maps to WinAppSDK [Microsoft.Windows.AI.ContentSafety.SeverityLevel](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.contentsafety.severitylevel?view=windows-app-sdk-1.8)

- `Minimum` (10) - Minimal content filtering. Maps to [SeverityLevel.Minimum](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.contentsafety.severitylevel?view=windows-app-sdk-1.8)
- `Low` (11) - Low severity threshold. Maps to [SeverityLevel.Low](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.contentsafety.severitylevel?view=windows-app-sdk-1.8)
- `Medium` (12) - Medium severity threshold. Maps to [SeverityLevel.Medium](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.contentsafety.severitylevel?view=windows-app-sdk-1.8)
- `High` (13) - High severity threshold. Maps to [SeverityLevel.High](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.contentsafety.severitylevel?view=windows-app-sdk-1.8)

#### `AIFeatureReadyResultState`

Maps to WinAppSDK [Microsoft.Windows.AI.AIFeatureReadyResultState](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.aifeaturereadyresultstate?view=windows-app-sdk-1.8)

- `InProgress` (0) - Operation is in progress. Maps to [AIFeatureReadyResultState.InProgress](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.aifeaturereadyresultstate?view=windows-app-sdk-1.8)
- `Success` (1) - Operation completed successfully. Maps to [AIFeatureReadyResultState.Success](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.aifeaturereadyresultstate?view=windows-app-sdk-1.8)
- `Failure` (2) - Operation failed. Maps to [AIFeatureReadyResultState.Failure](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.aifeaturereadyresultstate?view=windows-app-sdk-1.8)

#### `ImageDescriptionKind`

Maps to WinAppSDK [Microsoft.Windows.AI.Imaging.ImageDescriptionKind](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.imagedescriptionkind?view=windows-app-sdk-1.8)

- `BriefDescription` (0) - Generate a brief description for the image. Maps to [ImageDescriptionKind.BriefDescription](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.imagedescriptionkind?view=windows-app-sdk-1.8)
- `DetailedDescription` (1) - Generate a detailed description of the image. Maps to [ImageDescriptionKind.DetailedDescription](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.imagedescriptionkind?view=windows-app-sdk-1.8)
- `DiagramDescription` (2) - Generate a description suitable for diagrams and technical images. Maps to [ImageDescriptionKind.DiagramDescription](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.imagedescriptionkind?view=windows-app-sdk-1.8)
- `AccessibleDescription` (3) - Generate accessible description optimized for screen readers. Maps to [ImageDescriptionKind.AccessibleDescription](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.imagedescriptionkind?view=windows-app-sdk-1.8)

#### `TextRewriteTone`

Maps to WinAppSDK [Microsoft.Windows.AI.Text.TextRewriteTone](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.textrewritetone?view=windows-app-sdk-1.8)

- `Default` (0) - Use the default tone for text rewriting. Maps to [TextRewriteTone.Default](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.textrewritetone?view=windows-app-sdk-1.8)
- `General` (1) - Use a general, neutral tone. Maps to [TextRewriteTone.General](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.textrewritetone?view=windows-app-sdk-1.8)
- `Casual` (2) - Use a casual, informal tone. Maps to [TextRewriteTone.Casual](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.textrewritetone?view=windows-app-sdk-1.8)
- `Formal` (3) - Use a formal, professional tone. Maps to [TextRewriteTone.Formal](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.textrewritetone?view=windows-app-sdk-1.8)

#### `ImageDescriptionResultStatus`

Maps to WinAppSDK [Microsoft.Windows.AI.Imaging.ImageDescriptionResultStatus](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.imagedescriptionresultstatus?view=windows-app-sdk-1.8)

- `Success` (0) - Image description generated successfully. Maps to [ImageDescriptionResultStatus.Success](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.imagedescriptionresultstatus?view=windows-app-sdk-1.8)
- `Failure` (1) - Image description generation failed. Maps to [ImageDescriptionResultStatus.Failure](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.imagedescriptionresultstatus?view=windows-app-sdk-1.8)
- `ContentFiltered` (2) - Image description blocked by content filtering. Maps to [ImageDescriptionResultStatus.ContentFiltered](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.imagedescriptionresultstatus?view=windows-app-sdk-1.8)

#### `RecognizedLineStyle`

Maps to WinAppSDK [Microsoft.Windows.AI.Imaging.RecognizedLineStyle](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.recognizedlinestyle?view=windows-app-sdk-1.8)

- `None` (0) - No specific style detected. Maps to [RecognizedLineStyle.None](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.recognizedlinestyle?view=windows-app-sdk-1.8)
- `Handwritten` (1) - Handwritten text style. Maps to [RecognizedLineStyle.Handwritten](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.recognizedlinestyle?view=windows-app-sdk-1.8)
- `Printed` (2) - Printed text style. Maps to [RecognizedLineStyle.Printed](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.recognizedlinestyle?view=windows-app-sdk-1.8)

#### `LimitedAccessFeatureStatus`

Maps to WinAppSDK [Windows.ApplicationModel.LimitedAccessFeatureStatus](https://learn.microsoft.com/en-us/uwp/api/windows.applicationmodel.limitedaccessfeaturestatus?view=winrt-26100)

- `Available` (0) - Limited access feature is available with proper authentication. Maps to [LimitedAccessFeatureStatus.Available](https://learn.microsoft.com/en-us/uwp/api/windows.applicationmodel.limitedaccessfeaturestatus?view=winrt-26100)
- `AvailableWithoutToken` (1) - Limited access feature is available without requiring a token. Maps to [LimitedAccessFeatureStatus.AvailableWithoutToken](https://learn.microsoft.com/en-us/uwp/api/windows.applicationmodel.limitedaccessfeaturestatus?view=winrt-26100)
- `Unknown` (2) - Limited access feature status is unknown. Maps to [LimitedAccessFeatureStatus.Unknown](https://learn.microsoft.com/en-us/uwp/api/windows.applicationmodel.limitedaccessfeaturestatus?view=winrt-26100)
- `Unavailable` (3) - Limited access feature is not available. Maps to [LimitedAccessFeatureStatus.Unavailable](https://learn.microsoft.com/en-us/uwp/api/windows.applicationmodel.limitedaccessfeaturestatus?view=winrt-26100)


