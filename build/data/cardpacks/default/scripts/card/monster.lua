-- #EDITOR_PROPERTIES
-- #PROPERTY power
-- #PROPERTY bad_stuff_script
-- #PROPERTY bad_stuff_script_args

local card = {}

function card:on_reveal()
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

return card