require_relative 'models/player'

class PlayerController < Nephos::Controller

  @@color = 0
  def connect_err
    return {json: {message: "failure"}, status: 401}
  end

  @@connect_mutex = Mutex.new
  def connect
    @@connect_mutex.lock # do not allows multiple connections to dispatch the games
    color = (@@color % 2 == 0 ? "white" : "black")
    @@color += 1
    cookies[:color] = color
    cookies[:code] = Player.new_code
    id = GameController.connect! cookies[:color], cookies[:code]
    @@connect_mutex.unlock # allows new connections
    cookies[:game] = id
    return {json: {message: "connected to #{id} as #{color}", id: id, color: color}, status: 200}
  end

end
