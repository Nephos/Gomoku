puts "Please be sure you started the server on 8080"

class TestGame < Test::Unit::TestCase

  BASE = "http://localhost:8080/"
  CONN = BASE + "players/connect"
  WAIT = BASE + "game"
  PLAY = BASE + "game/X/Y"
  def test_connection
    c1 = HTTP::Client.new
    c2 = HTTP::Client.new
    assert(r1 = c1.get(CONN))
    assert(r2 = c2.get(CONN))
    assert_equal 200, r1.status
    assert_equal 200, r2.status
    assert JSON.parse(r1.body.to_s)["message"].match(/connected to [0-9a-f]{64} as (white|black)/)
    assert JSON.parse(r2.body.to_s)["message"].match(/connected to [0-9a-f]{64} as (white|black)/)
  end

end
