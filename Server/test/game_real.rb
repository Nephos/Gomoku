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
    play(2, 1, 0, 200, 200)
    play(1, 0, 1, 200, 200)
    play(2, 2, 0, 200, 200)
    play(1, 0, 2, 200, 200)
    play(2, 3, 0, 200, 200)
    play(1, 0, 3, 200, 200)
    play(2, 4, 0, 200, 200)
    play(1, 0, 4, 200, 200, {end_msg: "You win."})
  end

  def test_game_simple_map
    new_game
    map = Map.new
    assert_equal map.to_a, get_map
    play(1, 0, 0, 200, 200)
    map[0][0] = 0
    assert_equal map.to_a, get_map

    play(2, 1, 0, 200, 200)
    map[0][1] = 1
    assert_equal map.to_a, get_map

    play(1, 0, 1, 200, 200)
    map[1][0] = 0
    assert_equal map.to_a, get_map

    play(2, 2, 0, 200, 200)
    map[0][2] = 1
    assert_equal map.to_a, get_map

    play(1, 0, 2, 200, 200)
    map[2][0] = 0
    assert_equal map.to_a, get_map

    play(2, 3, 0, 200, 200)
    map[0][3] = 1
    assert_equal map.to_a, get_map

    play(1, 0, 3, 200, 200)
    map[3][0] = 0
    assert_equal map.to_a, get_map

    play(2, 4, 0, 200, 200)
    map[0][4] = 1
    assert_equal map.to_a, get_map

    body = play(1, 0, 4, 200, 200).body.to_s
    map[4][0] = 0
    assert_equal map.to_a, get_map
    assert_equal "You win.", JSON.parse(body)["message"]
  end

  def test_simple_take
    new_game
    map = Map.new
    play(1, 0, 0, 200, 200); map[0][0] = 0
    play(2, 0, 1, 200, 200); map[1][0] = 1
    play(1, 0, 9, 200, 200); map[9][0] = 0
    play(2, 0, 2, 200, 200); map[2][0] = 1
    play(1, 0, 3, 200, 200); map[3][0] = 0
    map[1][0] = nil
    map[2][0] = nil
    assert_equal map.to_a, get_map
  end

  def test_double_take
    new_game
    map = Map.new
    play_map(map, 1, 0, 1) # 1
    play_map(map, 2, 1, 1)
    play_map(map, 1, 1, 0) # 2
    play_map(map, 2, 2, 1)
    play_map(map, 1, 0, 0) # 3
    play_map(map, 2, 3, 1)
    play_map(map, 1, 4, 1) # 4
    play_map(map, 2, 1, 2)
    _ = play_map(map, 1, 1, 3).body.to_s # 5
    assert_equal 0, get_map.count(1)
    # continue
  end

  def play(color, x, y, status_wait=nil, status_end=nil, opt={})
    c = (color == 1 ? @c1 : @c2)
    co = (color == 1 ? @co1 : @co2)
    r = c.get(WAIT, {:headers => co})
    assert_equal status_wait, r.status if status_wait
    r = c.post(PLAY.gsub('X', x.to_s).gsub('Y', y.to_s), {headers: co})
    puts r.body.to_s
    assert_equal status_end, r.status if status_end
    if opt[:end_msg]
      assert_equal JSON.parse(r.body.to_s)["message"], opt[:end_msg]
    end
    r
  end

  def play_map(map, color, x, y, status_wait=nil, status_end=nil, opt={})
    r = play(color, x, y, status_wait, status_end, opt)
    if r.status == 200
      map[y][x] = color - 1
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
