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
  T_FREE3 = T_CAPTURE

  attr_reader :size, :data, :took, :capturable, :free3

  def initialize size=19
    @id = 0
    @size = size
    @data = Array.new(size){Array.new(size) {nil}}
    @capturable = Array.new(size){Array.new(size) {false}}
    @free3 = Array.new(size){Array.new(size) {[]}}
    @took = [0, 0]
    @free3_list = {}
  end

  def newid!
    id = @id
    @id += 1
    id
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
    return 4 if make_free3(y, x, color) >= 2
    #return 5 if not @moves.empty? and not @moves.include? [x, y]
    return nil
  end
  def valid_xy? x, y
    return (y >= 0 and y <= 18 and x >= 0 and x <= 18)
  end
  def border? x, y
    return (y == 0 or x == 0 or y == 18 or x == 18)
  end

  def update! y, x
    update_capturablex!(y, x)
    update_free3!(y, x)
  end

  ### Free3 section ###
  private
  public # remove
  def make_free3 y, x, color
    @data[y][x] = color
    all_points = update_free3! y, x
    @data[y][x] = nil
    return (all_points.map{|y2, x2| @free3_cpy[y2][x2] }.inject(&:+) ||[]).uniq.size
  end

  def update_free3! y, x
    color = @data[y][x]
    @free3_cpy = Marshal.load(Marshal.dump(@free3))
    break_free3!(y, x)
    @free3_cpy[y][x] = []
    all_points = []

    T_FREE3.each do |tuples|
      @allies = []
      @borders = []
      @ext_borders = []
      void = 0
      allies = 1
      freeb = 0
      void, allies, freeb = free3_tests(void, allies, freeb, y, x, tuples[0], color, false)
      if freeb > 0
        void, allies, freeb = free3_tests(void, allies, freeb, y, x, tuples[1], color, (void == 1 && allies != 3))
      end
      if void <= 1 and allies == 3 and freeb >= 3
        id = newid!
        points = (@allies + @borders + [[y, x]])
        @free3_list[id] = {allies: points, borders: @ext_borders}
        all_points << points
        points.each do |y2, x2|
          @free3_cpy[y2][x2] << id
        end
      end
    end
    return (all_points.map{|e| e||[] }.inject(&:+) ||[] ).uniq
  end

  def free3_tests(void, allies, freeb, y, x, tuple, color, reset_void)
    y2, x2 = y+tuple[0], x+tuple[1]
    return [void, allies, freeb] if not valid_xy? y2, x2 # out of range
    e = @data[y2][x2]

    # new ally
    if e == color
      if allies < 4
        @allies << [y2, x2, tuple] # tag ally
        return free3_tests(void, allies+1, freeb, y2, x2, tuple, color, reset_void)
      end
      return [0, 4, 0] # nope nope nope

    # nil
    elsif e == nil
      y3, x3 = y2+tuple[0], x2+tuple[1]
      f = @data[y3][x3]

      # border
      if not valid_xy? y3, x3
        @borders << [y2, x2]
        return [void, allies, freeb+1]

      #ally 2 cases away
      elsif f == color
        # void allowed
        if void == 0
          @allies << [y2, x2]
          @allies << [y3, x3] # tag ally
          return free3_tests(void+1, allies+1, freeb, y3, x3, tuple, color, reset_void)

        # void allowed by resetting the first
        elsif void == 1 and reset_void == true
          @borders = [@allies.shift] # void becomes border
          @allies.clear # untag allies
          @allies << [y2, x2] # tag void
          @allies << [y3, x3] # tag ally
          return free3_tests(void, allies, freeb, y3, x3, tuple, color, false)

        # void disallowed
        else
          @borders << [y2, x2]
          return [void, allies, freeb+1]
        end

      # . nil nil
      elsif f == nil
        @borders << [y2, x2]
        @borders << [y3, x3]
        @ext_borders << [y3, x3]
        return [void, allies, freeb+2]
      end

      # enemy 2 cases away
      @borders << [y2, x2]
      return [void, allies, freeb+1] if e != f
    end

    # enemy
    return [void, allies, freeb]
  end

  def break_free3! y, x
    @free3_cpy[y][x].each do |id|
      if @free_list[id][:borders].include?([y, x]) and @free_list[id][:borders].size == 2
        @free_list[id][:borders].delete([y, x])
        next
      end
      @free_list[id].each do |y2, x2|
        @free3_cpy[y2][x2].delete id
      end
      @free_list.delete id
    end
  end

  def save_free3!
    @free3 = @free3_cpy
  end

  ### Capturable section (5) ###

  private
  def update_capturablex! y, x
    @i = 0
    @capture = []
    update_capturable!(y, x)
    @capture.each do |cy, cx|
      @capturable[cy][cx] = true
    end
  end

  def update_capturable! y, x, rec=true
    @i += 1
    color = @data[y][x]
    @capturable[y][x] = false

    T.each do |tuple|
      y2, x2 = y+tuple[0], x+tuple[1]
      #@capturable[y2][x2] = false
      update_capturable!(y2, x2, false) if @capturable[y2][x2] == true or rec
    end

    return if color.nil? # handled by recursion

    T_CAPTURE.each do |tuples|
      y2, x2 = y+tuples[0][0], x+tuples[0][1]
      y22, x22 = y+tuples[0][0]*2, x+tuples[0][1]*2
      y3, x3 = y+tuples[1][0], x+tuples[1][1]
      y33, x33 = y+tuples[1][0]*2, x+tuples[1][1]*2
      next if not valid_xy?(x2, y2)
      next if not valid_xy?(x3, y3)
      r2, r3 = @data[y2][x2], @data[y3][x3]
      r22, r33 = @data[y22][x22], @data[y33][x33]
      next if r2 == r3
      next if r2 == nil and r3 != color
      next if r3 == nil and r2 != color
      if r2 == color
        next if not valid_xy?(x22, y22)
        next if r22 == color
        next if r22 == r3
        update_capturable_case!(y, x, y2, x2)
      elsif r3 == color
        next if not valid_xy?(x33, y33)
        next if r33 == color
        next if r33 == r2
        update_capturable_case!(y, x, y3, x3)
      end
    end
  end

  def update_capturable_case!(y, x, y2, x2)
    #@capturable[y][x] = true
    #@capturable[y2][x2] = true
    @capture << [y, x]
    @capture << [y2, x2]
  end

  ### Take section ###

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

  ### Win section ###

  public
  # for every "color" or "2"
  # test with every lines, based on T and (y, x)
  # if there is a 5 aligned "color" or "2"
  def win? color
    return true if @took[color] >= 10
    fives = fives(color)
    return false if fives.empty?
    breakables = fives.map{|five| breakable_in(five, color)}
    moves = breakables.inject(&:&) || []
    return true if moves.empty? # win if cannot break all with one mov
    return false
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
