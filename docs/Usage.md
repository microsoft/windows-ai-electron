## Usage Examples

Below are a few usage examples of the APIs in this module. For more samples on how to use the `@microsoft/windows-ai-electron`, see [Electron Gallery](https://github.com/microsoft/electron-gallery). For more samples on how to use Windows AI Foundry, see [AI Dev Gallery](https://github.com/microsoft/ai-dev-Gallery).

### Text Summarization

```javascript
const {
  LanguageModel,
  AIFeatureReadyResultState,
  TextSummarizer,
  LanguageModelResponseStatus,
} = require("@microsoft/windows-ai-electron");

async function summarizeText() {
  try {
    // Ensure AI is ready
    const readyResult = await LanguageModel.EnsureReadyAsync();
    if (readyResult.Status !== AIFeatureReadyResultState.Success) {
      console.log("AI not ready:", readyResult.ErrorDisplayText);
      return "";
    }

    // Create language model and text summarizer
    const model = await LanguageModel.CreateAsync();
    const textSummarizer = new TextSummarizer(model);

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

    if (result.Status === LanguageModelResponseStatus.Complete) {
      console.log("Original text length:", longText.length);
      console.log("Summary:", result.Text);
      console.log("Summary length:", result.Text.length);
      return result.Text;
    } else {
      console.log("Summarization failed with status:", result.Status);
      return "";
    }
  } catch (error) {
    console.error("Error:", error);
  }
}
```

### Text Summarization with Progress Tracking

```javascript
const {
  LanguageModel,
  AIFeatureReadyResultState,
  TextSummarizer,
  LanguageModelResponseStatus,
} = require("@microsoft/windows-ai-electron");

async function summarizeParagraphWithProgress(callbackFunction) {
  try {
    const readyResult = await LanguageModel.EnsureReadyAsync();
    if (readyResult.Status !== AIFeatureReadyResultState.Success) {
      console.log("AI not ready:", readyResult.ErrorDisplayText);
      return "";
    }

    const model = await LanguageModel.CreateAsync();
    const textSummarizer = new TextSummarizer(model);

    const paragraph = `
      Machine learning is a method of data analysis that automates analytical 
      model building. It is a branch of artificial intelligence based on the 
      idea that systems can learn from data, identify patterns and make 
      decisions with minimal human intervention. Through algorithms and 
      statistical models, machine learning systems improve their performance 
      on a specific task through experience without being explicitly programmed.
    `;

    // Start summarization with progress tracking
    const summarizationPromise = textSummarizer.SummarizeAsync(paragraph);

    // Track progress if supported
    if (summarizationPromise.progress) {
      summarizationPromise.progress((error, progressText) => {
        callbackFunction(progressText);
      });
    }

    const result = await summarizationPromise;

    if (result.Status === LanguageModelResponseStatus.Complete) {
      return result.Text;
    } else {
      return "";
    }
  } catch (error) {
    console.error("Error:", error);
  }
}
```

### Conversation Summarization

```javascript
const {
  LanguageModel,
  AIFeatureReadyResultState,
  TextSummarizer,
  ConversationItem,
  LanguageModelResponseStatus,
} = require("@microsoft/windows-ai-electron");

async function summarizeConversation() {
  try {
    // Setup AI components
    const readyResult = await LanguageModel.EnsureReadyAsync();
    if (readyResult.Status !== AIFeatureReadyResultState.Success) {
      console.log("AI not ready:", readyResult.ErrorDisplayText);
      return "";
    }

    const model = await LanguageModel.CreateAsync();
    const textSummarizer = new TextSummarizer(model);

    // Create conversation items
    const conversation = [];

    const item1 = new ConversationItem();
    item1.Message =
      "Hello, I'm having trouble with my computer. It keeps freezing when I try to open large files.";
    item1.Participant = "User";
    conversation.push(item1);

    const item2 = new ConversationItem();
    item2.Message =
      "I'd be happy to help you with that. Can you tell me what type of files you're trying to open and how large they are?";
    item2.Participant = "Support";
    conversation.push(item2);

    const item3 = new ConversationItem();
    item3.Message =
      "They're mostly video files, around 2-3 GB each. My computer has 8GB of RAM.";
    item3.Participant = "User";
    conversation.push(item3);

    const item4 = new ConversationItem();
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

    if (result.Status === LanguageModelResponseStatus.Complete) {
      return result.Text;
    } else {
      console.log(
        "Conversation summarization failed with status:",
        result.Status
      );
      return "";
    }
  } catch (error) {
    console.error("Error:", error);
  }
}
```

### Checking Prompt Context Size

```javascript
const { LanguageModel, TextSummarizer } = require("@microsoft/windows-ai-electron");

async function checkConversationSize() {
  try {
    const model = await LanguageModel.CreateAsync();
    const textSummarizer = new TextSummarizer(model);

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
```

### Text Generation

> [!IMPORTANT]  
> `GenerateResponseAsync` uses a Windows API which is a part of a [Limited Access Feature](https://learn.microsoft.com/en-us/uwp/api/windows.applicationmodel.limitedaccessfeatures?view=winrt-26100). To request an unlock token, please use the [LAF Access Token Request Form](https://go.microsoft.com/fwlink/?linkid=2271232&c1cid=04x409). To use this method, you must first call [LimitedAccessFeature.TryUnlockToken](#limitedaccessfeatures).

```javascript
const {
  LimitedAccessFeatures,
  LimitedAccessFeatureStatus,
  LanguageModel,
} = require("@microsoft/windows-ai-electron");

async function generateTextWithProgress(progressCallback) {
  try {
    const access = LimitedAccessFeatures.TryUnlockFeature(
      "com.microsoft.windows.ai.languagemodel",
      LAF_TOKEN,
      "<APP_ID> has registered their use of com.microsoft.windows.ai.languagemodel with Microsoft and agrees to the terms of use."
    );
    if (
      access.Status == LimitedAccessFeatureStatus.Available ||
      access.Status == LimitedAccessFeatureStatus.AvailableWithoutToken
    ) {
      var languageModel = await LanguageModel.CreateAsync();
      if (languageModel) {
        var progressResult = languageModel.GenerateResponseAsync(prompt);
        progressResult.progress((sender, progress) => {
          progressCallback(progress);
        });
        var result = await progressResult;
        return result.Text;
      } else {
        return "Language Model is not ready. Please check that your device meets the requirements to use Phi Silica.";
      }
    } else {
      return "You need an access token to use this Language Model feature.";
    }
  } catch (error) {
    console.error("Error during streaming generation:", error);
  }
}
```

### Text Generation with LanguageModelOptions

> [!IMPORTANT]  
> `GenerateResponseAsync` uses a Windows API which is a part of a [Limited Access Feature](https://learn.microsoft.com/en-us/uwp/api/windows.applicationmodel.limitedaccessfeatures?view=winrt-26100). To request an unlock token, please use the [LAF Access Token Request Form](https://go.microsoft.com/fwlink/?linkid=2271232&c1cid=04x409). To use this method, you must first call [LimitedAccessFeature.TryUnlockToken](#limitedaccessfeatures).

```javascript
const {
  LimitedAccessFeatures,
  LimitedAccessFeatureStatus,
  LanguageModel,
  AIFeatureReadyResultState,
  LanguageModelOptions,
  LanguageModelResponseStatus,
} = require("@microsoft/windows-ai-electron");

async function generateTextWithProgress(progressCallback) {
  try {
    const access = LimitedAccessFeatures.TryUnlockFeature(
      "com.microsoft.windows.ai.languagemodel",
      LAF_TOKEN,
      "<APP_ID> has registered their use of com.microsoft.windows.ai.languagemodel with Microsoft and agrees to the terms of use."
    );
    if (
      access.Status == LimitedAccessFeatureStatus.Available ||
      access.Status == LimitedAccessFeatureStatus.AvailableWithoutToken
    ) {
      // Ensure AI is ready
      const readyResult = await LanguageModel.EnsureReadyAsync();
      if (readyResult.Status !== AIFeatureReadyResultState.Success) {
        return "AI not Ready";
      }

      // Create language model instance
      const model = await LanguageModel.CreateAsync();

      // Create options
      const options = new LanguageModelOptions();
      options.Temperature = 0.8;
      options.TopP = 0.95;

      // Start generation with progress tracking
      const prompt = "What is artificial intelligence?";
      const generationPromise = model.GenerateResponseAsync(prompt, options);

      // Set up progress handler to stream partial results
      generationPromise.progress((error, progressText) => {
        progressResult.progress((sender, progress) => {
          progressCallback(progress); // Callback to caller to process text stream
        });
      });

      const finalResult = await generationPromise;

      if (finalResult.Status === LanguageModelResponseStatus.Complete) {
        return finalResult.Text;
      } else {
        console.log("Generation ended with status:", finalResult.Status);
        return "Unable to generate text.";
      }
    }
  } catch (error) {
    console.error("Error during streaming generation:", error);
  }
}
```

### Content Safety Configuration

```javascript
const {
  TextContentFilterSeverity,
  ImageContentFilterSeverity,
  ContentFilterOptions,
  LanguageModelOptions,
} = require("@microsoft/windows-ai-electron");

function setupContentFiltering() {
  // Create text content severity settings
  const textSeverity = new TextContentFilterSeverity();
  textSeverity.Hate = SeverityLevel.Medium;
  textSeverity.Sexual = SeverityLevel.High;
  textSeverity.Violent = SeverityLevel.Medium;
  textSeverity.SelfHarm = SeverityLevel.High;

  // Create image content severity settings
  const imageSeverity = new ImageContentFilterSeverity();
  imageSeverity.AdultContentLevel = SeverityLevel.High;
  imageSeverity.ViolentContentLevel = SeverityLevel.Medium;
  imageSeverity.RacyContentLevel = SeverityLevel.Medium;
  imageSeverity.GoryContentLevel = SeverityLevel.High;

  // Create content filter options
  const contentFilter = new ContentFilterOptions();
  contentFilter.promptMaxAllowedSeverityLevel = textSeverity;
  contentFilter.responseMaxAllowedSeverityLevel = textSeverity;
  contentFilter.imageMaxAllowedSeverityLevel = imageSeverity;

  // Apply to language model options
  const modelOptions = new LanguageModelOptions();
  modelOptions.ContentFilterOptions = contentFilter;

  return modelOptions;
}
```

### Checking AI Feature Availability

```javascript
const { LanguageModel, AIFeatureReadyState } = require("@microsoft/windows-ai-electron");

async function checkAIAvailability() {
  try {
    // Check specific ready state
    const state = LanguageModel.GetReadyState();
    switch (state) {
      case AIFeatureReadyState.Ready:
        console.log("AI is ready for use");
        break;
      case AIFeatureReadyState.NotReady:
        console.log("AI is not ready yet");
        break;
      case AIFeatureReadyState.NotSupportedOnCurrentSystem:
        console.log("AI not supported on this system");
        break;
      case AIFeatureReadyState.DisabledByUser:
        console.log("AI disabled by user settings");
        break;
    }
  } catch (error) {
    console.error("Error checking AI availability:", error);
  }
}
```

### Text Rewriting

```javascript
const {
  LanguageModel,
  AIFeatureReadyResultState,
  TextRewriter,
  TextRewriteTone,
  LanguageModelResponseStatus,
} = require("@microsoft/windows-ai-electron");

async function rewriteText() {
  try {
    // Setup AI components
    const readyResult = await LanguageModel.EnsureReadyAsync();
    if (readyResult.Status !== AIFeatureReadyResultState.Success) {
      return "AI Not Ready";
    }

    const model = await LanguageModel.CreateAsync();
    const textRewriter = new TextRewriter(model);

    // Text to rewrite
    const originalText =
      "Hey there! Just wanted to let you know that the meeting got pushed back to 3pm. Hope that works for everyone!";

    // Formal tone
    const formalResult = await textRewriter.RewriteAsync(
      originalText,
      TextRewriteTone.Formal
    );
    if (formalResult.Status === LanguageModelResponseStatus.Complete) {
      return formalResult.Text;
    } else {
      return "Unable to rewrite text";
    }
  } catch (error) {
    console.error("Error:", error);
  }
}
```

### Text Rewriting with Progress Tracking

```javascript
const {
  LanguageModel,
  AIFeatureReadyResultState,
  TextRewriter,
  TextRewriteTone,
  LanguageModelResponseStatus,
} = require("@microsoft/windows-ai-electron");

async function rewriteTextWithProgress(progressCallback) {
  try {
    // Setup AI components
    const readyResult = await LanguageModel.EnsureReadyAsync();
    if (readyResult.Status !== AIFeatureReadyResultState.Success) {
      return "AI not ready";
    }

    const model = await LanguageModel.CreateAsync();
    const textRewriter = new TextRewriter(model);

    const originalText = `
      The quarterly business review meeting has been scheduled for next Tuesday at 2:00 PM 
      in the main conference room. All department heads are expected to attend and present 
      their Q3 results and Q4 projections. Please bring your laptops and ensure all 
      presentation materials are ready beforehand. Coffee and snacks will be provided. 
      If you have any scheduling conflicts, please reach out to HR immediately so we can 
      make alternative arrangements. This is a mandatory meeting for all senior staff members.
    `;

    // Create the promise with progress tracking
    const rewritePromise = textRewriter.RewriteAsync(
      originalText,
      TextRewriteTone.Formal
    );

    // Set up progress tracking
    rewritePromise.progress((error, progressText) => {
      progressCallback(progressText);
    });

    // Wait for completion
    const result = await rewritePromise;

    if (result.Status === LanguageModelResponseStatus.Complete) {
      return result.Text;
    } else {
      return "Unable to rewrite text.";
    }
  } catch (error) {
    console.error("Error:", error);
  }
}
```

### Text to Table Conversion

```javascript
const {
  LanguageModel,
  AIFeatureReadyResultState,
  TextToTableConverter,
} = require("@microsoft/windows-ai-electron");

async function convertTextToTable() {
  try {
    // Setup AI components
    const readyResult = await LanguageModel.EnsureReadyAsync();
    if (readyResult.Status !== AIFeatureReadyResultState.Success) {
      console.log("AI not ready:", readyResult.ErrorDisplayText);
      return;
    }

    const model = await LanguageModel.CreateAsync();
    const tableConverter = new TextToTableConverter(model);

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

    // Convert text to table format
    const result = await tableConverter.ConvertAsync(textData);

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
```

### Image Description Generation

```javascript
const {
  ImageDescriptionGenerator,
  AIFeatureReadyResultState,
  ContentFilterOptions,
  ImageDescriptionKind,
} = require("@microsoft/windows-ai-electron");

async function generateImageDescription() {
  try {
    // Check if image description features are ready
    const readyResult = await ImageDescriptionGenerator.EnsureReadyAsync();
    if (readyResult.Status !== AIFeatureReadyResultState.Success) {
      return "ImageDescriptionGenerator not ready";
    }

    // Create image description generator
    const generator = await ImageDescriptionGenerator.CreateAsync();

    // Set up content filtering
    const contentFilter = new ContentFilterOptions();
    // Configure content filtering as needed...

    // Generate description with progress tracking
    const imagePath = "C:\\path\\to\\your\\image.jpg";
    const result = await generator.DescribeAsync(
      imagePath,
      ImageDescriptionKind.BriefDescription,
      contentFilter
    );

    generator.Close();
    return result.Description;
  } catch (error) {
    console.error("Error generating image description:", error);
  }
}
```

### Optical Character Recognition (OCR)

```javascript
const {
  TextRecognizer,
  AIFeatureReadyResultState,
} = require("@microsoft/windows-ai-electron");

async function recognizeTextFromImage() {
  try {
    // Check if OCR features are ready
    const readyResult = await TextRecognizer.EnsureReadyAsync();
    if (readyResult.Status !== AIFeatureReadyResultState.Success) {
      console.log("OCR not ready:", ErrorDisplayText);
      return;
    }

    // Create text recognizer
    const recognizer = await TextRecognizer.CreateAsync();

    // Recognize text from image
    const imagePath = "C:\\path\\to\\your\\document.jpg";
    const result = await recognizer.RecognizeTextFromImageAsync(imagePath);

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
```

### Synchronous OCR for Better Performance

```javascript
const {
  TextRecognizer,
  AIFeatureReadyResultState,
} = require("@microsoft/windows-ai-electron");

async function quickTextRecognition() {
  try {
    // Create recognizer
    const recognizer = await TextRecognizer.CreateAsync();

    // Use synchronous method for faster processing
    const imagePath = "C:\\path\\to\\your\\document.jpg";
    const result = recognizer.RecognizeTextFromImage(imagePath);

    // Extract all text as a single string
    const allText = result.Lines.map((line) => line.Text).join("\n");
    console.log("Extracted text:\n", allText);

    // Clean up resources
    recognizer.Close();
  } catch (error) {
    console.error("Error in quick text recognition:", error);
  }
}
```
