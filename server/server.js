const express = require('express')
const http = require('http');

const app = express();
const httpServer = http.createServer(app);
app.use('/', express.static('client'));
app.use('/js/node-editor.js', express.static('build/node-editor.js'));
app.use('/js/node-editor.js.mem', express.static('build/node-editor.js.mem'));

const port = process.env.PORT || 80;
httpServer.listen(port, () => console.log(`Listening on port ${port}!`));