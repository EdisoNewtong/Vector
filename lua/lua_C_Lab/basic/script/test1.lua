
-- 0. create a userdata
rgb1 = lua_createRGBAObject();

-- 1. set value
rValue = rgb1.r
gValue = rgb1.g
bValue = rgb1.b
aValue = rgb1.a
xxxValue = rgb1.xxx
print("Before setfield : 1. rgb1 in detail\n      r = ",rValue, "g = ",gValue, "b = ",bValue, "a = ",aValue, "xxxValue = ",xxxValue)




-- 2. set value
rgb1.r = -100
rgb1.g = -200
rgb1.b = -300
rgb1.a = -400

rValue = rgb1.r
gValue = rgb1.g
bValue = rgb1.b
aValue = rgb1.a
print("After  setfield : 2. rgb1 in detail\n      r = ",rValue, "g = ",gValue, "b = ",bValue, "a = ",aValue, "xxxValue = ",xxxValue)

