const { LanguageModel, LanguageModelResponseResult, LanguageModelResponseStatus } = require('./windows-ai-addon/build/Release/windows-ai-addon.node');

async function testResponseResult() {
    console.log('Testing LanguageModelResponseResult wrapper class...\n');
    
    try {
        // Test the exported constants first
        console.log('0. Testing LanguageModelResponseStatus constants:');
        console.log('Complete:', LanguageModelResponseStatus.Complete);
        console.log('InProgress:', LanguageModelResponseStatus.InProgress);
        console.log('BlockedByPolicy:', LanguageModelResponseStatus.BlockedByPolicy);
        console.log('PromptLargerThanContext:', LanguageModelResponseStatus.PromptLargerThanContext);
        console.log('PromptBlockedByContentModeration:', LanguageModelResponseStatus.PromptBlockedByContentModeration);
        console.log('ResponseBlockedByContentModeration:', LanguageModelResponseStatus.ResponseBlockedByContentModeration);
        console.log('Error:', LanguageModelResponseStatus.Error);
        console.log('');
        
        // Create the model
        console.log('Creating model...');
        const model = await LanguageModel.CreateAsync();
        console.log('Model created successfully!\n');
        
        // Test with basic GenerateResponseAsync
        console.log('1. Testing basic GenerateResponseAsync:');
        const result = await model.GenerateResponseAsync('Tell me a short joke.');
        console.log('Result type:', typeof result);
        console.log('Result constructor:', result.constructor.name);
        console.log('Result text:', result.text);
        console.log('Result status (numeric):', result.status);
        console.log('Result status meaning:', result.status === LanguageModelResponseStatus.Complete ? 'Complete' : 'Other');
        console.log('Result extendedError:', result.extendedError);
        console.log('');
        
        // Test with progress streaming
        console.log('2. Testing GenerateResponseAsync with progress:');
        let progressText = '';
        const progressResult = model.GenerateResponseAsync('Write a haiku about coding.');
        
        progressResult.progress((sender, progress) => {
            progressText += progress;
            process.stdout.write(progress);
        });
        
        const finalResult = await progressResult;
        console.log('\n\nFinal result type:', typeof finalResult);
        console.log('Final result text:', finalResult.text);
        console.log('Final result status (numeric):', finalResult.status);
        console.log('Final result status meaning:', finalResult.status === LanguageModelResponseStatus.Complete ? 'Complete' : 'Other');
        console.log('Final result extendedError:', finalResult.extendedError);
        console.log('Progress text collected:', progressText);
        console.log('');
        
        // Test hasResult method if available
        if (typeof finalResult.HasResult === 'function') {
            console.log('3. Testing HasResult method:', finalResult.HasResult());
        }
        
        console.log('All tests completed successfully!');
        
    } catch (error) {
        console.error('Error during testing:', error);
    }
}

testResponseResult();