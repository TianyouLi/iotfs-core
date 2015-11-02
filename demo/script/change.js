var fs = require('fs');
var result = process.argv.slice(2);
fs.writeFileSync(result[0], result[1] + '\n'); 