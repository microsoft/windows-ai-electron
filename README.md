# Windows AI Addon for Electron

A Node.js native addon that provides access to Windows AI APIs, enabling Electron applications to leverage AI capabilities directly from Windows with hardware acceleration support.

## Overview

This package wraps the Microsoft Windows AI APIs in a Node.js native addon, allowing Electron applications to:

- Access local Language Models (LLMs)
- Use Content Safety filtering features
- Summarize text and conversations with AI
- Rewrite text with different tones (formal, casual, general)
- Convert unstructured text into structured table formats
- Generate image descriptions and captions
- Perform optical character recognition (OCR) on images
- Remove objects from images
- Scale and resize images with AI
- Check AI feature readiness and availability
- Configure AI model options and parameters

## Prerequisites

- **Windows 11** (22H2 or later)
- **Node.js** 18.x or later
- **Visual Studio 2022** with C++ build tools
- **Python** 3.x
- **Yarn** package manager
- **Copilot+PC**

## Installation and Building

### 1. Clone the Repository

```bash
git clone https://github.com/microsoft/electron-windows-ai-addon.git
```

### 2. Build and Package winsdk Package

```bash
git clone https://github.com/microsoft/winsdk.git
cd .\winsdk\src\winsdk-npm
npm i
npm run build
npm pack
```

### 3. Install Dependencies

```bash
cd \<path to electron-windows-ai-addon repo\>
yarn install
yarn winsdk restore
```

### 4. Build the Native Addon

```bash
yarn build-windows-ai-addon
```

### 5. Locally Package Addon

```bash
npm pack
```

## Supported APIs

### Core Classes

#### `LanguageModel`

Main class for text generation and AI model interactions.

**Static Methods:**

- `CreateAsync()` - Asynchronously creates a new LanguageModel instance
- `GetReadyState()` - Returns the current AI feature ready state
- `EnsureReadyAsync()` - Ensures AI features are ready for use

**Instance Methods:**

- `GenerateResponseAsync(prompt, options?)` - Generates text response from a prompt

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

- `new TextSummarizer(languageModel)` - Creates a TextSummarizer with the provided LanguageModel instance

**Instance Methods:**

- `SummarizeAsync(text)` - Asynchronously summarizes the provided text
- `SummarizeParagraphAsync(text)` - Asynchronously summarizes a paragraph with paragraph-specific optimization
- `SummarizeConversationAsync(messages, options)` - Asynchronously summarizes a conversation from an array of ConversationItem objects
- `IsPromptLargerThanContext(messages, options)` - Checks if conversation prompt exceeds context window

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

- `new TextRewriter(languageModel)` - Creates a TextRewriter with the provided LanguageModel instance

**Instance Methods:**

- `RewriteAsync(text)` - Asynchronously rewrites the provided text using the default tone
- `RewriteAsync(text, tone)` - Asynchronously rewrites the provided text using the specified TextRewriteTone

#### `TextToTableConverter`

Main class for AI-powered conversion of text content into structured table format.

**Constructor:**

- `new TextToTableConverter(languageModel)` - Creates a TextToTableConverter with the provided LanguageModel instance

**Instance Methods:**

- `ConvertAsync(text)` - Asynchronously converts the provided text into a structured table format, returns TextToTableResponseResult

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

- `DescribeAsync(filePath, descriptionKind, contentFilterOptions)` - Generates description for an image
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

- `RecognizeTextFromImageAsync(filePath)` - Asynchronously recognizes text in an image
- `RecognizeTextFromImage(filePath)` - Synchronously recognizes text in an image
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

#### `ImageObjectRemover` [IN PROGRESS - NOT READY FOR USE]

Class for removing objects from images using AI.

**Static Methods:**

- `CreateAsync()` - Asynchronously creates a new ImageObjectRemover instance
- `GetReadyState()` - Returns the current AI feature ready state
- `EnsureReadyAsync()` - Ensures object removal features are ready for use

**Instance Methods:**

- `RemoveAsync(...)` - Removes objects from an image (implementation pending)
- `Close()` - Closes the remover and releases resources

#### `ImageScaler` [IN PROGRESS - NOT READY FOR USE]

Class for scaling and resizing images with AI enhancement.

**Static Methods:**

- `CreateAsync()` - Asynchronously creates a new ImageScaler instance
- `GetReadyState()` - Returns the current AI feature ready state
- `EnsureReadyAsync()` - Ensures image scaling features are ready for use

**Instance Methods:**

- `ScaleAsync(...)` - Scales an image with AI enhancement (implementation pending)
- `Close()` - Closes the scaler and releases resources

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

## Usage Examples

Below are a few usage examples of the APIs in this module. For more samples on how to use the electron-windows-ai-addon, see https://github.com/microsoft/electron-gallery. For more samples on how to use Windows AI Foundry, see https://github.com/microsoft/ai-dev-Gallery.

### Basic Language Model Usage

```javascript
const windowsAI = require("electron-windows-ai-addon");

async function generateText() {
  try {
    // Check if AI features are ready
    const readyResult = await windowsAI.LanguageModel.EnsureReadyAsync();
    if (readyResult.Status !== windowsAI.AIFeatureReadyResultState.Success) {
      console.log("AI not ready:", readyResult.ErrorDisplayText);
      return;
    }

    // Create language model instance
    const model = await windowsAI.LanguageModel.CreateAsync();

    // Generate response
    const result = await model.GenerateResponseAsync(
      "What is artificial intelligence?",
      options
    );

    if (result.Status === windowsAI.LanguageModelResponseStatus.Complete) {
      console.log("Generated text:", result.Text);
    } else {
      console.log("Generation failed with status:", result.Status);
      if (result.ExtendedError) {
        console.log("Extended error:", result.ExtendedError);
      }
    }
  } catch (error) {
    console.error("Error:", error);
  }
}

generateText();
```

### Text Summarization

```javascript
const windowsAI = require("electron-windows-ai-addon");

async function summarizeText() {
  try {
    // Ensure AI is ready
    const readyResult = await windowsAI.LanguageModel.EnsureReadyAsync();
    if (readyResult.Status !== windowsAI.AIFeatureReadyResultState.Success) {
      console.log("AI not ready:", readyResult.ErrorDisplayText);
      return;
    }

    // Create language model and text summarizer
    const model = await windowsAI.LanguageModel.CreateAsync();
    const textSummarizer = new windowsAI.TextSummarizer(model);

    // Text to summarize
    const longText = `
      Artificial intelligence (AI) is intelligence demonstrated by machines, 
      in contrast to the natural intelligence displayed by humans and animals. 
      Leading AI textbooks define the field as the study of "intelligent agents": 
      any device that perceives its environment and takes actions that maximize 
      its chance of successfully achieving its goals. Colloquially, the term 
      "artificial intelligence" is often used to describe machines that mimic 
      "cognitive" functions that humans associate with the human mind, such as 
      "learning" and "problem solving".
    `;

    // Summarize the text
    const result = await textSummarizer.SummarizeAsync(longText);

    if (result.Status === windowsAI.LanguageModelResponseStatus.Complete) {
      console.log("Original text length:", longText.length);
      console.log("Summary:", result.Text);
      console.log("Summary length:", result.Text.length);
    } else {
      console.log("Summarization failed with status:", result.Status);
    }
  } catch (error) {
    console.error("Error:", error);
  }
}

summarizeText();
```

### Conversation Summarization

```javascript
const windowsAI = require("electron-windows-ai-addon");

async function summarizeConversation() {
  try {
    // Setup AI components
    const readyResult = await windowsAI.LanguageModel.EnsureReadyAsync();
    if (readyResult.Status !== windowsAI.AIFeatureReadyResultState.Success) {
      console.log("AI not ready:", readyResult.ErrorDisplayText);
      return;
    }

    const model = await windowsAI.LanguageModel.CreateAsync();
    const textSummarizer = new windowsAI.TextSummarizer(model);

    // Create conversation items
    const conversation = [];

    const item1 = new windowsAI.ConversationItem();
    item1.Message =
      "Hello, I'm having trouble with my computer. It keeps freezing when I try to open large files.";
    item1.Participant = "User";
    conversation.push(item1);

    const item2 = new windowsAI.ConversationItem();
    item2.Message =
      "I'd be happy to help you with that. Can you tell me what type of files you're trying to open and how large they are?";
    item2.Participant = "Support";
    conversation.push(item2);

    const item3 = new windowsAI.ConversationItem();
    item3.Message =
      "They're mostly video files, around 2-3 GB each. My computer has 8GB of RAM.";
    item3.Participant = "User";
    conversation.push(item3);

    const item4 = new windowsAI.ConversationItem();
    item4.Message =
      "That could be a memory issue. Try closing other applications before opening large video files, and consider upgrading your RAM if this happens frequently.";
    item4.Participant = "Support";
    conversation.push(item4);

    // Conversation summary options
    const options = {
      includeMessageCitations: true,
      includeParticipantAttribution: true,
    };

    // Summarize the conversation
    const result = await textSummarizer.SummarizeConversationAsync(
      conversation,
      options
    );

    if (result.Status === windowsAI.LanguageModelResponseStatus.Complete) {
      console.log("Conversation Summary:", result.Text);
    } else {
      console.log(
        "Conversation summarization failed with status:",
        result.Status
      );
    }
  } catch (error) {
    console.error("Error:", error);
  }
}

summarizeConversation();
```

### Paragraph Summarization with Progress Tracking

```javascript
const windowsAI = require("electron-windows-ai-addon");

async function summarizeParagraphWithProgress() {
  try {
    const readyResult = await windowsAI.LanguageModel.EnsureReadyAsync();
    if (readyResult.Status !== windowsAI.AIFeatureReadyResultState.Success) {
      console.log("AI not ready:", readyResult.ErrorDisplayText);
      return;
    }

    const model = await windowsAI.LanguageModel.CreateAsync();
    const textSummarizer = new windowsAI.TextSummarizer(model);

    const paragraph = `
      Machine learning is a method of data analysis that automates analytical 
      model building. It is a branch of artificial intelligence based on the 
      idea that systems can learn from data, identify patterns and make 
      decisions with minimal human intervention. Through algorithms and 
      statistical models, machine learning systems improve their performance 
      on a specific task through experience without being explicitly programmed.
    `;

    // Start summarization with progress tracking
    const summarizationPromise =
      textSummarizer.SummarizeParagraphAsync(paragraph);

    // Track progress if supported
    if (summarizationPromise.progress) {
      summarizationPromise.progress((error, progressText) => {
        if (error) {
          console.error("Progress error:", error);
          return;
        }
        console.log("Progress:", progressText);
      });
    }

    const result = await summarizationPromise;

    if (result.Status === windowsAI.LanguageModelResponseStatus.Complete) {
      console.log("Paragraph Summary:", result.Text);
    }
  } catch (error) {
    console.error("Error:", error);
  }
}

summarizeParagraphWithProgress();
```

### Checking Prompt Context Size

```javascript
const windowsAI = require("electron-windows-ai-addon");

async function checkConversationSize() {
  try {
    const model = await windowsAI.LanguageModel.CreateAsync();
    const textSummarizer = new windowsAI.TextSummarizer(model);

    // Create a conversation
    const conversation = [
      /* your conversation items */
    ];
    const options = {
      includeMessageCitations: false,
      includeParticipantAttribution: true,
    };

    // Check if the conversation is too large for the context window
    const sizeCheck = textSummarizer.IsPromptLargerThanContext(
      conversation,
      options
    );

    console.log("Is conversation larger than context:", sizeCheck.isLarger);
    console.log("Cutoff position:", sizeCheck.cutoffPosition);

    if (sizeCheck.isLarger) {
      console.log(
        "Consider truncating the conversation or using a sliding window approach"
      );
    }
  } catch (error) {
    console.error("Error checking conversation size:", error);
  }
}

checkConversationSize();
```

### Streaming Text Generation with Progress (with Options)

```javascript
const windowsAI = require("electron-windows-ai-addon");

async function generateTextWithProgress(progressCallback) {
  try {
    // Ensure AI is ready
    const readyResult = await windowsAI.LanguageModel.EnsureReadyAsync();
    if (readyResult.Status !== windowsAI.AIFeatureReadyResultState.Success) {
      console.log("AI not ready:", readyResult.ErrorDisplayText);
      return;
    }

    // Create language model instance
    const model = await windowsAI.LanguageModel.CreateAsync();

    // Create options
    const options = new windowsAI.LanguageModelOptions();
    options.Temperature = 0.8;
    options.TopP = 0.95;

    // Start generation with progress tracking
    const prompt = "What is artificial intelligence?";
    const generationPromise = model.GenerateResponseAsync(prompt, options);

    // Set up progress handler to stream partial results
    generationPromise.progress((error, progressText) => {
      if (error) {
        console.error("Progress error:", error);
        return;
      }

      progressResult.progress((sender, progress) => {
        progressCallback(progress); // Callback to caller to process text stream
      });
    });

    const finalResult = await generationPromise;

    if (finalResult.Status === windowsAI.LanguageModelResponseStatus.Complete) {
      console.log("\n=== Final Complete Text ===");
      console.log(finalResult.Text);
    } else {
      console.log("Generation ended with status:", finalResult.Status);
    }
  } catch (error) {
    console.error("Error during streaming generation:", error);
  }
}
```

### Content Safety Configuration

```javascript
const windowsAI = require("electron-windows-ai-addon");

function setupContentFiltering() {
  // Create text content severity settings
  const textSeverity = new windowsAI.TextContentFilterSeverity();
  textSeverity.Hate = windowsAI.SeverityLevel.Medium;
  textSeverity.Sexual = windowsAI.SeverityLevel.High;
  textSeverity.Violent = windowsAI.SeverityLevel.Medium;
  textSeverity.SelfHarm = windowsAI.SeverityLevel.High;

  // Create image content severity settings
  const imageSeverity = new windowsAI.ImageContentFilterSeverity();
  imageSeverity.AdultContentLevel = windowsAI.SeverityLevel.High;
  imageSeverity.ViolentContentLevel = windowsAI.SeverityLevel.Medium;
  imageSeverity.RacyContentLevel = windowsAI.SeverityLevel.Medium;
  imageSeverity.GoryContentLevel = windowsAI.SeverityLevel.High;

  // Create content filter options
  const contentFilter = new windowsAI.ContentFilterOptions();
  contentFilter.promptMaxAllowedSeverityLevel = textSeverity;
  contentFilter.responseMaxAllowedSeverityLevel = textSeverity;
  contentFilter.imageMaxAllowedSeverityLevel = imageSeverity;

  // Apply to language model options
  const modelOptions = new windowsAI.LanguageModelOptions();
  modelOptions.ContentFilterOptions = contentFilter;

  return modelOptions;
}
```

### Checking AI Feature Availability

```javascript
const windowsAI = require("electron-windows-ai-addon");

async function checkAIAvailability() {
  try {
    // Check specific ready state
    const state = windowsAI.LanguageModel.GetReadyState();
    switch (state) {
      case windowsAI.AIFeatureReadyState.Ready:
        console.log("AI is ready for use");
        break;
      case windowsAI.AIFeatureReadyState.NotReady:
        console.log("AI is not ready yet");
        break;
      case windowsAI.AIFeatureReadyState.NotSupportedOnCurrentSystem:
        console.log("AI not supported on this system");
        break;
      case windowsAI.AIFeatureReadyState.DisabledByUser:
        console.log("AI disabled by user settings");
        break;
    }
  } catch (error) {
    console.error("Error checking AI availability:", error);
  }
}

checkAIAvailability();
```

### Text Rewriting

```javascript
const windowsAI = require("electron-windows-ai-addon");

async function rewriteText() {
  try {
    // Setup AI components
    const readyResult = await windowsAI.LanguageModel.EnsureReadyAsync();
    if (readyResult.Status !== windowsAI.AIFeatureReadyResultState.Success) {
      console.log("AI not ready:", readyResult.ErrorDisplayText);
      return;
    }

    const model = await windowsAI.LanguageModel.CreateAsync();
    const textRewriter = new windowsAI.TextRewriter(model);

    // Text to rewrite
    const originalText =
      "Hey there! Just wanted to let you know that the meeting got pushed back to 3pm. Hope that works for everyone!";

    // Rewrite with default tone
    console.log("Original text:", originalText);
    console.log("\\n--- Rewriting with different tones ---\\n");

    // Default tone
    const defaultResult = await textRewriter.RewriteAsync(originalText);
    if (
      defaultResult.Status === windowsAI.LanguageModelResponseStatus.Complete
    ) {
      console.log("Default tone:", defaultResult.Text);
    }

    // Formal tone
    const formalResult = await textRewriter.RewriteAsync(
      originalText,
      windowsAI.TextRewriteTone.Formal
    );
    if (
      formalResult.Status === windowsAI.LanguageModelResponseStatus.Complete
    ) {
      console.log("Formal tone:", formalResult.Text);
    }

    // Casual tone
    const casualResult = await textRewriter.RewriteAsync(
      originalText,
      windowsAI.TextRewriteTone.Casual
    );
    if (
      casualResult.Status === windowsAI.LanguageModelResponseStatus.Complete
    ) {
      console.log("Casual tone:", casualResult.Text);
    }

    // General tone
    const generalResult = await textRewriter.RewriteAsync(
      originalText,
      windowsAI.TextRewriteTone.General
    );
    if (
      generalResult.Status === windowsAI.LanguageModelResponseStatus.Complete
    ) {
      console.log("General tone:", generalResult.Text);
    }
  } catch (error) {
    console.error("Error:", error);
  }
}

rewriteText();
```

### Text Rewriting with Progress Tracking

```javascript
const windowsAI = require("electron-windows-ai-addon");

async function rewriteTextWithProgress(progressCallback) {
  try {
    // Setup AI components
    const readyResult = await windowsAI.LanguageModel.EnsureReadyAsync();
    if (readyResult.Status !== windowsAI.AIFeatureReadyResultState.Success) {
      console.log("AI not ready:", readyResult.ErrorDisplayText);
      return;
    }

    const model = await windowsAI.LanguageModel.CreateAsync();
    const textRewriter = new windowsAI.TextRewriter(model);

    // Longer text to rewrite (to better demonstrate progress)
    const originalText = `
      The quarterly business review meeting has been scheduled for next Tuesday at 2:00 PM 
      in the main conference room. All department heads are expected to attend and present 
      their Q3 results and Q4 projections. Please bring your laptops and ensure all 
      presentation materials are ready beforehand. Coffee and snacks will be provided. 
      If you have any scheduling conflicts, please reach out to HR immediately so we can 
      make alternative arrangements. This is a mandatory meeting for all senior staff members.
    `;

    console.log("Original text:", originalText.trim());
    console.log("\n--- Rewriting with progress tracking ---\n");

    // Create the promise with progress tracking
    const rewritePromise = textRewriter.RewriteAsync(
      originalText,
      windowsAI.TextRewriteTone.Formal
    );

    // Set up progress tracking
    rewritePromise.progress((error, progressText) => {
      progressCallback(progressText);
    });

    // Wait for completion
    const result = await rewritePromise;

    if (result.Status === windowsAI.LanguageModelResponseStatus.Complete) {
      console.log("\n--- Final Result ---");
      console.log("Rewritten text:", result.Text);
    } else {
      console.log("Rewriting failed with status:", result.Status);
      if (result.ExtendedError) {
        console.log("Extended error:", result.ExtendedError);
      }
    }
  } catch (error) {
    console.error("Error:", error);
  }
}

rewriteTextWithProgress(progressCallback);
```

### Text to Table Conversion

```javascript
const windowsAI = require("electron-windows-ai-addon");

async function convertTextToTable() {
  try {
    // Setup AI components
    const readyResult = await windowsAI.LanguageModel.EnsureReadyAsync();
    if (readyResult.Status !== windowsAI.AIFeatureReadyResultState.Success) {
      console.log("AI not ready:", readyResult.ErrorDisplayText);
      return;
    }

    const model = await windowsAI.LanguageModel.CreateAsync();
    const tableConverter = new windowsAI.TextToTableConverter(model);

    // Sample text data to convert to table format
    const textData = `
      Sales Report for Q3 2024:
      - Product A sold 150 units at $25 each for total revenue of $3750
      - Product B sold 200 units at $40 each for total revenue of $8000  
      - Product C sold 75 units at $60 each for total revenue of $4500
      - Product D sold 300 units at $15 each for total revenue of $4500
      Total units sold: 725
      Total revenue: $20750
    `;

    console.log("Original text:", textData.trim());
    console.log("\\n--- Converting to table format ---\\n");

    // Convert text to table format
    const result = await tableConverter.ConvertAsync(textData);

    console.log("\\n--- Table Conversion Results ---");
    console.log("Status:", result.Status);
    if (result.ExtendedError) {
      console.log("Extended Error:", result.ExtendedError);
    }

    // Get the structured table data
    const rows = result.GetRows();
    console.log(`\\nExtracted ${rows.length} rows:`);

    rows.forEach((row, rowIndex) => {
      const columns = row.GetColumns();
      console.log(`Row ${rowIndex + 1}: [${columns.join(", ")}]`);
    });
  } catch (error) {
    console.error("Error:", error);
  }
}

convertTextToTable();
```

### Text to Table Conversion with Progress

```javascript
const windowsAI = require("electron-windows-ai-addon");

async function convertTextToTableWithProgress() {
  try {
    // Setup AI components
    const readyResult = await windowsAI.LanguageModel.EnsureReadyAsync();
    if (readyResult.Status !== windowsAI.AIFeatureReadyResultState.Success) {
      console.log("AI not ready:", readyResult.ErrorDisplayText);
      return;
    }

    const model = await windowsAI.LanguageModel.CreateAsync();
    const tableConverter = new windowsAI.TextToTableConverter(model);

    // Complex data that would benefit from table structure
    const complexTextData = `
      Annual Financial Report 2024:
      
      Revenue by Quarter:
      Q1 had total sales of $125,000 with 1,250 transactions and average order value of $100
      Q2 showed growth with $156,000 in sales, 1,400 transactions, average order $111.43
      Q3 continued upward trend: $189,000 total sales, 1,620 transactions, $116.67 average order
      Q4 finished strong with $203,000 in sales, 1,750 transactions, and $116 average order value
      
      Department Performance:
      Marketing department spent $45,000 and generated 2,100 leads with conversion rate of 12%
      Sales team closed 252 deals worth $673,000 total value
      Customer Service handled 8,500 tickets with 95% satisfaction rating
      Operations reduced costs by 8% while maintaining quality metrics above 98%
      
      Regional Breakdown:
      North region: 35% of sales, 2,200 customers, $115 average transaction
      South region: 28% of sales, 1,890 customers, $124 average transaction  
      East region: 22% of sales, 1,456 customers, $119 average transaction
      West region: 15% of sales, 998 customers, $108 average transaction
    `;

    console.log("Original complex text data:");
    console.log(complexTextData.trim());
    console.log(
      "\\n--- Converting to structured table format with progress ---\\n"
    );

    // Create the promise with progress tracking
    const convertPromise = tableConverter.ConvertAsync(complexTextData);

    // Set up progress tracking
    convertPromise.progress((error, progressText) => {
      if (error) {
        console.error("Progress error:", error);
      } else {
        console.log("Conversion progress:", progressText);
      }
    });

    // Wait for completion
    const result = await convertPromise;

    console.log("\\n--- Structured Table Results ---");
    console.log("Status:", result.Status);
    if (result.ExtendedError) {
      console.log("Extended Error:", result.ExtendedError);
    }

    // Process the structured table data
    const rows = result.GetRows();
    console.log(
      `\\nSuccessfully extracted ${rows.length} rows of structured data:`
    );

    // Display as formatted table
    rows.forEach((row, rowIndex) => {
      const columns = row.GetColumns();
      if (rowIndex === 0) {
        console.log("\\n" + "=".repeat(80));
        console.log("TABLE DATA:");
        console.log("=".repeat(80));
      }
      console.log(
        `Row ${String(rowIndex + 1).padStart(2)}: ${columns
          .map((col) => `"${col}"`)
          .join(" | ")}`
      );
    });

    console.log("=".repeat(80));
  } catch (error) {
    console.error("Error:", error);
  }
}

convertTextToTableWithProgress();
```

### Image Description Generation

```javascript
const windowsAI = require("electron-windows-ai-addon");

async function generateImageDescription() {
  try {
    // Check if image description features are ready
    const readyResult =
      await windowsAI.ImageDescriptionGenerator.EnsureReadyAsync();
    if (readyResult.Status !== windowsAI.AIFeatureReadyResultState.Success) {
      console.log("Image description not ready:", readyResult.ErrorDisplayText);
      return;
    }

    // Create image description generator
    const generator = await windowsAI.ImageDescriptionGenerator.CreateAsync();

    // Set up content filtering
    const contentFilter = new windowsAI.ContentFilterOptions();
    // Configure content filtering as needed...

    // Generate description with progress tracking
    const imagePath = "C:\\path\\to\\your\\image.jpg";
    const descriptionPromise = generator.DescribeAsync(
      imagePath,
      windowsAI.ImageDescriptionKind.BriefDescription,
      contentFilter
    );

    // Track progress
    descriptionPromise.progress((error, progressText) => {
      if (error) {
        console.error("Progress error:", error);
        return;
      }
      console.log("Progress:", progressText);
    });

    // Get final result
    const result = await descriptionPromise;

    console.log("Image description:", result.Description);
    console.log("Status:", result.Status);

    // Clean up
    generator.Close();
  } catch (error) {
    console.error("Error generating image description:", error);
  }
}

generateImageDescription();
```

### Optical Character Recognition (OCR)

```javascript
const windowsAI = require("electron-windows-ai-addon");

async function recognizeTextFromImage() {
  try {
    // Check if OCR features are ready
    const readyResult = await windowsAI.TextRecognizer.EnsureReadyAsync();
    if (readyResult.Status !== windowsAI.AIFeatureReadyResultState.Success) {
      console.log("OCR not ready:", readyResult.ErrorDisplayText);
      return;
    }

    // Create text recognizer
    const recognizer = await windowsAI.TextRecognizer.CreateAsync();

    // Recognize text from image
    const imagePath = "C:\\path\\to\\your\\document.jpg";
    const result = await recognizer.RecognizeTextFromImageAsync(imagePath);

    console.log("Text angle:", result.TextAngle);
    console.log("Number of lines:", result.Lines.length);

    // Process each recognized line
    result.Lines.forEach((line, lineIndex) => {
      console.log(`\nLine ${lineIndex + 1}:`);
      console.log("  Text:", line.Text);
      console.log("  Style confidence:", line.LineStyleConfidence);
      console.log("  Bounding box:", {
        topLeft: line.BoundingBox.TopLeft,
        topRight: line.BoundingBox.TopRight,
        bottomLeft: line.BoundingBox.BottomLeft,
        bottomRight: line.BoundingBox.BottomRight,
      });

      // Process individual words
      line.Words.forEach((word, wordIndex) => {
        console.log(`    Word ${wordIndex + 1}:`, word.Text);
        console.log(`      Confidence: ${word.MatchConfidence}`);
      });
    });

    // Clean up
    recognizer.Close();
  } catch (error) {
    console.error("Error recognizing text:", error);
  }
}

recognizeTextFromImage();
```

### Synchronous OCR for Better Performance

```javascript
const windowsAI = require("electron-windows-ai-addon");

async function quickTextRecognition() {
  try {
    // Create recognizer
    const recognizer = await windowsAI.TextRecognizer.CreateAsync();

    // Use synchronous method for faster processing
    const imagePath = "C:\\path\\to\\your\\document.jpg";
    const result = recognizer.RecognizeTextFromImage(imagePath);

    // Extract all text as a single string
    const allText = result.Lines.map((line) => line.Text).join("\n");
    console.log("Extracted text:\n", allText);

    // Clean up resources
    recognizer.Dispose(); // Use Dispose for complete cleanup
  } catch (error) {
    console.error("Error in quick text recognition:", error);
  }
}

quickTextRecognition();
```

## Error Handling

Always check response status and handle potential errors:

```javascript
const windowsAI = require("electron-windows-ai-addon");

async function robustGeneration(prompt) {
  try {
    // Ensure AI is ready
    const readyResult = await windowsAI.LanguageModel.EnsureReadyAsync();
    if (readyResult.State !== windowsAI.AIFeatureReadyResultState.Success) {
      throw new Error(`AI not ready: ${readyResult.ErrorDisplayText}`);
    }

    // Create model and generate
    const model = new windowsAI.LanguageModel();
    const result = await model.GenerateAsync(prompt);

    // Handle different response statuses
    switch (result.Status) {
      case windowsAI.LanguageModelResponseStatus.Complete:
        return result.Text;

      case windowsAI.LanguageModelResponseStatus
        .PromptBlockedByContentModeration:
        throw new Error("Prompt was blocked by content moderation");

      case windowsAI.LanguageModelResponseStatus
        .ResponseBlockedByContentModeration:
        throw new Error("Response was blocked by content moderation");

      case windowsAI.LanguageModelResponseStatus.PromptLargerThanContext:
        throw new Error("Prompt is too long for the model context");

      case windowsAI.LanguageModelResponseStatus.Error:
        throw new Error(
          `Model error: ${result.ExtendedError || "Unknown error"}`
        );

      default:
        throw new Error(`Unexpected status: ${result.Status}`);
    }
  } catch (error) {
    console.error("Generation failed:", error.message);
    throw error;
  }
}
```

## Development

### Project Structure

```
electron-windows-ai-addon/
├── windows-ai-addon/
│   ├── windows-ai-addon.cc          # Main addon entry point
│   ├── LanguageModelProjections.h   # Language model & text summarization API wrappers
│   ├── LanguageModelProjections.cpp
│   ├── ImagingProjections.h         # Imaging API wrappers
│   ├── ImagingProjections.cpp
│   ├── ContentSeverity.h            # Content safety API wrappers
│   ├── ContentSeverity.cpp
│   ├── ProjectionHelper.h           # Utility functions
│   ├── ProjectionHelper.cpp
│   └── binding.gyp                  # Build configuration
├── package.json
└── README.md
```

## License

## This project is licensed under the MIT License - see the LICENSE file for details.

## Supported File Formats

### Image Formats (for OCR and Description)

- **JPEG** (.jpg, .jpeg)
- **PNG** (.png)
- **BMP** (.bmp)
- **TIFF** (.tiff, .tif)
- **GIF** (.gif)

## Performance Considerations

### Language Model

- First-time model loading may take several seconds
- Subsequent generations are typically faster
- Progress callbacks provide real-time generation status

### Imaging APIs

- OCR performance varies with image quality and text clarity
- Image description generation requires good hardware acceleration
- Synchronous OCR methods (`RecognizeTextFromImage`) are faster for single operations
- Asynchronous methods (`RecognizeTextFromImageAsync`) are better for UI responsiveness

## Troubleshooting

### Common Issues

1. **"AI not ready" errors**: Ensure Windows 11 22H2+ and Copilot+ PC requirements are met
2. **Image file not found**: Use absolute file paths with proper Windows path separators
3. **Content moderation blocks**: Adjust `ContentFilterOptions` severity levels as appropriate
4. **Memory issues**: Always call `Close()` or `Dispose()` methods to clean up resources

### Debug Tips

```javascript
// Check feature availability before use
const checkAllFeatures = () => {
  const features = {
    "Language Model": windowsAI.LanguageModel.GetReadyState(),
    "Image Description": windowsAI.ImageDescriptionGenerator.GetReadyState(),
    "Text Recognition": windowsAI.TextRecognizer.GetReadyState(),
    "Object Remover": windowsAI.ImageObjectRemover.GetReadyState(),
    "Image Scaler": windowsAI.ImageScaler.GetReadyState(),
  };

  Object.entries(features).forEach(([name, state]) => {
    const status =
      state === windowsAI.AIFeatureReadyState.Ready
        ? "✓ Ready"
        : state === windowsAI.AIFeatureReadyState.NotReady
        ? "⚠ Not Ready"
        : state === windowsAI.AIFeatureReadyState.NotSupportedOnCurrentSystem
        ? "✗ Not Supported"
        : "✗ Disabled";
    console.log(`${name}: ${status}`);
  });
};

// Test TextSummarizer functionality
const testSummarization = async () => {
  try {
    const model = await windowsAI.LanguageModel.CreateAsync();
    const summarizer = new windowsAI.TextSummarizer(model);

    const testText = "This is a test sentence for summarization functionality.";
    const result = await summarizer.SummarizeAsync(testText);

    console.log("TextSummarizer test successful:", result.Text);
    return true;
  } catch (error) {
    console.error("TextSummarizer test failed:", error);
    return false;
  }
};
```

**Note**: This addon requires Windows 11 with AI platform components installed. Performance and availability may vary based on hardware capabilities and system configuration. Copilot+ PC is recommended for optimal performance.
