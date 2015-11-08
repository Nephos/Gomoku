require 'pry'
require 'colorize'
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
  PLAY = BASE + "game/play/X/Y"

  def test_game
    @c1 = HTTP::Client.new
    @c2 = HTTP::Client.new
    @r1 = @c1.get CONN
    @r2 = @c2.get CONN
    @co1 = @r1.cookies.map{|e| e.split(';').first}
    @co2 = @r2.cookies.map{|e| e.split(';').first}
    @co1 = {"Cookie" => @co1}
    @co2 = {"Cookie" => @co2}

    puts "J1: #{@co1}"
    puts "J2: #{@co2}"

    play(1, 0, 0, 200, 200)
    play(2, 1, 0, 200, 200)
    play(1, 0, 1, 200, 200)
    play(2, 2, 0, 200, 200)
    play(1, 0, 2, 200, 200)
    play(2, 2, 0, 200, 200)
    play(1, 0, 3, 200, 200)
    play(2, 2, 0, 200, 200)
    play(1, 0, 4, 200, 200, {end_msg: "You win."})
  end

  def play(color, x, y, status_wait=nil, status_end=nil, opt={})
    c = (color == 1 ? @c1 : @c2)
    co = (color == 1 ? @co1 : @co2)
    r = c.get(WAIT, {:headers => co})
    puts r
    assert_equal status_wait, r.status if status_wait
    r = c.post(PLAY.gsub('X', '0').gsub('Y', '0'), {headers: co})
    puts r
    assert_equal status_end, r.status if status_end
    if opt[:end_msg]
      assert_eql JSON.parse(r.body.to_s)["message"], opt[:end_msg]
    end
    r
  end

end
