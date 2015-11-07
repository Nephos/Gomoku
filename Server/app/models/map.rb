class Map

  attr_reader :size, :data

  def initialize size=19
    @size = 19
    @data = Array.new(19){Array.new(19) {nil}}
  end

  # we can use map like an array
  def method_missing m, *a
    @data.send(m, *a)
  end

  def to_a
    @data
  end

  # T is the list of the direction, an array of [y, x]
  T = [
    [-1, -1], [-1, 0], [-1, 1],
    [0, -1], [0, 1],
    [1, -1], [1, 0], [1, 1],
  ]

  public
  # try to take every lines around (y, x)
  # use the directions T
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

  # change elements "2" to "color"
  def took! color
    @data.map!{|line| line.map{|e| e == 2 ? color : e}}
  end

  private
  # try to take the line with the direction "tuple"
  # from (y, x)
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
      @data[y][x] = 2
      take_around! y, x, color
      return true
    end
  end

  public
  # for every "color" or "2"
  # test with every lines, based on T and (y, x)
  # if there is a 5 aligned "color" or "2"
  def win? color
    @data.each_with_index do |line, y|
      line.each_with_index do |e, x|
        next if e != 2 and e != color
        T.each do |tuple|
          y2, x2 = y + tuple[0], x + tuple[1]
          c = @data[y2][x2]
         return true if c == color and win_direction? tuple, y2, x2, color
        end
      end
    end
    return false
  end

  private
  # test a line
  def win_direction? tuple, y, x, color, distance=0
    return true if distance == 4
    case @data[y][x]
    when color
      return win_direction?(tuple, y + tuple[0], x + tuple[1], color, distance + 1)
    else
      return false
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
