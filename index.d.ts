// Type definitions for electron-windows-ai-addon
// Project: https://github.com/microsoft/electron-windows-ai-addon
// Definitions by: Microsoft Corporation

declare module "electron-windows-ai-addon" {
  
  // =============================
  // Enums and Constants
  // =============================
  
  export enum AIFeatureReadyState {
    Ready = 0,
    NotReady = 1,
    NotSupportedOnCurrentSystem = 2,
    DisabledByUser = 3
  }
  
  export enum LanguageModelResponseStatus {
    Complete = 0,
    InProgress = 1,
    BlockedByPolicy = 2,
    PromptLargerThanContext = 3,
    PromptBlockedByContentModeration = 4,
    ResponseBlockedByContentModeration = 5,
    Error = 6
  }
  
  export enum SeverityLevel {
    Minimum = 10,
    Low = 11,
    Medium = 12,
    High = 13
  }
  
  export enum AIFeatureReadyResultState {
    InProgress = 0,
    Success = 1,
    Failure = 2
  }
  
  export enum ImageDescriptionKind {
    BriefDescription = 0,
    DetailedDescription = 1,
    DiagramDescription = 2,
    AccessibleDescription = 3
  }

  export enum TextRewriteTone {
    Default = 0,
    General = 1,
    Casual = 2,
    Formal = 3
  }

  export enum ImageDescriptionResultStatus {
    Success = 0,
    Failure = 1,
    ContentFiltered = 2
  }

  export enum RecognizedLineStyle {
    None = 0,
    Handwritten = 1,
    Printed = 2
  }
  
  // =============================
  // Progress Promise Interface
  // =============================
  
  interface ProgressPromise<T> extends Promise<T> {
    progress(callback: (error: Error | null, progress: string) => void): this;
  }
  
  // =============================
  // Language Model Classes
  // =============================
  
  export class LanguageModel {
    static CreateAsync(): Promise<LanguageModel>;
    static GetReadyState(): AIFeatureReadyState;
    static EnsureReadyAsync(): ProgressPromise<AIFeatureReadyResult>;
    
    GenerateResponseAsync(prompt: string, options?: LanguageModelOptions): ProgressPromise<LanguageModelResponseResult>;
    Close(): void;
  }
  
  export class LanguageModelOptions {
    constructor();
    Temperature?: number;
    TopK?: number;
    TopP?: number;
    ContentFilterOptions?: ContentFilterOptions;
  }
  
  export class LanguageModelResponseResult {
    readonly Text: string;
    readonly Status: LanguageModelResponseStatus;
    readonly ExtendedError?: string;
  }
  
  export class AIFeatureReadyResult {
    readonly Error: number;
    readonly ErrorDisplayText: string;
    readonly ExtendedError: number;
    readonly Status: AIFeatureReadyResultState;
  }

  export class ConversationItem {
    constructor(message?: string, participant?: string);
    Message: string;
    Participant: string;
  }

  // =============================
  // Text Intelligence Classes
  // =============================

  export class TextSummarizer {
    constructor(languageModel: LanguageModel);
    
    SummarizeAsync(text: string): ProgressPromise<LanguageModelResponseResult>;
    SummarizeConversationAsync(conversationItems: ConversationItem[]): ProgressPromise<LanguageModelResponseResult>;
    SummarizeParagraphAsync(text: string): ProgressPromise<LanguageModelResponseResult>;
    IsPromptLargerThanContext(text: string): boolean;
  }

  export class TextRewriter {
    constructor(languageModel: LanguageModel);
    
    RewriteAsync(text: string): ProgressPromise<LanguageModelResponseResult>;
    RewriteAsync(text: string, tone: TextRewriteTone): ProgressPromise<LanguageModelResponseResult>;
  }
  
  // =============================
  // Content Safety Classes
  // =============================
  
  export class ContentFilterOptions {
    constructor();
    imageMaxAllowedSeverityLevel?: ImageContentFilterSeverity;
    promptMaxAllowedSeverityLevel?: TextContentFilterSeverity;
    responseMaxAllowedSeverityLevel?: TextContentFilterSeverity;
  }
  
  export class ImageContentFilterSeverity {
    constructor();
    AdultContentLevel?: SeverityLevel;
    GoryContentLevel?: SeverityLevel;
    RacyContentLevel?: SeverityLevel;
    ViolentContentLevel?: SeverityLevel;
  }
  
  export class TextContentFilterSeverity {
    constructor();
    Hate?: SeverityLevel;
    SelfHarm?: SeverityLevel;
    Sexual?: SeverityLevel;
    Violent?: SeverityLevel;
  }
  
  // =============================
  // Imaging Classes
  // =============================
  
  export class ImageDescriptionGenerator {
    static CreateAsync(): Promise<ImageDescriptionGenerator>;
    static GetReadyState(): AIFeatureReadyState;
    static EnsureReadyAsync(): ProgressPromise<AIFeatureReadyResult>;
    
    DescribeAsync(
      filePath: string,
      descriptionKind: ImageDescriptionKind,
      contentFilterOptions: ContentFilterOptions
    ): ProgressPromise<ImageDescriptionResult>;
    Close(): void;
  }
  
  export class ImageDescriptionResult {
    readonly Description: string;
    readonly Status: ImageDescriptionResultStatus;
  }
  
  export class TextRecognizer {
    static CreateAsync(): Promise<TextRecognizer>;
    static GetReadyState(): AIFeatureReadyState;
    static EnsureReadyAsync(): ProgressPromise<AIFeatureReadyResult>;
    
    RecognizeTextFromImageAsync(filePath: string): Promise<RecognizedText>;
    RecognizeTextFromImage(filePath: string): RecognizedText;
    Close(): void;
    Dispose(): void;
  }
  
  export class RecognizedText {
    readonly Lines: RecognizedLine[];
    readonly TextAngle: number;
  }
  
  export class RecognizedLine {
    readonly BoundingBox: RecognizedTextBoundingBox;
    readonly Style: RecognizedLineStyle;
    readonly LineStyleConfidence: number;
    readonly Text: string;
    readonly Words: RecognizedWord[];
  }
  
  export class RecognizedWord {
    readonly BoundingBox: RecognizedTextBoundingBox;
    readonly MatchConfidence: number;
    readonly Text: string;
  }
  
  export interface Point {
    X: number;
    Y: number;
  }
  
  export class RecognizedTextBoundingBox {
    readonly TopLeft: Point;
    readonly TopRight: Point;
    readonly BottomLeft: Point;
    readonly BottomRight: Point;
  }
  
  export class ImageObjectRemover {
    static CreateAsync(): Promise<ImageObjectRemover>;
    static GetReadyState(): AIFeatureReadyState;
    static EnsureReadyAsync(): Promise<AIFeatureReadyResult>;
    
    RemoveAsync(...args: any[]): Promise<any>; // Implementation pending
    Close(): void;
  }
  
  export class ImageScaler {
    static CreateAsync(): Promise<ImageScaler>;
    static GetReadyState(): AIFeatureReadyState;
    static EnsureReadyAsync(): Promise<AIFeatureReadyResult>;
    
    ScaleAsync(...args: any[]): Promise<any>; // Implementation pending
    Close(): void;
  }
  
  export class ImageObjectExtractor {
    static CreateAsync(): Promise<ImageObjectExtractor>;
    static GetReadyState(): AIFeatureReadyState;
    static EnsureReadyAsync(): Promise<AIFeatureReadyResult>;
    
    ExtractAsync(...args: any[]): Promise<any>; // Implementation pending
    Close(): void;
  }
  
  export class ImageObjectExtractorHint {
    constructor();
    X: number;
    Y: number;
    Width: number;
    Height: number;
  }
  
  // =============================
  // Module Properties
  // =============================
  
  export const version: string;
  
  // =============================
  // Default Export (for require())
  // =============================
  
  const windowsAIAddon: {
    // Enums
    AIFeatureReadyState: typeof AIFeatureReadyState;
    LanguageModelResponseStatus: typeof LanguageModelResponseStatus;
    SeverityLevel: typeof SeverityLevel;
    AIFeatureReadyResultState: typeof AIFeatureReadyResultState;
    ImageDescriptionKind: typeof ImageDescriptionKind;
    TextRewriteTone: typeof TextRewriteTone;
    ImageDescriptionResultStatus: typeof ImageDescriptionResultStatus;
    RecognizedLineStyle: typeof RecognizedLineStyle;
    
    // Language Model Classes
    LanguageModel: typeof LanguageModel;
    LanguageModelOptions: typeof LanguageModelOptions;
    LanguageModelResponseResult: typeof LanguageModelResponseResult;
    AIFeatureReadyResult: typeof AIFeatureReadyResult;
    ConversationItem: typeof ConversationItem;
    
    // Text Intelligence Classes
    TextSummarizer: typeof TextSummarizer;
    TextRewriter: typeof TextRewriter;
    
    // Content Safety Classes
    ContentFilterOptions: typeof ContentFilterOptions;
    ImageContentFilterSeverity: typeof ImageContentFilterSeverity;
    TextContentFilterSeverity: typeof TextContentFilterSeverity;
    
    // Imaging Classes
    ImageDescriptionGenerator: typeof ImageDescriptionGenerator;
    ImageDescriptionResult: typeof ImageDescriptionResult;
    TextRecognizer: typeof TextRecognizer;
    RecognizedText: typeof RecognizedText;
    RecognizedLine: typeof RecognizedLine;
    RecognizedWord: typeof RecognizedWord;
    RecognizedTextBoundingBox: typeof RecognizedTextBoundingBox;
    ImageObjectRemover: typeof ImageObjectRemover;
    ImageScaler: typeof ImageScaler;
    ImageObjectExtractor: typeof ImageObjectExtractor;
    ImageObjectExtractorHint: typeof ImageObjectExtractorHint;
    
    // Module Properties
    version: string;
  };
  
  export = windowsAIAddon;
}