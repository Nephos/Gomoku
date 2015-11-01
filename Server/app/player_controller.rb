require_relative 'models/player'

class PlayerController < Nephos::Controller

  @@p = {white: Player.new(:white), black: Player.new(:black)}

  def index
    return {json: {data: {p1: @@p[:white].to_hash, p2: @@p[:black].to_hash} }}
  end

  def connect1
    connect :white
  end

  def connect2
    connect :whate
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
    return {json: {message: "connected"}, status: 200}
  end

end
