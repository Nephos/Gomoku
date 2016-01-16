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
    @win = @game[:win]
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
                     players_took: {"white" => 0, "black" => 0},
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

  def request_map
    return auth_err unless auth?
    get_map_render
    return {plain: "ok.\n" + @map_render} if plain?
    return {json: {message: "Ok", map: @map_render, points: @map.took_hash}}
  end

  # The method waits for the turn of the player before returning
  #
  # returns the map every times
  # also add in first place "failed/continue. ...\n" for plain
  def request_round
    return auth_err unless auth?
    log "#{@color} requested a round"
    return wait_err unless wait_round
    get_map_render
    return game_terminated_msg if game_terminated?
    return {plain: "continue.\n" + @map_render} if plain?
    return {json: {message: "It's your turn", map: @map_render, points: @map.took_hash}}
  end
  private
  def game_terminated_msg
    if @win == @color
      return {plain: "win.\n" + @map_render} if plain?
      return {json: {message: "failed", map: @map_render, points: @map.took_hash}}
    else
      return {plain: "failed.\n" + @map_render} if plain?
      return {json: {message: "failed", map: @map_render, points: @map.took_hash}}
    end
  end
  public

  # The route /game/play/x/y will try to put a item with color at y:x
  # It will check if the move is valid, and then apply the rules
  #
  # returns the map every times
  # also add in first place "failed/continue/win. ...\n" for plain
  def play_round
    return auth_err unless auth?
    get_map_render
    if @round != @color
      return {plain: "failed. not your turn\n" + @map_render, status: 401} if plain?
      return {json: {message: "Not your turn. It's #{@round}.", points: @map.took_hash}, status: 401}
    end
    x, y = Integer(params[:x]), Integer(params[:y])
    color = @color == "white" ? 0 : 1
    case @map.valid_place_for? x, y, color
    when nil
      # ok
    when 1
      return {plain: "failed. invalid x\n" + @map_render, status: 401} if plain?
      return {json: {message: "Invalid position (x)", map: @map_render, points: @map.took_hash}, status: 401}
    when 2
      return {plain: "failed. invalid y\n" + @map_render, status: 401} if plain?
      return {json: {message: "Invalid position (y)", map: @map_render, points: @map.took_hash}, status: 401}
    when 3
      return {plain: "failed. occupied\n" + @map_render, status: 401} if plain?
      return {json: {message: "Invalid position (occupied)", map: @map_render, points: @map.took_hash}, status: 401}
    when 4
      # error for 3x3
    when 5
      # error for 5 lock
    else
      # WHAT ?!
    end
    @game[:map][y][x] = color
    @game[:map].take_around!(y, x, color)
    win = @game[:map].win? color
    get_map_render
    if win
      game_terminated!
      return {plain: "win.\n" + @map_render} if plain?
      return {json: {message: "You win.", map: @map_render, points: @map.took_hash}}
    end
    next_round!
    return {plain: "continue.\n" + @map_render} if plain?
    return {json: {message: "Well played. Next turn...", map: @map_render, points: @map.took_hash}}
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
      return {plain: "failed. not ready\n", status: 401} if plain?
      return {json: {message: "Forbidden. Game not validated."}, status: 401}
    end
    return {plain: "failed. not connected\n", status: 403} if plain?
    return {json: {message: "Forbidden. Not connected."}, status: 403}
  end

  def get_map_render
    if plain?
      @map_render = @map.to_a.map{|e| e.map{|x| x||"x"}.join(" ")}.join("\n")
      @map_render += "\n" + @map.took.join(" ")
    else
      @map_render = @map.to_a
    end
  end

  def next_round!
    @game[:round] = (@game[:round] == "white") ? "black" : "white"
  end
  def wait_round
    # Already waiting for #{color}
    return true if @game[:round].nil?
    return false unless @game[:players_mutex][@color].try_lock
    loop do
      # wait untile the color is right
      if @game[:round] == @color or game_terminated?
        @game[:players_mutex][@color].unlock
        return true
      end
      sleep 0.1
    end
  end
  def wait_err
    return {plain: "failed. connect join", status: 401} if plain?
    return {json: {message: "Error, you cannot join the round"}, status: 401}
  end

  def game_terminated!
    @game[:round] = nil
    @game[:win] = @color
  end
  def game_terminated?
    @game[:round].nil?
  end

end
