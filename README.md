# Windows AI Addon for Electron

A Node.js native addon that provides access to Windows AI APIs, enabling Electron applications to leverage AI capabilities directly from Windows with hardware acceleration support.

## Overview

This package wraps the Microsoft Windows AI APIs in a Node.js native addon, allowing Electron applications to:

- Access local Language Models (LLMs)
- Use Content Safety filtering features
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
- `Status` (number) - Status code from the operation

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
- `Style` (number) - Text style information
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

- `Caption` (0) - Generate a brief caption for the image
- `DenseCaption` (1) - Generate a detailed description of the image
- `AltText` (2) - Generate accessible alt text for the image
- `Summary` (3) - Generate a summary description of the image

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

### Streaming Text Generation with Progress (with Options)

```javascript
const windowsAI = require("./windows-ai-addon/build/Release/windows-ai-addon.node");

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
const windowsAI = require("./windows-ai-addon/build/Release/windows-ai-addon.node");

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
```

**Note**: This addon requires Windows 11 with AI platform components installed. Performance and availability may vary based on hardware capabilities and system configuration. Copilot+ PC is recommended for optimal performance.
