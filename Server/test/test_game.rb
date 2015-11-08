puts "Please be sure you started the server on 8080"
require 'pry'

module HTTP
  class Response
    def cookies
      self.headers["Set-Cookie"]
    end
  end
end

class TestGame < Test::Unit::TestCase

  BASE = "http://localhost:8080/"
  CONN = BASE + "players/connect"
  WAIT = BASE + "game"
  PLAY = BASE + "game/X/Y"

  INIT_WAIT_REP = {"message"=>"It's your turn",
                   "map"=>
                   [[nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil],
                    [nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil],
                    [nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil],
                    [nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil],
                    [nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil],
                    [nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil],
                    [nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil],
                    [nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil],
                    [nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil],
                    [nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil],
                    [nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil],
                    [nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil],
                    [nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil],
                    [nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil],
                    [nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil],
                    [nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil],
                    [nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil],
                    [nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil],
                    [nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil]]}

  def test_connection
    c1 = HTTP::Client.new
    c2 = HTTP::Client.new
    assert(r1_1 = c1.get(CONN))
    assert(r2_1 = c2.get(CONN))
    assert_equal 200, r1_1.status
    assert_equal 200, r2_1.status
    assert JSON.parse(r1_1.body.to_s)["message"].match(/connected to [0-9a-f]{64} as (white|black)/)
    assert JSON.parse(r2_1.body.to_s)["message"].match(/connected to [0-9a-f]{64} as (white|black)/)
    co1 = r1_1.cookies.map{|e| e.split(';').first}
    co2 = r2_1.cookies.map{|e| e.split(';').first}
    # h_co1= {"Cookie" => Hash[co1.map{|e|e.split('=')}]}
    # h_co2= {"Cookie" => Hash[co2.map{|e|e.split('=')}]}
    h_co1 = {"Cookie" => co1}
    h_co2 = {"Cookie" => co2}
    r1_2 = c1.get(WAIT, {:headers => h_co1})
    assert_equal INIT_WAIT_REP, JSON.parse(r1_2.body.to_s)
    #r2_2 = c1.get(WAIT, {:headers => h_co2})

    # binding.pry
  end

end
