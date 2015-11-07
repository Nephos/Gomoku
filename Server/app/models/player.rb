require 'digest'

class Player

  attr_reader :color, :code

  def initialize color
    raise ArgumentError unless ["white", "black"].include? color
    @color = color
    @code = nil
  end

  def busy?
    @code != nil
  end

  def finish!
    @code = nil
  end

  CODES = (0..2**63)
  def reset!
    @code = Digest::SHA256.hexdigest(rand(CODES).to_s)
  end

  def to_hash
    {color: color}
  end

end
