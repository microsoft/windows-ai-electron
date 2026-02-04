const os = require('os'); 
const arch = os.arch(); 
if (arch === 'arm64') { 
    module.exports = require('./windows-ai-electron/prebuilds/win32-arm64/node.node'); 
} else { 
    module.exports = require('./windows-ai-electron/prebuilds/win32-x64/node.node'); 
}