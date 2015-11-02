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
    wait_round(cookies[:color])
    return {json: {message: win_message(cookies[:color])}} if game_terminated?
    return {json: {message: "It's your turn", map: @@map.to_a}}
  end

  def play_round
    raise "Not implemented"
    # Here try to play
    #...
    # Then returns success or error
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
  # ach ach ach
  # TODO: protect against DOS on flood of threads whith a color
  def wait_round color
    loop do
      return true if @@round == color
      sleep 0.1
    end
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
