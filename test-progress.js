const { LanguageModel } = require('./windows-ai-addon/build/Release/windows-ai-addon.node');

async function testProgressStreaming() {
    console.log('Testing Windows AI Progress Streaming with .progress() method...\n');
    
    try {
        // Create the model
        console.log('Creating model...');
        const model = await LanguageModel.CreateAsync();
        console.log('Model created successfully!\n');
        
        console.log('1. Testing basic GenerateResponseAsync (no progress):');
        const basicResponse = await model.GenerateResponseAsync('Tell me a short joke.');
        console.log('Response:', basicResponse);
        console.log('');
        
        console.log('2. Testing GenerateResponseAsync with .progress() method:');
        let progressText = '';
        const result = model.GenerateResponseAsync('Write a short story about a robot learning to paint.');
        
        result.progress((sender, progress) => {
            progressText += progress;
            process.stdout.write(progress);
        });
        
        const response = await result;
        console.log('\n\nFinal response:', response);
        console.log('Progress text collected:', progressText);
        console.log('');
        
        console.log('3. Testing chaining .progress() and .then():');
        let chainedProgressText = '';
        const chainedResult = model.GenerateResponseAsync('Explain quantum computing in simple terms.')
            .progress((sender, progress) => {
                chainedProgressText += progress;
                process.stdout.write(`[STREAMING] ${progress}`);
            })
            .then(response => {
                console.log('\n\nChained response:', response);
                console.log('Chained progress collected:', chainedProgressText);
                return response;
            });
        
        await chainedResult;
        
    } catch (error) {
        console.error('Error during testing:', error);
    }
}

testProgressStreaming();