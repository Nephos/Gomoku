require_relative 'models/map'

class MapControler < Nephos::Controller
  @@map = Map.new

  def index
    return {json: {data: @@map.to_a}}
  end
end
