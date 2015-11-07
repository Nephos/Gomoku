require 'digest'

module Player

  CODES = (0..2**63)
  def self.new_code
    Digest::SHA256.hexdigest(rand(CODES).to_s)
  end

end
