class Map

  attr_reader :size, :data

  def initialize size=19
    @size = 19
    @data = Array.new(19){Array.new(19) {nil}}
  end

  def to_a
    @data
  end

end
