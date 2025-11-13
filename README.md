<h1 align="center">
  <span>Windows AI Addon for Electron</span>
</h1>
<h3 align="center">
  <span>Leverage Windows AI capabilities directly from your app's JavaScript</span>
</h3>

> [!IMPORTANT]
> :warning: **Status: Public Preview** - The Windows AI Addon is experimental and in active development. We'd love your feedback! Share your thoughts by creating an [issue](https://github.com/microsoft/winapp-windows-ai/issues).

The Windows AI Addon for Electron is a Node.js native addon that provides access to the [Windows AI APIs](https://learn.microsoft.com/en-us/windows/ai/apis/), enabling Electron applications to leverage Windows AI capabilities directly from JavaScript.

## Prerequisites

- **Copilot+PC**
- **Windows 11** (using Windows Insider Preview build 26120.3073 (Dev and Beta Channels) or later)
- **Node.js** 18.x or later
- **Visual Studio 2022** with C++ build tools
- **Python** 3.x
- **Yarn** package manager
- **Limited Access Feature Token for Phi Silica** - Request a token using the [LAF Access Token Request Form](https://forms.office.com/pages/responsepage.aspx?id=v4j5cvGGr0GRqy180BHbR25txIwisw1PlceTVpYHUm9UODlVMkszVTFaRlVLVlBPNkNaV0hKMzM5Mi4u&route=shorturl). Only needed if you wish to use the `GenerateResponseAsync` API.

See [Windows AI API's Dependencies](https://learn.microsoft.com/windows/ai/apis/get-started?tabs=winget%2Cwinui%2Cwinui2#dependencies) for more information on system requirements to run Windows AI API's and scripts to install required prerequisites.

To verify your device is able to access Windows AI models, you can download the [AI Dev Gallery app](https://apps.microsoft.com/detail/9n9pn1mm3bd5?hl=en-US&gl=US) and verify the "AI APIs" samples run on your device.

## Dependencies

This package depends on the [@microsoft/winappcli](https://github.com/microsoft/WinAppCli) package. Any electron app which uses this package must also take a dependency on `@microsoft/winappcli` and follow its installation and setup steps.

## Get Started Using winapp-windows-ai in an Electron App

Let's walk through a simple tutorial of how to get started using `winapp-windows-ai`. In this tutorial, we will create a simple Electron app that summarizes a block of text using Windows AI's Text Summarizer.

### 1. Create an Electron App

Create an electron app by following the getting started directions at [Electron: Building you First App](https://www.electronjs.org/docs/latest/tutorial/tutorial-first-app).

The instructions below follow the steps for adding `winapp-windows-ai` to a standard Electron app. This module can also be added to Electron apps built using [Electron Forge](https://www.electronforge.io/) and other templates. The setup steps should be similar, but additional configuration may be required to support using NodeJS addons.

### 2. Add winapp-windows-ai as a Dependency

The `winapp-windows-ai` package has not been published to npm yet.

To install, [download the latest prerelease](https://github.com/microsoft/winapp-windows-ai/releases) from GitHub Releases.

```bash
cd <your-electron-app>
yarn add <path to tgz>
```

### 2. Add @microsoft/winappcli as a Dependency

The `@microsoft/winappcli` package has not been published to npm yet. You can install it from GitHub.

Check the `@microsoft/winappcli` version required by `winapp-windows-ai` in the [Release notes](https://github.com/microsoft/winapp-windows-ai/releases). Then download the matching [release .tgz](https://github.com/microsoft/WinAppCli/releases) from the Assets section.

```bash
yarn add <path to tgz>
```

### 4. Install and Setup Dependencies

Initialize project with Windows SDK and Windows App SDK:

```bash
yarn winapp init
```

Edit `winapp.yaml` to use Microsoft.WindowsAppSDK `1.8.251003001` (`winapp.yaml` within app must match `winapp-windows-ai`'s `winapp.yaml`).

Update Windows SDK and Windows App SDK dependencies:

```bash
yarn winapp restore
```

### 5. Add Debug Identity + Capabilities to App

Disable sandboxing in `main.js` (temporary workaround for Windows bug with sparse packaging):

```javacript
app.commandLine.appendSwitch('--no-sandbox');
```

Add `systemAIModels` Capability in `appxmanifest.xml` for app to gain access to local models:

```xml
<Capabilities>
    <rescap:Capability Name="systemAIModels" />
</Capabilities>
```

Add identity to Electron process (required for Windows SDK and Windows App SDK API's):

```bash
yarn winapp node add-electron-debug-identity
```

### 6. Use winapp-windows-ai

Create a `preload.js` file at the root of your project:

```javascript
const { contextBridge } = require("electron");
const { LanguageModel, TextSummarizer } = require("../index.js");

contextBridge.exposeInMainWorld("windowsAI", {
  summarizeText: async (prompt, progressCallback) => {
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
      console.error("Error summarizing text:", error);
    }
  },
});
```

In `main.js`, update `webPreferences`:

```javascript
const win = new BrowserWindow({
    ...
    webPreferences: {
      nodeIntegration: false,
      contextIsolation: true,
      preload: __dirname + '/preload.js', // If using Webpack or other plug-in, this path may change. See plug-in's documentation for more information.
      sandbox: false, // Workaround for Windows bug with sandboxing
    }
  })
```

### 7. Add UI

In `index.html`, we'll add a block of text to summarize, a button to trigger our call to `winapp-windows-ai`, and a block for the generated text:

```html
<!DOCTYPE html>
<html>
  <head>
    <meta charset="UTF-8" />
    <title>Windows AI Example</title>
    <style>
      #summarizeButton {
        width: 200px;
        height: 30px;
        border-radius: 8px;
        border: 1px solid;
        cursor: pointer;
      }
      #textToSummarize,
      #summarizedText {
        margin-top: 10px;
        padding: 15px;
        background-color: #f9f9f9;
        min-height: 50px;
        white-space: pre-wrap;
        border-radius: 8px;
      }
    </style>
    <script>
      document.addEventListener("DOMContentLoaded", function () {
        document.getElementById("summarizeButton").onclick = async function () {
          const textDisplay = document.getElementById("summarizedText");
          const textToSummarize =
            document.getElementById("textToSummarize").textContent;

          setTimeout(async () => {
            try {
              textDisplay.textContent = "";
              const generated = await window.windowsAI.summarizeText(
                textToSummarize,
                (progress) => {
                  textDisplay.textContent += progress;
                }
              );

              if (generated !== textDisplay.textContent) {
                textDisplay.textContent = generated;
              }
            } catch (error) {
              console.error("Error summarizing text:", error);
              textDisplay.textContent =
                "Error summarizing text: " + error.message;
            }
          }, 50);
        };
      });
    </script>
  </head>
  <body>
    <div id="textToSummarize">
      Artificial intelligence (AI) is intelligence demonstrated by machines, in
      contrast to the natural intelligence displayed by humans and animals.
      Leading AI textbooks define the field as the study of "intelligent
      agents": any device that perceives its environment and takes actions that
      maximize its chance of successfully achieving its goals. Colloquially, the
      term "artificial intelligence" is often used to describe machines that
      mimic "cognitive" functions that humans associate with the human mind,
      such as "learning" and "problem solving".
    </div>
    <button id="summarizeButton">Summarize</button>
    <div id="summarizedText">Summarized text will appear here...</div>
  </body>
</html>
```

### 8. Run Your App

```bash
yarn electron .
```

## Supported APIs

This package supports many of the API's within

- [Microsoft.Windows.AI.Text](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text?view=windows-app-sdk-1.8)
- [Microsoft.Windows.AI.Imaging](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging?view=windows-app-sdk-1.8)
- [Microsoft.Windows.AI](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai?view=windows-app-sdk-1.8)

For the full list of supported API's, see [Supported API's](docs/API-Reference.md).

If you have a request for additional API support, please file an [issue](https://github.com/microsoft/winapp-windows-ai/issues/new).

## Usage Examples

For more examples on how to use `winapp-windows-ai` within your Electron application, see the [Usage Guide](docs/Usage.md).

## Development

### Project Structure

```
winapp-windows-ai/
├── winapp-windows-ai/
│   ├── winapp-windows-ai.cc         # Main addon entry point
│   ├── LanguageModelProjections.h   # Language model & text skills APIs
│   ├── LanguageModelProjections.cpp
│   ├── ImagingProjections.h         # Imaging APIs
│   ├── ImagingProjections.cpp
│   ├── ContentSeverity.h            # Content safety APIs
│   ├── ContentSeverity.cpp
│   ├── ProjectionHelper.h           # Utility functions
│   ├── ProjectionHelper.cpp
│   └── binding.gyp                  # Build configuration
├── test-app/                        # Sample Electron application
│   ├── main.js                      # Electron main process
│   ├── preload.js                   # Preload script for winapp-windows-ai integration
│   ├── index.html                   # Sample UI for testing APIs
│   └── package.json                 # Sample app package configuration and dependencies
├── docs/                            # Documentation files
├── scripts/                         # Build scripts
│   ├── build-pkg.ps1                # PowerShell script for package building
│   └── get-build-number.ps1         # Script to retrieve build numbers
├── index.d.ts                       # TypeScript type definitions
├── index.js                         # Main module entry point and exports
└── package.json                     # Package configuration and dependencies
```

### Building the Package Locally

#### 1. Clone the Repository

```bash
git clone https://github.com/microsoft/winapp-windows-ai.git
```

#### 2. Download Windows App Development CLI Package

Check the `@microsoft/winappcli` version required by `winapp-windows-ai` in `package.json`. Then download the matching [release .tgz](https://github.com/microsoft/WinAppCli/releases) from the Assets section.

Move the `.tgz` file to the file path specified in `package.json` or update `package.json` with the correct `.tgz` path.

#### 3. Install Dependencies

```bash
cd <path to winapp-windows-ai repo>
yarn install
yarn winapp restore
```

#### 4. Build the Native Addon

```bash
yarn build-winapp-windows-ai
```

### Building `test-app` Locally

#### 1. Build Package Locally

Complete [Building the Package Locally](#1-build-package-locally) steps above.

#### 2. Setup Dependencies

If `@microsoft/winappcli` package is installed at a different location than specified in `test-app/package.json`, update the `test-app/package.json` entry.

```bash
yarn install
yarn winapp restore
yarn setup-debug
```

#### 3. Run the App

```bash
yarn run start
```

If you make changes to the `winapp-windows-ai` package and want to see your changes loaded into the sample app, make sure to re-build the addon before re-running `test-app`.

## License

This project is licensed under the MIT License - see the [LICENSE](/LICENSE) for details.

## Troubleshooting

### Common Issues

1. **"AI not ready"**: Ensure Windows 11 25H2+ (Windows Insider Preview) and Copilot+ PC requirements are met (see [Prerequisites](#prerequisites))
2. **EnsureReadyAsync cancelled or failed**: Ensure `appxmanifest` has the `systemAIModels` capability added (see [Add Debug Identity + Capabilities to App](#5-add-debug-identity--capabilities-to-app))
3. **"Class Not Registered"**:

   1. Make sure `@microsoft/winappcli` package was setup correctly.
   2. Ensure `winapp.yaml` file in app exactly matches the `winapp.yaml` file in `winapp-windows-ai`.
   3. Ensure `yarn winapp restore` then `yarn winapp node add-electron-debug-identity` have been called.
   4. If the issue is still persisting:
      1. Delete `node_modules` and `.winapp`
      2. Run `yarn cache clean`
      3. Run `yarn install`
      4. Run `yarn winapp restore`
      5. Run `yarn winapp node add-electron-debug-identity`

4. **App renders blank**: Make sure to disable sandboxing when running your Electron app on Windows, then re-run `yarn winapp node add-electron-debug-identity` (see [Add Debug Identity + Capabilities to App](#5-add-debug-identity--capabilities-to-app))
5. **"Can't find module: winapp-windows-ai"**: Make sure sandboxing has been disabled in `webPreferences` (see [Use winapp-windows-ai](#6-use-winapp-windows-ai))
6. **"'functionName' doesn't exist"**: Make sure `preload.js` file has been created. Add `preload.js` to `webPreferences` with the correct absolute path (see [Use winapp-windows-ai](#6-use-winapp-windows-ai)). Path may vary depending on Electron template being used.
7. **Image file not found**: You must use absolute file paths with proper Windows path separators.
8. **Content moderation blocks**: Adjust `ContentFilterOptions` severity levels as appropriate.
9. **Memory issues**: Always call `Close()` or `Dispose()` methods to clean up resources.
10. **GenerateResponseAsync calls are failing**: `GenerateResponseAsync` uses a native API that is currently a [Limited Access Feature](https://learn.microsoft.com/en-us/uwp/api/windows.applicationmodel.limitedaccessfeatures?view=winrt-26100). To request an unlock token, please use the [LAF Access Token Request Form](https://go.microsoft.com/fwlink/?linkid=2271232&c1cid=04x409). To use this method, you must first call `LimitedAccessFeature.TryUnlockToken`. See <TODO> for usage examples.
11. See [Windows AI API's Troubleshooting](https://learn.microsoft.com/windows/ai/apis/troubleshooting) for more information on troubleshooting the native Windows AI API's.
