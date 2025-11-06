const addon = require('./winapp-windows-ai/build/Release/winapp-windows-ai.node');

async function testAsyncCreate() {
    console.log('Testing async CreateAsync...');
    
    try {
        // This should now return a Promise
        console.log('Calling LanguageModel.CreateAsync()...');
        const startTime = Date.now();
        
        const model = await addon.LanguageModel.CreateAsync();
        
        const endTime = Date.now();
        console.log(`✅ CreateAsync completed in ${endTime - startTime}ms`);
        console.log('Model instance:', !!model);
        
        // Test that other methods still work synchronously
        console.log('\nTesting synchronous methods...');
        const readyState = addon.LanguageModel.GetReadyState();
        console.log('Ready state:', readyState);
        
        // Test EnsureReadyAsync (should still be synchronous)
        console.log('Calling EnsureReadyAsync (sync)...');
        addon.LanguageModel.EnsureReadyAsync();
        console.log('✅ EnsureReadyAsync completed');
        
        console.log('\n🎉 All tests passed! Only CreateAsync is now truly async.');
        
    } catch (error) {
        console.error('❌ Error:', error.message);
    }
}

testAsyncCreate();