require_relative 'models/player'

class PlayerController < Nephos::Controller

  @@p = {
    "white" => Player.new("white"),
    "black" => Player.new("black"),
  }
  def self.all
    @@p
  end
  def self.disconnect!
    @@p["white"].finish!
    @@p["black"].finish!
  end

  def index
    return {json: {data: {p1: @@p["white"].to_hash, p2: @@p["black"].to_hash} }}
  end

  def connect1
    connect "white"
  end

  def connect2
    connect "black"
  end

  private
  def connect color
    #require 'pry' ; binding.pry
    return connect_err if @@p[color].busy?
    return connect_new color
  end

  def connect_err
    return {json: {message: "failure"}, status: 401}
  end

  def connect_new color
    @@p[color].reset!
    cookies[:color] = color
    cookies[:code] = @@p[color].code
    GameController.start_new_game! if @@p.count{|c, p| p.busy?} == 2
    return {json: {message: "connected"}, status: 200}
  end

end
