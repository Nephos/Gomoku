class Map

  attr_reader :size, :data, :took

  def initialize size=19
    @size = size
    @data = Array.new(size){Array.new(size) {nil}}
    @took = [0, 0]
  end

  def took_hash
    {"white" => @took[0], "black" => @took[1]}
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
    #puts "take around #{x} #{y} (#{color})"
    # check every directions
    T.each do |tuple|
      y2, x2 = y + tuple[0], x + tuple[1]
      next unless valid_xy? x2, y2
      c = @data[y2][x2]
      if c && c != color
        take_direction! tuple, y2, x2, color
      end
    end
  end

  private
  def valid_xy? x, y
    return false if y < 0 or y >= 19 or x < 0 or x >= 19
    return true
  end
  # try to take the line with the direction "tuple"
  # from (y, x)
  # if a point is captured, then it will try to take every point around itself
  def take_direction! tuple, y, x, color, n=1
    #puts "take direction #{x} #{y} (#{color})"
    #puts "Check at #{y}:#{x}: #{@data[y][x].class}"
    case @data[y][x]
    when nil
      return false
    when color
      #puts "Direction validated (#{tuple}) at #{y}:#{x}"
      return n == 3 # true if end on the 3rd ball
    end
    y2, x2 = y + tuple[0], x + tuple[1]
    return false unless valid_xy? x2, y2
    if take_direction! tuple, y2, x2, color, n+1
      @data[y][x] = nil
      @took[color] += 1
      #take_around! y, x, color
      return true
    end
  end

  public
  # for every "color" or "2"
  # test with every lines, based on T and (y, x)
  # if there is a 5 aligned "color" or "2"
  def win? color
    return true if @took[color] >= 10
    # for each line and each cell
    @data.each_with_index do |line, y|
      line.each_with_index do |e, x|
        # do not computes if not 2 (took this round) or same color
        next if e != color
        # for each direction name tuple
        T.each do |tuple|
          # calculate position of the next element
          y2, x2 = y + tuple[0], x + tuple[1]
          # if at border, end
          next unless valid_xy? x2, y2
          c = @data[y2][x2]
          # check if the direction and win if one direction is true
         return true if c == color and win_direction? tuple, y2, x2, color
        end
      end
    end
    return false
  end

  private
  # test a line
  def win_direction? tuple, y, x, color, distance=0
    # if we found 4 items + the base (from "win?") it's finished
    return true if distance == 4
    case @data[y][x]
    when color
      # check the next case with the direction if the color is right
      y2, x2 = y + tuple[0], x + tuple[1]
      return false unless valid_xy? x2, y2
      return win_direction?(tuple, y2, x2, color, distance + 1)
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
