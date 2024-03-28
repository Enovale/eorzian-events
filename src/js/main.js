var Clay = require('pebble-clay');
var clayConfig = require('./app/config.json');
var clay = new Clay(clayConfig);

Pebble.addEventListener('ready', function(e) {
  // Load local file
  require('./app');
});