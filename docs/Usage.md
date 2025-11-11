## Usage Examples

Below are a few usage examples of the APIs in this module. For more samples on how to use the `winapp-windows-ai`, see [Electron Gallery](https://github.com/microsoft/electron-gallery). For more samples on how to use Windows AI Foundry, see [AI Dev Gallery](https://github.com/microsoft/ai-dev-Gallery).

### Basic Language Model Usage

```javascript
const windowsAI = require("winapp-windows-ai");

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
const windowsAI = require("winapp-windows-ai");

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
const windowsAI = require("winapp-windows-ai");

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
const windowsAI = require("winapp-windows-ai");

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
const windowsAI = require("winapp-windows-ai");

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
const windowsAI = require("winapp-windows-ai");

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
const windowsAI = require("winapp-windows-ai");

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
const windowsAI = require("winapp-windows-ai");

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
const windowsAI = require("winapp-windows-ai");

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
const windowsAI = require("winapp-windows-ai");

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
const windowsAI = require("winapp-windows-ai");

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
const windowsAI = require("winapp-windows-ai");

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
const windowsAI = require("winapp-windows-ai");

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
const windowsAI = require("winapp-windows-ai");

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
const windowsAI = require("winapp-windows-ai");

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
