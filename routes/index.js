var express = require('express')
var router  = express.Router()
const addon = require('../build/Release/addon');
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
  var battle_snake_data = [];

  var boardx = req["body"]["width"]
  var boardy = req["body"]["height"]

  var snakes_alive = req["body"]["snakes"].length - req["body"]["dead_snakes"].length

  var snakes = req["body"]["snakes"]
  var you;
  snakes.forEach(function (snake) {
    if (snake["id"] == req["body"]["you"]) {
      you = snake
    }
  })

  var your_length = you["coords"].length
  var your_health = you["health_points"]

  console.log("BOARDX: " + boardx)
  console.log("BOARDY: " + boardy)
  console.log("snakes_alive: " + snakes_alive)
  console.log("snakes: " + JSON.stringify(snakes))
  
  console.log("you: " + JSON.stringify(you))
  console.log("your_length: " + your_length)
  console.log("your_health: " + your_health)

  console.log("BODY: " + JSON.stringify(req["body"]))

  console.log('This should be 3:', addon.cppaddon(3));
  console.log('This should be 5:', addon.cppaddon(5));

  battle_snake_data.push(boardx);
  battle_snake_data.push(boardy);
  battle_snake_data.push(snakes_alive);
  battle_snake_data.push(your_length);
  battle_snake_data.push(your_health);

  you["coords"].forEach(function (coord) {
    battle_snake_data.push(coord[0])
    battle_snake_data.push(coord[1])
  })

  snakes.forEach(function (snake) {
    if (snake["id"] != req["body"]["you"]) {
      battle_snake_data.push(snake["coords"].length)
      snake["coords"].forEach(function (coord) {
        battle_snake_data.push(coord[0])
        battle_snake_data.push(coord[1])
      })
    }
  })

  req["body"]["food"].forEach(function (food) {
      battle_snake_data.push(food[0])
      battle_snake_data.push(food[1])
  })

  console.log("SNAKE DATA TO SEND TO C++ ADDON : " + battle_snake_data.toString().replace(",", ""))

  // Response data
  var data = {
    move: 'up', // one of: ['up','down','left','right']
    taunt: 'Outta my way, snake!', // optional, but encouraged!
  }

  return res.json(data)
})

module.exports = router
