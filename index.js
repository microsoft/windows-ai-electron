const windowsaiAddon = require('./winapp-windows-ai/build/Release/winapp-windows-ai.node');

windowsaiAddon.version = require('./package.json').version;

module.exports = windowsaiAddon;