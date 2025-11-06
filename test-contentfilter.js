// Test script to verify complete ContentFilterOptions functionality
try {
    const windowsAI = require('./winapp-windows-ai/build/Release/winapp-windows-ai.node');
    
    console.log('Testing complete ContentFilterOptions functionality...\n');
    
    // Test SeverityLevel enum
    console.log('=== SeverityLevel Enum ===');
    
    // Expected values based on the enum definition
    const expectedSeverityValues = {
        Minimum: 10,
        Low: 11,
        Medium: 12,
        High: 13
    };
    
    // Validate each constant
    for (const [name, expectedValue] of Object.entries(expectedSeverityValues)) {
        const actualValue = windowsAI.SeverityLevel[name];
        if (actualValue !== expectedValue) {
            throw new Error(`SeverityLevel.${name} expected ${expectedValue} but got ${actualValue}`);
        }
        console.log(`✓ ${name}: ${actualValue} (correct)`);
    }
    console.log('All SeverityLevel constants are correct!\n');
    
    // Test AIFeatureReadyState enum
    console.log('=== AIFeatureReadyState Enum ===');
    
    // Expected values based on the enum definition
    const expectedReadyStateValues = {
        Ready: 0,
        NotReady: 1,
        NotSupportedOnCurrentSystem: 2,
        DisabledByUser: 3
    };
    
    // Validate each constant
    for (const [name, expectedValue] of Object.entries(expectedReadyStateValues)) {
        const actualValue = windowsAI.AIFeatureReadyState[name];
        if (actualValue !== expectedValue) {
            throw new Error(`AIFeatureReadyState.${name} expected ${expectedValue} but got ${actualValue}`);
        }
        console.log(`✓ ${name}: ${actualValue} (correct)`);
    }
    console.log('All AIFeatureReadyState constants are correct!\n');
    
    // Test ImageContentFilterSeverity
    console.log('=== ImageContentFilterSeverity ===');
    const imageSeverity = new windowsAI.ImageContentFilterSeverity();
    console.log('✓ Created ImageContentFilterSeverity instance');
    
    // Test with SeverityLevel parameter
    const imageSeverityWithLevel = new windowsAI.ImageContentFilterSeverity(windowsAI.SeverityLevel.Medium);
    console.log('✓ Created ImageContentFilterSeverity with SeverityLevel parameter');
    
    // Test properties
    console.log('AdultContentLevel:', imageSeverity.AdultContentLevel);
    console.log('GoryContentLevel:', imageSeverity.GoryContentLevel);
    console.log('RacyContentLevel:', imageSeverity.RacyContentLevel);
    console.log('ViolentContentLevel:', imageSeverity.ViolentContentLevel);
    
    // Test setting properties and validate they're set correctly
    imageSeverity.AdultContentLevel = windowsAI.SeverityLevel.High;
    imageSeverity.GoryContentLevel = windowsAI.SeverityLevel.Low;
    console.log('✓ Set ImageContentFilterSeverity properties');
    
    // Validate the values were set correctly
    if (imageSeverity.AdultContentLevel !== windowsAI.SeverityLevel.High) {
        throw new Error(`AdultContentLevel expected ${windowsAI.SeverityLevel.High} but got ${imageSeverity.AdultContentLevel}`);
    }
    if (imageSeverity.GoryContentLevel !== windowsAI.SeverityLevel.Low) {
        throw new Error(`GoryContentLevel expected ${windowsAI.SeverityLevel.Low} but got ${imageSeverity.GoryContentLevel}`);
    }
    
    console.log(`✓ Validated AdultContentLevel: ${imageSeverity.AdultContentLevel} (High)`);
    console.log(`✓ Validated GoryContentLevel: ${imageSeverity.GoryContentLevel} (Low)`);
    console.log('✓ ImageContentFilterSeverity working correctly\n');
    
    // Test TextContentFilterSeverity
    console.log('=== TextContentFilterSeverity ===');
    const textSeverity = new windowsAI.TextContentFilterSeverity();
    console.log('✓ Created TextContentFilterSeverity instance');
    
    // Test with SeverityLevel parameter
    const textSeverityWithLevel = new windowsAI.TextContentFilterSeverity(windowsAI.SeverityLevel.Low);
    console.log('✓ Created TextContentFilterSeverity with SeverityLevel parameter');
    
    // Test properties
    console.log('Hate:', textSeverity.Hate);
    console.log('SelfHarm:', textSeverity.SelfHarm);
    console.log('Sexual:', textSeverity.Sexual);
    console.log('Violent:', textSeverity.Violent);
    
    // Test setting properties and validate they're set correctly
    textSeverity.Hate = windowsAI.SeverityLevel.Medium;
    textSeverity.Violent = windowsAI.SeverityLevel.High;
    console.log('✓ Set TextContentFilterSeverity properties');
    
    // Validate the values were set correctly
    if (textSeverity.Hate !== windowsAI.SeverityLevel.Medium) {
        throw new Error(`Hate expected ${windowsAI.SeverityLevel.Medium} but got ${textSeverity.Hate}`);
    }
    if (textSeverity.Violent !== windowsAI.SeverityLevel.High) {
        throw new Error(`Violent expected ${windowsAI.SeverityLevel.High} but got ${textSeverity.Violent}`);
    }
    
    console.log(`✓ Validated Hate: ${textSeverity.Hate} (Medium)`);
    console.log(`✓ Validated Violent: ${textSeverity.Violent} (High)`);
    console.log('✓ TextContentFilterSeverity working correctly\n');
    
    // Test ContentFilterOptions with subclasses
    console.log('=== ContentFilterOptions Integration ===');
    const contentFilter = new windowsAI.ContentFilterOptions();
    console.log('✓ Created ContentFilterOptions instance');
    
    // Test setting severity objects
    contentFilter.imageMaxAllowedSeverityLevel = imageSeverity;
    contentFilter.promptMaxAllowedSeverityLevel = textSeverity;
    contentFilter.responseMaxAllowedSeverityLevel = textSeverityWithLevel;
    console.log('✓ Set ContentFilterOptions severity properties (including response)');
    
    // Test getting severity objects back
    const retrievedImageSeverity = contentFilter.imageMaxAllowedSeverityLevel;
    const retrievedPromptSeverity = contentFilter.promptMaxAllowedSeverityLevel;
    const retrievedResponseSeverity = contentFilter.responseMaxAllowedSeverityLevel;
    
    console.log('Retrieved imageMaxAllowedSeverityLevel:', retrievedImageSeverity);
    console.log('Retrieved promptMaxAllowedSeverityLevel:', retrievedPromptSeverity);
    console.log('Retrieved responseMaxAllowedSeverityLevel:', retrievedResponseSeverity);
    
    if (retrievedImageSeverity && retrievedPromptSeverity && retrievedResponseSeverity) {
        console.log('✓ Successfully retrieved all severity objects');
        
        // Test accessing properties from retrieved objects
        console.log('Retrieved Image AdultContentLevel:', retrievedImageSeverity.AdultContentLevel);
        console.log('Retrieved Prompt Hate level:', retrievedPromptSeverity.Hate);
        console.log('Retrieved Response Sexual level:', retrievedResponseSeverity.Sexual);
        console.log('✓ Successfully accessed properties from retrieved objects');
    }
    
    // Test with LanguageModelOptions
    console.log('\n=== LanguageModelOptions Integration ===');
    const options = new windowsAI.LanguageModelOptions();
    console.log('✓ Created LanguageModelOptions instance');
    
    // Set ContentFilterOptions
    options.ContentFilterOptions = contentFilter;
    console.log('✓ Set ContentFilterOptions on LanguageModelOptions');
    
    // Get ContentFilterOptions back
    const retrievedContentFilter = options.ContentFilterOptions;
    console.log('Retrieved ContentFilterOptions:', retrievedContentFilter);
    
    if (retrievedContentFilter && typeof retrievedContentFilter === 'object') {
        console.log('✓ Successfully retrieved ContentFilterOptions from LanguageModelOptions');
        
        // Test accessing nested severity objects
        const nestedImageSeverity = retrievedContentFilter.imageMaxAllowedSeverityLevel;
        if (nestedImageSeverity) {
            console.log('Nested Image AdultContentLevel:', nestedImageSeverity.AdultContentLevel);
            console.log('✓ Successfully accessed nested severity objects');
        }
    }
    
    console.log('\n🎉 All tests passed! Complete ContentFilterOptions implementation working correctly!');
    
} catch (error) {
    console.error('❌ Error during testing:', error.message);
    console.error(error.stack);
}