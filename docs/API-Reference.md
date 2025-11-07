# Supported API's

This package supports many of the API's within the [Microsoft.Windows.AI.Text](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text?view=windows-app-sdk-1.8), [Microsoft.Windows.AI.Imaging](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging?view=windows-app-sdk-1.8), and [Microsoft.Windows.AI](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai?view=windows-app-sdk-1.8) namespaces.

Below is the list of supported API's for the `winapp-windows-ai` package. For usage information, see the [Usage Guide](Usage.md)

### Core Classes

#### `LanguageModel`

Main class for text generation and AI model interactions.

**Static Methods:**

- `CreateAsync()` - Asynchronously creates a new LanguageModel instance
- `GetReadyState()` - Returns the current AI feature ready state
- `EnsureReadyAsync()` - Ensures AI features are ready for use

**Instance Methods:**

- `GenerateResponseAsync(string, LanguageModelOptions?)` - Generates text response from a prompt

#### `LanguageModelOptions`

Configuration options for language model behavior.

**Properties:**

- `Temperature` (number) - Controls randomness in generation (0.0-1.0)
- `TopK` (number) - Limits vocabulary for next token selection
- `TopP` (number) - Nucleus sampling threshold (0.0-1.0)
- `ContentFilterOptions` (ContentFilterOptions) - Content safety settings

#### `LanguageModelResponseResult`

Contains the result of a text generation operation.

**Properties:**

- `Text` (string) - The generated text response
- `Status` (number) - Status code from LanguageModelResponseStatus enum
- `ExtendedError` (string) - Extended error information if available

#### `TextSummarizer`

Main class for AI-powered text and conversation summarization.

**Constructor:**

- `new TextSummarizer(LanguageModel)` - Creates a TextSummarizer with the provided LanguageModel instance

**Instance Methods:**

- `SummarizeAsync(string)` - Asynchronously summarizes the provided text
- `SummarizeParagraphAsync(string)` - Asynchronously summarizes a paragraph with paragraph-specific optimization
- `SummarizeConversationAsync(ConversationItem[], ConversationSummaryOptions)` - Asynchronously summarizes a conversation from an array of ConversationItem objects
- `IsPromptLargerThanContext(ConversationItem[], ConversationSummaryOptions)` - Checks if conversation prompt exceeds context window

#### `ConversationItem`

Represents a single message in a conversation for summarization.

**Constructor:**

- `new ConversationItem()` - Creates a new conversation item

**Properties:**

- `Message` (string) - The message content
- `Participant` (string) - The participant who sent the message (e.g., "User", "Assistant", "Support")

#### `ConversationSummaryOptions`

Configuration options for conversation summarization.

**Properties:**

- `includeMessageCitations` (boolean) - Whether to include references to specific messages in the summary
- `includeParticipantAttribution` (boolean) - Whether to attribute parts of the summary to specific participants

#### `TextRewriter`

Main class for AI-powered text rewriting and tone adjustment.

**Constructor:**

- `new TextRewriter(LanguageModel)` - Creates a TextRewriter with the provided LanguageModel instance

**Instance Methods:**

- `RewriteAsync(string)` - Asynchronously rewrites the provided text using the default tone
- `RewriteAsync(string, TextRewriteTone)` - Asynchronously rewrites the provided text using the specified TextRewriteTone

#### `TextToTableConverter`

Main class for AI-powered conversion of text content into structured table format.

**Constructor:**

- `new TextToTableConverter(LanguageModel)` - Creates a TextToTableConverter with the provided LanguageModel instance

**Instance Methods:**

- `ConvertAsync(string)` - Asynchronously converts the provided text into a structured table format, returns TextToTableResponseResult

#### `TextToTableResponseResult`

Result object containing structured table data from text conversion operations.

**Properties:**

- `ExtendedError` (string) - Extended error information as hex string (e.g., "0x80004005") if available
- `Status` (number) - Status code from the conversion operation

**Methods:**

- `GetRows()` - Returns an array of TextToTableRow objects representing the table structure

#### `TextToTableRow`

Represents a single row in a converted table structure.

**Methods:**

- `GetColumns()` - Returns an array of strings representing the column values for this row

#### `AIFeatureReadyResult`

Result object for AI feature readiness operations.

**Properties:**

- `Error` (number) - Error code if operation failed
- `ErrorDisplayText` (string) - Human-readable error description
- `ExtendedError` (number) - Extended error code
- `Status` (number) - Operation status from AIFeatureReadyResultState enum

### Imaging Classes

#### `ImageDescriptionGenerator`

Main class for generating descriptions and captions for images.

**Static Methods:**

- `CreateAsync()` - Asynchronously creates a new ImageDescriptionGenerator instance
- `GetReadyState()` - Returns the current AI feature ready state for image description
- `EnsureReadyAsync()` - Ensures image description features are ready for use

**Instance Methods:**

- `DescribeAsync(string, ImageDescriptionKind, ContentFilterOptions)` - Generates description for an image, file path must be the absolute path to the image
- `Close()` - Closes the generator and releases resources

#### `ImageDescriptionResult`

Contains the result of an image description operation.

**Properties:**

- `Description` (string) - The generated image description
- `Status` (ImageDescriptionResultStatus) - Status code from ImageDescriptionResultStatus enum

#### `TextRecognizer`

Main class for optical character recognition (OCR) on images.

**Static Methods:**

- `CreateAsync()` - Asynchronously creates a new TextRecognizer instance
- `GetReadyState()` - Returns the current AI feature ready state for text recognition
- `EnsureReadyAsync()` - Ensures text recognition features are ready for use

**Instance Methods:**

- `RecognizeTextFromImageAsync(string)` - Asynchronously recognizes text in an image, file path must be the absolute path to the image
- `RecognizeTextFromImage(string)` - Synchronously recognizes text in an image, file path must be the absolute path to the image
- `Close()` - Closes the recognizer and releases resources
- `Dispose()` - Disposes the recognizer and cleans up resources

#### `RecognizedText`

Contains the complete text recognition results from an image.

**Properties:**

- `Lines` (RecognizedLine[]) - Array of recognized text lines
- `TextAngle` (number) - Angle of the text in the image

#### `RecognizedLine`

Represents a single line of recognized text.

**Properties:**

- `BoundingBox` (RecognizedTextBoundingBox) - Bounding box coordinates for the line
- `Style` (RecognizedLineStyle) - Text style information from RecognizedLineStyle enum
- `LineStyleConfidence` (number) - Confidence level for the line style
- `Text` (string) - The recognized text content
- `Words` (RecognizedWord[]) - Array of individual words in the line

#### `RecognizedWord`

Represents a single recognized word.

**Properties:**

- `BoundingBox` (RecognizedTextBoundingBox) - Bounding box coordinates for the word
- `MatchConfidence` (number) - Confidence level for the word recognition
- `Text` (string) - The recognized word text

#### `RecognizedTextBoundingBox`

Defines the bounding box coordinates for recognized text elements.

**Properties:**

- `TopLeft` (object) - Top-left corner coordinates {X, Y}
- `TopRight` (object) - Top-right corner coordinates {X, Y}
- `BottomLeft` (object) - Bottom-left corner coordinates {X, Y}
- `BottomRight` (object) - Bottom-right corner coordinates {X, Y}

### Content Safety Classes

#### `ContentFilterOptions`

Configuration for content filtering policies.

**Properties:**

- `imageMaxAllowedSeverityLevel` (ImageContentFilterSeverity) - Maximum allowed severity for image content
- `promptMaxAllowedSeverityLevel` (TextContentFilterSeverity) - Maximum allowed severity for prompt text
- `responseMaxAllowedSeverityLevel` (TextContentFilterSeverity) - Maximum allowed severity for response text

#### `ImageContentFilterSeverity`

Severity levels for different types of image content.

**Properties:**

- `AdultContentLevel` (number) - Severity level for adult content
- `GoryContentLevel` (number) - Severity level for gory content
- `RacyContentLevel` (number) - Severity level for racy content
- `ViolentContentLevel` (number) - Severity level for violent content

#### `TextContentFilterSeverity`

Severity levels for different types of text content.

**Properties:**

- `Hate` (number) - Severity level for hate speech
- `SelfHarm` (number) - Severity level for self-harm content
- `Sexual` (number) - Severity level for sexual content
- `Violent` (number) - Severity level for violent content

### Enums and Constants

#### `AIFeatureReadyState`

- `Ready` (0) - AI features are ready for use
- `NotReady` (1) - AI features are not ready yet
- `NotSupportedOnCurrentSystem` (2) - AI not supported on this system
- `DisabledByUser` (3) - AI disabled by user settings

#### `LanguageModelResponseStatus`

- `Complete` (0) - Generation completed successfully
- `InProgress` (1) - Generation is still in progress
- `BlockedByPolicy` (2) - Request blocked by policy
- `PromptLargerThanContext` (3) - Prompt exceeds context length
- `PromptBlockedByContentModeration` (4) - Prompt blocked by content filter
- `ResponseBlockedByContentModeration` (5) - Response blocked by content filter
- `Error` (6) - An error occurred during generation

#### `SeverityLevel`

- `Minimum` (10) - Minimal content filtering
- `Low` (11) - Low severity threshold
- `Medium` (12) - Medium severity threshold
- `High` (13) - High severity threshold

#### `AIFeatureReadyResultState`

- `InProgress` (0) - Operation is in progress
- `Success` (1) - Operation completed successfully
- `Failure` (2) - Operation failed

#### `ImageDescriptionKind`

- `BriefDescription` (0) - Generate a brief description for the image
- `DetailedDescription` (1) - Generate a detailed description of the image
- `DiagramDescription` (2) - Generate a description suitable for diagrams and technical images
- `AccessibleDescription` (3) - Generate accessible description optimized for screen readers

#### `TextRewriteTone`

- `Default` (0) - Use the default tone for text rewriting
- `General` (1) - Use a general, neutral tone
- `Casual` (2) - Use a casual, informal tone
- `Formal` (3) - Use a formal, professional tone

#### `ImageDescriptionResultStatus`

- `Success` (0) - Image description generated successfully
- `Failure` (1) - Image description generation failed
- `ContentFiltered` (2) - Image description blocked by content filtering

#### `RecognizedLineStyle`

- `None` (0) - No specific style detected
- `Handwritten` (1) - Handwritten text style
- `Printed` (2) - Printed text style
