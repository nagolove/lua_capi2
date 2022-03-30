--[[
local record Factory
    record Object end
    record Buffer end
    new_object: function(): Object
    new_buffer: function(): Buffer
    process: function(Buffer)
end
--]]
print("start")
local o = factory.new_object()

--o:do_something()

print(tostring(o))

local b = factory.new_buffer()

--b.does()
--b:does()
b:do1()
b:do2()
b:do3()
--b:process(o)

empty()

system.empty()

print(tostring(b))

print('factory.process', factory.process)

local ok, errmsg = pcall(function()
    factory.process(o)
    --error("Беда случилась, память кончилась.")
end)
if not ok then
    error("Could not process factory with " .. errmsg)
end
--factory.process(b)

print("end")
