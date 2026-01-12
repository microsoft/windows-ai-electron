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
- **Limited Access Feature Token for Phi Silica** - Request a token using the [LAF Access Token Request Form](https://forms.office.com/pages/responsepage.aspx?id=v4j5cvGGr0GRqy180BHbR25txIwisw1PlceTVpYHUm9UODlVMkszVTFaRlVLVlBPNkNaV0hKMzM5Mi4u&route=shorturl). Only needed if you wish to use the `GenerateResponseAsync` API.

See [Windows AI API's Dependencies](https://learn.microsoft.com/windows/ai/apis/get-started?tabs=winget%2Cwinui%2Cwinui2#dependencies) for more information on system requirements to run Windows AI API's and scripts to install required prerequisites.

> [!IMPORTANT]
> Verify your device is able to access Windows AI models by downloading the [AI Dev Gallery app](https://apps.microsoft.com/detail/9n9pn1mm3bd5?hl=en-US&gl=US). Navigate to the "AI APIs" samples and ensure they can run on your device. If the samples are blocked, the AI models may be missing from your machine. You can manually request a download by selecting the "Request Model" button and following the directions within Windows Update settings.

## Dependencies

This package depends on the [@microsoft/winappcli](https://github.com/microsoft/WinAppCli) package. Any electron app which uses this package must also take a dependency on `@microsoft/winappcli` and follow its installation and setup steps.

## Get Started Using @microsoft/winapp-windows-ai in an Electron App

Let's walk through a simple tutorial of how to get started using `@microsoft/winapp-windows-ai`. In this tutorial, we will create a simple Electron app that summarizes a block of text using Windows AI's Text Summarizer.

### 1. Create an Electron App

Create an electron app by following the getting started directions at [Electron: Building you First App](https://www.electronjs.org/docs/latest/tutorial/tutorial-first-app).

The instructions below follow the steps for adding `@microsoft/winapp-windows-ai` to a standard Electron app. This module can also be added to Electron apps built using [Electron Forge](https://www.electronforge.io/) and other templates.

### 2. Add @microsoft/winapp-windows-ai as a Dependency

```bash
cd <your-electron-app>
npm i @microsoft/winapp-windows-ai
```

### 3. Add @microsoft/winappcli as a dev Dependency

```bash
npm i @microsoft/winappcli -D
```

### 4. Install and Setup Dependencies

Initialize your project for calling Windows APIs. This will create an appxmanifest.xml, required assets, and make the Windows App SDK available for usage:

```bash
npx winapp init
```

> [!IMPORTANT]
> When you run `npx winapp init`, it generates a `winapp.yaml` file for managing SDK versions. Make sure the version of the Microsoft.WindowsAppSDK package is `1.8.xxxxx`. If it's not, simply set the version to `1.8.251106002` and run `npx winapp restore` to ensure the proper dependencies are available for the project.

### 5. Add `systemAIModels` Capability

Add `systemAIModels` Capability in `appxmanifest.xml` for app to gain access to local models:

```xml
<Capabilities>
    <rescap:Capability Name="systemAIModels" />
</Capabilities>
```
### 6. Add Debug Identity
 
Before your app can call the AI APIs, we need to make sure the electron process has identity as defined in you appxmanifest.xml. Add app identity to the Electron process for debugging:

```bash
npx winapp node add-electron-debug-identity
```

> [!IMPORTANT]
> There is a known issue with sparse packaging Electron applications which causes the app to crash on start or not render the web content. The issue has been fixed in Windows but has not yet fully propagated to all Windows devices. If you are seeing this issue after calling `add-electron-debug-identity`, you can [disable sandboxing in your Electron app](https://www.electronjs.org/docs/latest/tutorial/sandbox#disabling-chromiums-sandbox-testing-only) for debug purposes with the `--no-sandbox` flag. This issue does not affect full MSIX packaging.

### 7. Use @microsoft/winapp-windows-ai

In `main.js` or `index.js`:

```javascript
const {
  LanguageModel,
  AIFeatureReadyResultState,
  TextSummarizer,
  LanguageModelResponseStatus,
} = require("@microsoft/winapp-windows-ai");

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
```

Call `summarizeText` somewhere in your `main.js` or `index.js`.

Here's an example:
```diff
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

+  summarizeText();
}
```

### 8. Run Your App

```bash
npx electron .
```

You should see the local model streaming the response to the console.

## Supported APIs

This package supports many of the API's within

- [Microsoft.Windows.AI.Text](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text?view=windows-app-sdk-1.8)
- [Microsoft.Windows.AI.Imaging](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging?view=windows-app-sdk-1.8)
- [Microsoft.Windows.AI](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai?view=windows-app-sdk-1.8)

For the full list of supported API's, see [Supported API's](docs/API-Reference.md).

If you have a request for additional API support, please file an [issue](https://github.com/microsoft/winapp-windows-ai/issues/new).

## Usage Examples

For more examples on how to use `@microsoft/winapp-windows-ai` within your Electron application, see the [Usage Guide](docs/Usage.md).

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
│   ├── LimitedAccessFeature.h       # Limited Access Feature token APIs
│   ├── LimitedAccessFeature.cpp
│   ├── ProjectionHelper.h           # Utility functions
│   ├── ProjectionHelper.cpp
│   └── binding.gyp                  # Build configuration
├── test-app/                        # Sample Electron application
│   ├── main.js                      # Electron main process
│   ├── preload.js                   # Preload script for @microsoft/winapp-windows-ai integration
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

#### 2. Install Dependencies

```bash
cd <path to @microsoft/winapp-windows-ai repo>
npm install
npx winapp restore
```

#### 3. Build the Native Addon

```bash
npm run build-all
```

### Building `test-app` Locally

#### 1. Build Package Locally

Complete [Building the Package Locally](#1-build-package-locally) steps above.

#### 2. Setup Dependencies

Install dependencies:
```bash
npm install
```

Initialize Windows App Development CLI (select N to preserve dependency versions):
```bash
npx winapp init
npm run setup-debug
```

#### 3. Run the App

```bash
npm run start
```

If you make changes to the `@microsoft/winapp-windows-ai` package and want to see your changes loaded into the sample app, make sure to re-build the addon before re-running `test-app`.

## License

This project is licensed under the MIT License - see the [LICENSE](/LICENSE) for details.

## Troubleshooting

### Common Issues

1. **"AI not ready" or EnsureReadyAsync/GetReadyState return status AIFeatureReadyResultState::Failure(2)**: Ensure Windows 11 25H2+ (Windows Insider Preview) and Copilot+ PC requirements are met. Validate your device has Windows AI models installed and available by downloading the [AI Dev Gallery app](https://apps.microsoft.com/detail/9n9pn1mm3bd5?hl=en-US&gl=US). Then navigate to the "AI APIs" samples and ensure they can run on your device. (see [Prerequisites](#prerequisites))
2. **EnsureReadyAsync cancelled or failed**: Ensure `appxmanifest` has the `systemAIModels` capability added (see [Add Debug Identity + Capabilities to App](#5-add-debug-identity--capabilities-to-app))
3. **"Class Not Registered"**: Make sure you have correctly setup the `@microsoft/winappcli` package (see [Add @microsoft/winappcli as a Dependency](#3-add-microsoftwinappcli-as-a-dependency)).
   - If the issue is still persisting:
      1. Delete `node_modules`, `yarn.lock`, and `.winapp`
      2. Run `npm cache clean --force`
      3. Run `npm install`
      4. Run `npx winapp restore`
      5. Run `npx winapp node add-electron-debug-identity`

4. **App renders blank**: You may need to disable sandboxing when running your Electron app on Windows. Then re-run `npx winapp node add-electron-debug-identity` (see [Add Debug Identity + Capabilities to App](#5-add-debug-identity--capabilities-to-app))
5. **"Can't find module: @microsoft/winapp-windows-ai"**: Verify you have added the `@microsoft/winapp-windows-ai` package as a dependency to your application.
7. **Image file not found**: You must use absolute file paths with proper Windows path separators.
8. **Content moderation blocks**: Adjust `ContentFilterOptions` severity levels as appropriate.
9. **Memory issues**: Always call `Close()` or `Dispose()` methods to clean up resources.
10. **GenerateResponseAsync calls are failing**: `GenerateResponseAsync` uses a native API that is currently a [Limited Access Feature](https://learn.microsoft.com/en-us/uwp/api/windows.applicationmodel.limitedaccessfeatures?view=winrt-26100). To request an unlock token, please use the [LAF Access Token Request Form](https://go.microsoft.com/fwlink/?linkid=2271232&c1cid=04x409). To use this method, you must first call `LimitedAccessFeature.TryUnlockToken`. See [Usage Guide](/docs/Usage.md/#text-generation) for usage examples.
11. See [Windows AI API's Troubleshooting](https://learn.microsoft.com/windows/ai/apis/troubleshooting) for more information on troubleshooting the native Windows AI API's.
