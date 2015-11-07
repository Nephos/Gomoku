require 'digest'

class GameController < Nephos::Controller

  @@games = {}
  #before_action :set_game
  def set_game
    @game = @@games[cookies[:game]]
    return false if @game.nil?
    @map = @game[:map]
    @code = cookies[:code]
    @color = cookies[:color]
    @round = @game[:round]
    return true
  end

  @@free_game = nil
  # If no free game, start new one
  # add player to free game
  # If free game complete, add it to games and reset
  @@connect_mutex = Mutex.new
  def self.connect! color, code
    @@connect_mutex.lock
    if @@free_game == nil
      @game_id = Player.new_code
      @@free_game = {id: @game_id,
                     map: Map.new,
                     round: "white",
                     # player codes
                     players: {},
                     # lock the game while there is no 2 players connected
                     players_mutex: {"white" => Mutex.new, "black" => Mutex.new},
      }
    end
    @@free_game[:players][color] = code # add the player
    if @@free_game[:players].size == 2
      @game_id = @@free_game[:id]
      @@games[@game_id] = @@free_game
      @@free_game = nil
    end
    @@connect_mutex.unlock
    return @game_id
  end

  def request_round
    return auth_err unless auth?
    return wait_err unless wait_round
    return {json: {message: "Game end. You failed."}} if game_terminated?
    return {json: {message: "It's your turn", map: @map.to_a}}
  end

  def play_round
    return auth_err unless auth?
    return {json: {message: "Not your turn. It's #{@round}."}, status: 401} if @round != @color
    x, y = Integer(params[:x]), Integer(params[:y])
    return {json: {message: "Invalid position (x)", map: @game[:map].to_a, status: 401}} if x < 0 or x > 18
    return {json: {message: "Invalid position (y)", map: @game[:map].to_a, status: 401}} if y < 0 or y > 18
    return {json: {message: "Invalid position (occupied)", map: @game[:map].to_a, status: 401}} if @game[:map][x][y]
    color = @color == "white" ? 0 : 1
    @game[:map][x][y] = color
    @game[:map].take_around!(x, y, color)
    if @game.win?(x, y, color)
      game_terminated!
      return {json: {message: "You win.", map: @game[:map].to_a}}
    end
    next_round!
    return {json: {message: "Well played. Next turn...", map: @game[:map].to_a}}
  end

  private
  def auth?
    return false if set_game == false
    p = @game[:players][cookies[:color]]
    # check if the client code is the player code
    return p == cookies[:code]
  end
  def auth_err
    g = @@games[cookies[:map]]
    unless g && g[:players][cookies[:color]] && @@free_game.nil?
      return {json: {message: "Forbidden. Game not validated.", status: 401}}
    end
    return {json: {message: "Forbidden. Not connected.", status: 403}}
  end

  def next_round!
    @game[:round] = (@game[:round] == "white") ? "black" : "white"
  end
  def wait_round
    # Already waiting for #{color}
    return false unless @game[:players_mutex][@color].try_lock
    loop do
      # wait untile the color is right
      if @game[:round] == @color
        @game[:players_mutex][@color].unlock
        return true
      end
      sleep 0.1
    end
  end
  def wait_err
    return {json: {status: 401, message: "Error, you cannot join the round"}}
  end

  def game_terminated!
    @game[:players] = {}
  end
  def game_terminated?
    @game[:players].empty?
  end

end
