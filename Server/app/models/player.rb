require 'digest'

class Player

  attr_reader :win, :color, :code

  def initialize color
    raise ArgumentError unless [:white, :black].include? color
    @color = color
    @code = nil
    @win = nil
  end

  def busy?
    @code != nil
  end

  def finish!
    @code = nil
  end

  CODES = (0..2**63)
  def reset!
    @win = nil
    @code = Digest::SHA256.digest(rand(CODES).to_s)
  end

  def to_hash
    {win: win, color: color}
  end

end
