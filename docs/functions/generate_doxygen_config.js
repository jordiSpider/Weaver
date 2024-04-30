const fs = require('fs');

// Leer el contenido del archivo Doxyfile
let content = fs.readFileSync('doxygen/Doxyfile', 'utf8');

// Leer el contenido del archivo config.json
const config = JSON.parse(fs.readFileSync('../config.json'));

// Reemplazar el valor de PROJECT_NUMBER con la versión
content = content.replace(/PROJECT_NUMBER\s*=\s*[^\n]*/, `PROJECT_NUMBER = ${config.version}`);

// Escribir el contenido actualizado en el archivo Doxyfile
fs.writeFileSync('doxygen/Doxyfile', content);