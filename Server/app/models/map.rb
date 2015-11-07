class Map

  attr_reader :size, :data

  def initialize size=19
    @size = 19
    @data = Array.new(19){Array.new(19) {nil}}
  end

  def method_missing m, *a
    @data.send(m, *a)
  end

  def to_a
    @data
  end

  def win?
    false
  end

  T = [
    [-1, -1], [-1, 0], [-1, 1],
    [0, -1], [0, 1],
    [1, -1], [1, 0], [1, 1],
  ]
  def take_around! y, x, color
    T.each do |tuple|
      y2, x2 = y + tuple[0], x + tuple[1]
      c = @data[y2][x2]
      #puts "Try #{c} at #{y2}:#{x2}"
      if c && c != color
        #puts "Direction found: #{tuple}"
        take_direction! tuple, y2, x2, color
      end
    end
  end

  private
  def take_direction! tuple, y, x, color
    #puts "Check at #{y}:#{x}: #{@data[y][x].class}"
    case @data[y][x]
    when nil
      return false
    when color
      #puts "Direction validated (#{tuple}) at #{y}:#{x}"
      return true
    end
    if take_direction! tuple, y + tuple[0], x + tuple[1], color
      @data[y][x] = color
      return true
    end
  end

end

if __FILE__ == $0
  map = Map.new
  map[0][0] = 0
  map[0][1] = 1
  require 'pry'
  binding.pry
end
