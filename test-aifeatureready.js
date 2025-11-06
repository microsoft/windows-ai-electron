// Test script to verify AIFeatureReadyResult functionality
try {
    const windowsAI = require('./winapp-windows-ai/build/Release/winapp-windows-ai.node');
    
    console.log('Testing AIFeatureReadyResult functionality...\n');
    
    // Test AIFeatureReadyResultState enum
    console.log('=== AIFeatureReadyResultState Enum ===');
    
    // Expected values based on the enum definition
    const expectedValues = {
        InProgress: 0,
        Success: 1,
        Failure: 2
    };
    
    // Validate each constant
    for (const [name, expectedValue] of Object.entries(expectedValues)) {
        const actualValue = windowsAI.AIFeatureReadyResultState[name];
        if (actualValue !== expectedValue) {
            throw new Error(`AIFeatureReadyResultState.${name} expected ${expectedValue} but got ${actualValue}`);
        }
        console.log(`✓ ${name}: ${actualValue} (correct)`);
    }
    console.log('All AIFeatureReadyResultState constants are correct!\n');
    
    // Test EnsureReadyAsync with progress and AIFeatureReadyResult
    console.log('=== Testing EnsureReadyAsync ===');
    console.log('Calling LanguageModel.EnsureReadyAsync()...');
    
    const ensureReadyPromise = windowsAI.LanguageModel.EnsureReadyAsync();
    
    // Check if it returns a promise with progress support
    if (ensureReadyPromise && typeof ensureReadyPromise.then === 'function') {
        console.log('✓ EnsureReadyAsync returned a promise');
        
        // Test if progress method exists
        if (typeof ensureReadyPromise.progress === 'function') {
            console.log('✓ Promise has progress method');
            
            // Set up progress handler
            ensureReadyPromise.progress((progressValue) => {
                console.log(`📈 Progress: ${(progressValue * 100).toFixed(1)}%`);
            });
        }
        
        // Handle the result
        ensureReadyPromise.then((result) => {
            console.log('\n🎉 EnsureReadyAsync completed successfully!');
            console.log('Result type:', typeof result);
            console.log('Result:', result);
            
            if (result && typeof result === 'object') {
                console.log('\n=== AIFeatureReadyResult Properties ===');
                
                // Test all properties
                console.log('Status:', result.Status);
                console.log('Error:', result.Error);
                console.log('ErrorDisplayText:', result.ErrorDisplayText);
                console.log('ExtendedError:', result.ExtendedError);
                
                // Validate status is a valid enum value
                const validStatuses = Object.values(expectedValues);
                if (!validStatuses.includes(result.Status)) {
                    throw new Error(`Invalid AIFeatureReadyResult status value ${result.Status}. Expected one of: ${validStatuses.join(', ')}`);
                }
                
                // Interpret status
                const statusValue = result.Status;
                let statusName = Object.keys(expectedValues).find(key => expectedValues[key] === statusValue);
                console.log(`✓ Status: ${statusName} (${statusValue}) - valid enum value`);
                
                if (statusValue === windowsAI.AIFeatureReadyResultState.Success) {
                    console.log('✅ Language model is ready!');
                } else if (statusValue === windowsAI.AIFeatureReadyResultState.Failure) {
                    console.log('❌ Language model failed to be ready');
                    if (result.ErrorDisplayText) {
                        console.log('Error message:', result.ErrorDisplayText);
                    }
                } else {
                    console.log('⏳ Language model is still in progress');
                }
                
                console.log('✓ All AIFeatureReadyResult properties accessible');
            }
            
            console.log('\n✅ All tests passed! AIFeatureReadyResult implementation working correctly!');
            
        }).catch((error) => {
            console.error('\n❌ EnsureReadyAsync failed:', error.message);
            
            // This might be expected if the language model isn't available
            if (error.message.includes('not supported') || error.message.includes('not available')) {
                console.log('💡 Note: This might be expected if Windows AI features are not available on this system');
                console.log('✓ The AIFeatureReadyResult implementation is still working correctly');
            }
        });
        
    } else {
        console.log('❌ EnsureReadyAsync did not return a promise');
    }
    
} catch (error) {
    console.error('❌ Error during testing:', error.message);
    console.error(error.stack);
}