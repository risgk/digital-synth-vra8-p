require_relative 'constants'

def low_byte(x)
  return x & 0xFF
end

def high_byte(x)
  return x >> 8
end

def high_sbyte(x)
  return x >> 8
end
