
//

var static = require('node-static');
var file = new static.Server();
const express = require('express')
const path = require('path')
const PORT = process.env.PORT || 5000
var tools = require('./interface/emscripten/wow_interface');

express()
  .use(express.static(path.join(__dirname, 'interface')))
  .set('view engine', 'html')
  .get('/', (req, res) => res.render('index'))
  .listen(PORT, () => console.log(`Listening on ${ PORT }`))

// require('http').createServer(function(request, response) {
//     request.addListener('end', function() {
//         file.serve(request, response);
//     }).resume();
// }).listen(process.env.PORT || 3000);