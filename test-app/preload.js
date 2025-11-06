const { contextBridge } = require("electron");
const {
  LanguageModel,
  AIFeatureReadyState,
} = require("../index.js");

contextBridge.exposeInMainWorld("windowsAI", {
  summarizeText: async (prompt) => {
    try {
        const languageModel = await LanguageModel.CreateAsync();
        const textSummarizer = new TextSummarizer(languageModel);

        const progressResult = textSummarizer.SummarizeAsync(prompt);

        progressResult.progress((sender, progress) => {
          progressCallback(progress);
        });

        const result = await progressResult;

        return result.Text;

    } catch (error) {
        console.error('Error summarizing text:', error);
    }
},
});