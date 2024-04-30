const fs = require('fs');

// Leer el contenido del archivo Doxyfile
let content = fs.readFileSync('config/_default/params.toml', 'utf8');

// Leer el contenido del archivo config.json
const config = JSON.parse(fs.readFileSync('../config.json'));

// Reemplazar el valor de PROJECT_NUMBER con la versión
content = content.replace(/version\s*=\s*[^\n]*/, `version = "${config.version}"`);

// Escribir el contenido actualizado en el archivo Doxyfile
fs.writeFileSync('config/_default/params.toml', content);