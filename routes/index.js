var express = require('express')
var router  = express.Router()
const worker = require("streaming-worker");
const path = require("path");
const streamify = require('stream-array');
const through = require('through');

var addon_path = path.join(__dirname, "/../build/Release/cppaddon");
const acc = worker(addon_path);

// Handle POST request to '/start'
router.post('/start', function (req, res) {
  // NOTE: Do something here to start the game

  // Response data
  var data = {
    color: "#DFFF00",
    name: "Trump Snake",
    head_url: "http://www.placecage.com/c/200/200", // optional, but encouraged!
    taunt: "Let's do thisss thang!", // optional, but encouraged!
  }

  return res.json(data)
})

// Handle POST request to '/move'
router.post('/move', function (req, res) {
  // NOTE: Do something here to generate your move
  // var boardx = req["body"]["width"]
  // var boardy = req["body"]["height"]

  // var snakes_alive = req["body"]["snakes"].length - req["body"]["dead_snakes"].length

  // var snakes = req["body"]["snakes"]
  // var you;
  // snakes.forEach(function (snake) {
  //   if (snake["id"] == req["body"]["you"]) {
  //     you = snake
  //   }
  // })

  // var your_length = you["coords"].length
  // var your_health = you["health_points"]

  // console.log("BOARDX: " + boardx)
  // console.log("BOARDY: " + boardy)
  // console.log("snakes_alive: " + snakes_alive)
  // console.log("snakes: " + JSON.stringify(snakes))
  
  // console.log("you: " + JSON.stringify(you))
  // console.log("your_length: " + your_length)
  // console.log("your_health: " + your_health)

  // console.log("BODY: " + JSON.stringify(req["body"]))
  

  var input = acc.to.stream("value",
	function () {
		acc.to.emit('value', -1);
	});

  streamify([1, 2, 3, 4, 5, 6]).pipe(input);

  acc.from.on('TEST', function(value){
      console.log("Accumulated Sum:  " + value);
  });

  // Response data
  var data = {
    move: 'up', // one of: ['up','down','left','right']
    taunt: 'Outta my way, snake!', // optional, but encouraged!
  }

  return res.json(data)
})

module.exports = router
