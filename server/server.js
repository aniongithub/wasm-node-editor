const express = require('express')
const http = require('http');

const app = express();
const httpServer = http.createServer(app);
app.use('/', express.static('client'));
app.use('/js/node-editor-wasm.js', express.static('build/wasm/node-editor-wasm.js'));
app.use('/js/node-editor-wasm.js.mem', express.static('build/wasm/node-editor-wasm.js.mem'));

const port = process.env.PORT || 80;
httpServer.listen(port, () => console.log(`Listening on port ${port}!`));