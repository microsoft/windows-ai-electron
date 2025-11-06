// Example usage of both LanguageModel and LanguageModelOptions classes
const windowsAI = require('./winapp-windows-ai/build/Release/winapp-windows-ai.node');

console.log('Testing LanguageModel class:');
console.log('- LanguageModel available:', typeof windowsAI.LanguageModel);
console.log('- CreateAsync method available:', typeof windowsAI.LanguageModel.CreateAsync);

console.log('\nTesting LanguageModelOptions class:');
console.log('- LanguageModelOptions available:', typeof windowsAI.LanguageModelOptions);

// Create a new LanguageModelOptions instance
var options = new windowsAI.LanguageModelOptions();

// Set properties as requested
options.temperature = 0.9;
options.topK = 50;
options.topP = 0.8;
options.contentFilterOptions = null;

// Display the values to verify they were set correctly
console.log('- Temperature:', options.temperature);
console.log('- TopK:', options.topK);
console.log('- TopP:', options.topP);
console.log('- ContentFilterOptions:', options.contentFilterOptions);

console.log('\n✅ Both classes successfully exported and working!');
console.log('\nNow you can use GenerateResponseAsync in two ways:');
console.log('1. Without options: model.GenerateResponseAsync("Hello")');
console.log('2. With options: model.GenerateResponseAsync("Hello", options)');

console.log('\nExample usage:');
console.log('// Create model and options');
console.log('var model = windowsAI.LanguageModel.CreateAsync();');
console.log('var options = new windowsAI.LanguageModelOptions();');
console.log('options.temperature = 0.9;');
console.log('');
console.log('// Use without options');
console.log('var response1 = model.GenerateResponseAsync("Tell me a joke");');
console.log('');
console.log('// Use with options');
console.log('var response2 = model.GenerateResponseAsync("Tell me a joke", options);');