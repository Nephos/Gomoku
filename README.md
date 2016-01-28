# Install and usage

Works only with Xlib.

*Windows fuck you.*

## Server

- Install [Ruby](https://nephos.xyz/ruby.html).
- Then ``cd Server; gem install bundler; bundle install``.
- Then you can start the server via ``nserver``.

## Client

- Be sure you have c++ (algo), xlib (graphical client) and boost (network) installed.
- Build ``cd Client; make -j``.
- Start the client via:
  - ``./client localhost:8080``
  - ``./client localhost:8080 --wheeeeee``
  - ``./client localhost:8080 --norainbows``
  - ``./client localhost:8080 --ia=true``
- Start the client a second time with ``--ia=true`` if you want to play against a robot (7 years kid like)
- Start the client a second time without if you want to play with an other player. You can change the ip and play from 2 computers :)

# Behavior

- The server simply waits for 2 following connections and start a game.
- If you started the client 3 times, only the 2 first will work, the last will just wait for a 4th connection.
- Just enjoy x)
- The last commits are "yolo".
