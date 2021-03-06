require 'pry'
require 'colorize'
require_relative '../app/models/map'
puts "Please be sure you started the server on 8080".yellow

module HTTP
  class Response
    def cookies
      self.headers["Set-Cookie"]
    end
  end
end

class TestGameReal < Test::Unit::TestCase

  BASE = "http://localhost:8080/"
  CONN = BASE + "players/connect"
  WAIT = BASE + "game"
  MAP = BASE + "game/map"
  PLAY = BASE + "game/play/X/Y"

  def new_game
    @c1 = HTTP::Client.new
    @c2 = HTTP::Client.new
    @r1 = @c1.get CONN
    @r2 = @c2.get CONN
    @co1 = @r1.cookies.map{|e| e.split(';').first}
    @co2 = @r2.cookies.map{|e| e.split(';').first}
    @co1 = {"Cookie" => @co1}
    @co2 = {"Cookie" => @co2}
  end

  def test_game_simple
    new_game
    play(1, 0, 0, 200, 200)
    play(2, 0, 1, 200, 200)
    play(1, 1, 0, 200, 200)
    play(2, 0, 2, 200, 200)
    play(1, 2, 0, 200, 200)
    play(2, 0, 3, 200, 200)
    play(1, 3, 0, 200, 200)
    play(2, 0, 4, 200, 200)
    play(1, 4, 0, 200, 200, {end_msg: "You win."})
  end

  def test_game_simple_map
    new_game
    map = Map.new
    assert_equal map.to_a, get_map
    play(1, 0, 0, 200, 200)
    map[0][0] = 0
    assert_equal map.to_a, get_map

    play(2, 0, 1, 200, 200)
    map[0][1] = 1
    assert_equal map.to_a, get_map

    play(1, 1, 0, 200, 200)
    map[1][0] = 0
    assert_equal map.to_a, get_map

    play(2, 0, 2, 200, 200)
    map[0][2] = 1
    assert_equal map.to_a, get_map

    play(1, 2, 0, 200, 200)
    map[2][0] = 0
    assert_equal map.to_a, get_map

    play(2, 0, 3, 200, 200)
    map[0][3] = 1
    assert_equal map.to_a, get_map

    play(1, 3, 0, 200, 200)
    map[3][0] = 0
    assert_equal map.to_a, get_map

    play(2, 0, 4, 200, 200)
    map[0][4] = 1
    assert_equal map.to_a, get_map

    body = play(1, 4, 0, 200, 200).body.to_s
    map[4][0] = 0
    assert_equal map.to_a, get_map
    assert_equal "You win.", JSON.parse(body)["message"]
  end

  def test_simple_take
    new_game
    map = Map.new
    play(1, 0, 0, 200, 200); map[0][0] = 0
    play(2, 1, 0, 200, 200); map[1][0] = 1
    play(1, 9, 0, 200, 200); map[9][0] = 0
    play(2, 2, 0, 200, 200); map[2][0] = 1
    play(1, 3, 0, 200, 200); map[3][0] = 0
    map[1][0] = nil
    map[2][0] = nil
    assert_equal map.to_a, get_map
  end

  def test_double_take
    new_game
    map = Map.new
    play_map(map, 1, 1, 0) # 1
    play_map(map, 2, 1, 1)
    play_map(map, 1, 0, 1) # 2
    play_map(map, 2, 1, 2)
    play_map(map, 1, 0, 0) # 3
    play_map(map, 2, 1, 3)
    play_map(map, 1, 1, 4) # 4
    play_map(map, 2, 2, 1)
    _ = play_map(map, 1, 3, 2).body.to_s # 5
    assert_equal 0, get_map.count(1)
    # continue
  end

  def test_5_breakable_disabled
    return if ENV["DISABLE_BREAK5"] != "true"
    new_game
    map = Map.new
    assert_equal map.to_a, get_map
    play_map(map, :white, 1, 0, 200, 200)
    play_map(map, :black, 9, 0, 200, 200)
    play_map(map, :white, 1, 1, 200, 200)
    play_map(map, :black, 9, 1, 200, 200)
    play_map(map, :white, 1, 2, 200, 200)
    play_map(map, :black, 9, 2, 200, 200)
    play_map(map, :white, 2, 2, 200, 200)
    play_map(map, :black, 0, 2, 200, 200)
    play_map(map, :white, 1, 3, 200, 200)
    play_map(map, :black, 9, 3, 200, 200)
    body = play_map(map, :white, 1, 4, 200, 200).body.to_s
    assert_equal "You win.", JSON.parse(body)["message"]
  end

  def test_5_breakable
    return if ENV["DISABLE_BREAK5"] == "true"
    new_game
    map = Map.new
    assert_equal map.to_a, get_map
    play_map(map, :white, 1, 0, 200, 200)
    play_map(map, :black, 9, 0, 200, 200)
    play_map(map, :white, 1, 1, 200, 200)
    play_map(map, :black, 9, 1, 200, 200)
    play_map(map, :white, 1, 2, 200, 200)
    play_map(map, :black, 9, 2, 200, 200)
    play_map(map, :white, 2, 2, 200, 200)
    play_map(map, :black, 0, 2, 200, 200)
    play_map(map, :white, 1, 3, 200, 200)
    play_map(map, :black, 9, 3, 200, 200)
    # not win (breakable)
    play_map(map, :white, 1, 4, 200, 200)
    # win not breakable
    body = play_map(map, :black, 9, 4, 200, 200).body.to_s
    assert_equal map.to_a, get_map
    assert_equal "You win.", JSON.parse(body)["message"]

    new_game
    map = Map.new
    assert_equal map.to_a, get_map
    play_map(map, :white, 1, 0, 200, 200)
    play_map(map, :black, 9, 0, 200, 200)
    play_map(map, :white, 1, 1, 200, 200)
    play_map(map, :black, 9, 1, 200, 200)
    play_map(map, :white, 1, 2, 200, 200)
    play_map(map, :black, 9, 2, 200, 200)
    play_map(map, :white, 2, 2, 200, 200)
    play_map(map, :black, 0, 2, 200, 200)
    play_map(map, :white, 1, 3, 200, 200)
    play_map(map, :black, 9, 3, 200, 200)
    # not win (breakable 1, 2)
    play_map(map, :white, 1, 4, 200, 200)
    # bkreakable 1,1
    play_map(map, :black, 0, 0, 200, 200)
    # unbreakable 1, 2
    play_map(map, :white, 3, 2, 200, 200)
    # random
    play_map(map, :black, 10, 10, 200, 200)
    # unbreakable 1, 1
    body = play_map(map, :white, 3, 3, 200, 200).body.to_s

    assert_equal map.to_a, get_map
    assert_equal "You win.", JSON.parse(body)["message"]
  end

  def test_free3
    return if ENV["DISABLE_FREE3"] == "true"
    new_game
    map = Map.new
    play_map(map, :white, 5, 5, 200, 200)
    play_map(map, :black, 0, 0, 200, 200)
    play_map(map, :white, 5, 6, 200, 200)
    play_map(map, :black, 0, 1, 200, 200)
    play_map(map, :white, 5, 7, 200, 200)
    play_map(map, :black, 0, 2, 200, 200)
    play_map(map, :white, 6, 7, 200, 200)
    play_map(map, :black, 0, 3, 200, 200)
    # echouer
    play_map(map, :white, 7, 7, 200, 401)
    play_map(map, :white, 8, 7, 200, 401)
    play_map(map, :white, 9, 7, 200, 200)
    play_map(map, :black, 8, 7, 200, 200)
    play_map(map, :white, 7, 7, 200, 200)
    #puts map.to_a.map{|e| e.map{|x| x||"."}.join(" ")}.join("\n")
  end

  private
  def play(color, y, x, status_wait=nil, status_end=nil, opt={})
    color = 1 if color == :white
    color = 2 if color == :black
    c = (color == 1 ? @c1 : @c2)
    co = (color == 1 ? @co1 : @co2)
    r = c.get(WAIT, {:headers => co})
    assert_equal status_wait, r.status if status_wait
    r = c.post(PLAY.gsub('X', x.to_s).gsub('Y', y.to_s), {headers: co})
    puts r.body.to_s if not r.status == 200
    assert_equal status_end, r.status if status_end
    if opt[:end_msg]
      assert_equal JSON.parse(r.body.to_s)["message"], opt[:end_msg]
    end
    r
  end

  def play_map(map, color, y, x, status_wait=nil, status_end=nil, opt={})
    color = 1 if color == :white
    color = 2 if color == :black
    r = play(color, y, x, status_wait, status_end, opt)
    if r.status == 200
      map[y][x] = color - 1
      map.update!(y, x)
      map.take_around!(y, x, color - 1)
    end
    r
  end

  def get_map(color=1)
    c = (color == 1 ? @c1 : @c2)
    co = (color == 1 ? @co1 : @co2)
    c.get(MAP, {:headers => co})
    JSON.parse(c.get(MAP, {:headers => co}).body.to_s)["map"] rescue nil
  end

end
