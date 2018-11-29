// Timbre

const five = require('johnny-five');
const raspi = require('raspi-io');

// Make a new `Board()` instance and use raspi-io 
const board = new five.Board({
  io: new raspi(),
});

// Run Board 
board.on('ready', function () {

  // Relay variable 
  var relay = new five.Relay({
    pin: 'P1-7'
  });

  this.repl.inject({
    relay: relay
  });

  var duracionRecreo = 1500;

  const temporizador = setInterval(() => {
    console.log('cambio de intervalo');
    relay.toggle();
  }, duracionRecreo);

  function stopRelay() {
    clearInterval(temporizador);
  }

  // Turn off when this script is stopped, 
  this.on('exit', function () {
    relay.off();
  });
}); 
