class Map

  # T is the list of the direction, an array of [y, x]
  T = [
    [-1, -1], [-1, 0], [-1, 1],
    [0, -1], [0, 1],
    [1, -1], [1, 0], [1, 1],
  ]

  T_CAPTURE = [
    [[-1, -1], [1, 1]],
    [[-1, 0], [1, 0]],
    [[-1, 1], [1, -1]],
    [[0, 1], [0, -1]],
  ]

  attr_reader :size, :data, :took, :capturable, :free3
  attr_accessor :moves

  def initialize size=19
    @size = size
    @data = Array.new(size){Array.new(size) {nil}}
    @capturable = Array.new(size){Array.new(size) {false}}
    @free3 = Array.new(size){Array.new(size) {false}}
    @took = [0, 0]
    @moves = []
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

  def to_s
    self.to_a.stringify_map
  end

  public
  def valid_place_for? x, y, color
    return 1 if x < 0 or x >= 19
    return 2 if y < 0 or y >= 19
    return 3 if @data[y][x]
    return 5 if not @moves.empty? and not @moves.include? [x, y]
    return nil
  end
  def valid_xy? x, y
    return false if y < 0 or y >= 19 or x < 0 or x >= 19
    return true
  end

  def update! y, x
    color = @data[y][x]
    update_capturable!(y, x, color)
    #update_free3!(y, x, color)
  end

  private
  def update_capturable! y, x, color
    @capturable[y][x] = false
    return false if @data[y][x].nil?
    T_CAPTURE.each do |tuples|
      #puts "TEST FOR #{tuples} at #{y}#{x}"
      y2,x2 = y + tuples[0][0], x + tuples[0][1]
      y3,x3 = y + tuples[1][0], x + tuples[1][1]
      #puts "2: #{y2}#{x2}, 3:#{y3}#{x3}"
      next if not valid_xy? x2, y2
      next if not valid_xy? x3, y3
      #puts :test
      a = @data[y2][x2]
      b = @data[y3][x3]
      ra = a == color && b == nil
      rb = b == color && a == nil
      if ra or rb
        @capturable[y][x] = true
        @capturable[y2][x2] = true if ra
        @capturable[y3][x3] = true if rb
        #puts :ra if ra
        #puts :rb if rb
      #else
        #puts :miss
      end
      ra||rb
    end
  end

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
      update!(y, x)
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
    fives = fives(color)
    return false if fives.empty?
    #return false if fives.empty?
    breakables = fives.map{|five| breakable_in(five, color)}
    @moves = breakables.inject(&:&) || []
    return true if @moves.empty? # win if cannot break all with one mov
    return true
  end

  private
  def fives color
    # for each line and each cell
    fives = []
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
          fives << [y, x, tuple] if c == color and win_direction? tuple, y2, x2, color
        end
      end
    end
    return fives
  end

  def breakable_in five, color
    y,x,t = five
    breakables = []
    while @data[y][x] == color
      breakables << [y, x] if @capturable[y][x]
      y += t[0]
      x += t[1]
    end
    return breakables
  end

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

  class Array
    def stringify_map(null='x')
      map{|e| e.map{|x| x||null}.join(" ")}.join("\n")
    end
  end

  require 'pry'
  m = Map.new
  m[0][0] = 0
  m[0][1] = 0
  m.update! 0, 0
  m.update! 0, 1
  m.update! 0, 2
  puts m.data.stringify_map
  puts m.capturable.stringify_map
  #assert 2, m.capturable.select{|e| e == true}

  m[0][2] = 1
  m.update! 0, 0
  m.update! 0, 1
  m.update! 0, 2
  puts m.data.stringify_map
  puts m.capturable.stringify_map
  #assert 0, m.capturable.select{|e| e == true}

  binding.pry # debug zone it's ok
end
