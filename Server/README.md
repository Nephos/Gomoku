# Start
``bundle install && nserver``

# How to use

## Connection
``/players/connect``
Connect 2 differents clients to the server.
Cookies will be set for color (color), map id (map) and secret token (code)

## Get your turn
``/game.txt or /game``
Get the map state when you can play. Wait until your round.
It will also check if you loose or win.

## Play a move
``/game/play/x/y
Try to put a item at y:x based on your color.
Return 401 if failed, and wait for a new move.
Return 200 if succeed, and also say continue or win if you finished the game

