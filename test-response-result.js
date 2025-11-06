const { LanguageModel, LanguageModelResponseResult, LanguageModelResponseStatus } = require('./winapp-windows-ai/build/Release/winapp-windows-ai.node');

async function testResponseResult() {
    console.log('Testing LanguageModelResponseResult wrapper class...\n');
    
    try {
        // Test the exported constants first
        console.log('0. Testing LanguageModelResponseStatus constants:');
        
        // Expected values based on the enum definition
        const expectedValues = {
            Complete: 0,
            InProgress: 1,
            BlockedByPolicy: 2,
            PromptLargerThanContext: 3,
            PromptBlockedByContentModeration: 4,
            ResponseBlockedByContentModeration: 5,
            Error: 6
        };
        
        // Validate each constant
        for (const [name, expectedValue] of Object.entries(expectedValues)) {
            const actualValue = LanguageModelResponseStatus[name];
            if (actualValue !== expectedValue) {
                throw new Error(`LanguageModelResponseStatus.${name} expected ${expectedValue} but got ${actualValue}`);
            }
            console.log(`✓ ${name}: ${actualValue} (correct)`);
        }
        console.log('All LanguageModelResponseStatus constants are correct!\n');
        
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
        
        // Validate status is a valid enum value
        const validStatuses = Object.values(expectedValues);
        if (!validStatuses.includes(result.status)) {
            throw new Error(`Invalid status value ${result.status}. Expected one of: ${validStatuses.join(', ')}`);
        }
        
        const statusName = Object.keys(expectedValues).find(key => expectedValues[key] === result.status);
        console.log(`✓ Result status: ${statusName} (${result.status}) - valid enum value`);
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
        
        // Validate final result status is a valid enum value
        if (!validStatuses.includes(finalResult.status)) {
            throw new Error(`Invalid final result status value ${finalResult.status}. Expected one of: ${validStatuses.join(', ')}`);
        }
        
        const finalStatusName = Object.keys(expectedValues).find(key => expectedValues[key] === finalResult.status);
        console.log(`✓ Final result status: ${finalStatusName} (${finalResult.status}) - valid enum value`);
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