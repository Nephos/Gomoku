class GameController < Nephos::Controller

  @@map = nil
  @@round = nil
  # each game played add :black or :white in @@win
  @@win = []
  @@last = nil

  def self.start_new_game!
    @@map = Map.new
    @@round = :white
  end

  def request_round
    return auth_err unless auth?
    return wait_err unless wait_round(cookies[:color])
    return {json: {message: win_message(cookies[:color])}} if game_terminated?
    return {json: {message: "It's your turn", map: @@map.to_a}}
  end

  @@round_mutex = Mutex.new
  def play_round
    @@round_mutex.lock
    raise "Not implemented"
    # Here try to play
    #...
    # Then returns success or error
    @@round_mutex.unlock
    #return {json: {message: win_message(cookies[:color])}} if game_terminated?
    #return {json: {message: "Not authorized", code: 401, map: @@map.to_a}}
    #next_round!
    #return {json: {message: "Well played. Next turn...", map: @@map.to_a}}
  end

  def status
    return {json: {win: @@win, current: !game_terminated?}}
  end

  private
  def auth?
    p = PlayerController.all[cookies[:color]]
    return false unless p
    return p.code == cookies[:code]
  end
  def auth_err
    return {json: "Forbidden. Not connected", status: 403}
  end

  def next_round!
    @@round = (@@round == :white) ? :black : :white
  end
  @@player_mutex = {white: Mutex.new, black: Mutex.new}
  def wait_round color
    return false unless @@player_mutex[color].try_lock
    loop do
      if @@round == color
        @@player_mutex[color].unlock
        return true
      end
      sleep 0.1
    end
  end
  def wait_err
    return {json: {status: 401, message: "Error, you cannot join the round"}}
  end

  def win! color
    @@win << color
    @@last += 1
    game_terminated!
  end
  def win
    return nil if @@round
    return @@win.last
  end
  def win_message color
    return nil unless win?
    return "You failed" if color != win
    return "You win"
  end

  def game_terminated!
    PlayerController.disconnect!
  end
  def game_terminated?
    return @@last == @win.size
  end

end
