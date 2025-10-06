# Windows AI Addon for Electron

A Node.js native addon that provides access to Windows AI APIs, enabling Electron applications to leverage AI capabilities directly from Windows with hardware acceleration support.

## Overview

This package wraps the Microsoft Windows AI APIs in a Node.js native addon, allowing Electron applications to:

- Access local Language Models (LLMs)
- Use Content Safety filtering features
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

#### `AIFeatureReadyResult`

Result object for AI feature readiness operations.

**Properties:**

- `Error` (number) - Error code if operation failed
- `ErrorDisplayText` (string) - Human-readable error description
- `ExtendedError` (number) - Extended error code
- `Status` (number) - Operation status from AIFeatureReadyResultState enum

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

## Usage Examples

### Basic Language Model Usage

```javascript
const windowsAI = require("./windows-ai-addon/build/Release/windows-ai-addon.node");

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

    // Create language model options (optional)
    const options = new windowsAI.LanguageModelOptions();
    options.Temperature = 0.7;
    options.TopP = 0.9;
    options.TopK = 40;

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

### Content Safety Configuration

```javascript
const windowsAI = require("./windows-ai-addon/build/Release/windows-ai-addon.node");

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
const windowsAI = require("./windows-ai-addon/build/Release/windows-ai-addon.node");

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

## Error Handling

Always check response status and handle potential errors:

```javascript
const windowsAI = require("./windows-ai-addon/build/Release/windows-ai-addon.node");

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
│   ├── LanguageModelProjections.h   # Language model API wrappers
│   ├── LanguageModelProjections.cpp
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

**Note**: This addon requires Windows 11 with AI platform components installed. Performance and availability may vary based on hardware capabilities and system configuration.
