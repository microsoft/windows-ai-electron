const { app, BrowserWindow } = require('electron/main');
const {
  LanguageModel,
  AIFeatureReadyResultState,
  TextSummarizer,
  LanguageModelResponseStatus,
} = require("../index.js");

const summarizeText = async () => {
  try {
    const readyResult = await LanguageModel.EnsureReadyAsync();
    if (readyResult.Status !== AIFeatureReadyResultState.Success) {
      console.log("AI not ready:", readyResult.ErrorDisplayText);
    }

    const model = await LanguageModel.CreateAsync();
    const textSummarizer = new TextSummarizer(model);

    const longText = `
      Artificial intelligence (AI) is intelligence demonstrated by machines, 
      in contrast to the natural intelligence displayed by humans and animals. 
      Leading AI textbooks define the field as the study of "intelligent agents": 
      any device that perceives its environment and takes actions that maximize 
      its chance of successfully achieving its goals.
    `;

    const summarizationPromise = textSummarizer.SummarizeAsync(longText);

    if (summarizationPromise.progress) {
      summarizationPromise.progress((error, progressText) => {
        console.log(progressText);
      });
    }

    const result = await summarizationPromise;

    if (result.Status === LanguageModelResponseStatus.Complete) {
      console.log(result.Text);
    } else {
      console.log("Summarization failed with status:", result.Status);
    }
  } catch (error) {
    console.error("Error:", error);
  }
}

const createWindow = () => {
  const win = new BrowserWindow({
    width: 800,
    height: 600,
    autoHideMenuBar: true,
    webPreferences: {
      preload: __dirname + '/preload.js',
      sandbox: false,
    }
  })

  win.loadFile('index.html')

  win.webContents.openDevTools();

  summarizeText();
}

app.whenReady().then(() => {
  createWindow()

  app.on('activate', () => {
    if (BrowserWindow.getAllWindows().length === 0) {
      createWindow()
    }
  })
})

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit()
  }
})