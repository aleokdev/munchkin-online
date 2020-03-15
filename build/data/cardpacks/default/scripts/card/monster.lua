-- #EDITOR_PROPERTIES
-- #PROPERTY power
-- #PROPERTY bad_stuff_script
-- #PROPERTY bad_stuff_script_args

local card = {}

function card:on_reveal()
	print("monster.lua on_reveal")
	print("self = " .. tostring(self))
	print("card = " .. tostring(card))
	for k,v in pairs(card) do
		print("\t"..tostring(k).." = "..tostring(v))
	end
	for k,v in pairs(card.properties) do
		print("\t"..tostring(k).." = "..tostring(v))
	end
	print("power = " .. tostring(card.properties.power))
	game:start_battle(self)
	game.current_battle:modify_card(self, card.properties.power)
end

function card:bad_stuff()
	if card.properties.bad_stuff_script then
		local bad_stuff_path = debug.getinfo(1).source:gsub("^@(.+/)[^/]+$", "%1").."../"..card.properties.bad_stuff_script
		local args = card.properties.bad_stuff_script_args
		local f = loadfile(bad_stuff_path)
		local _ENV = {_G=_G, args=args, pcall=pcall}
		pcall(f)
	end
end

print("monster.lua loaded")
return card