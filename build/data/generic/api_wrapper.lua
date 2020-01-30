-- api_wrapper.lua
-- Defines API functions pretty much generic to all game rules, such as iter_players
-- and wrappers for the state variable

-- Note: Try to stay away from IDs! They cause confusion, since they start at 0!
-- Use game.iter_players() and other wrappers instead!

-- munchkin_state

function munchkin_state.iter_players(self)
    local i = -1
    return function()
        i = i + 1
        if i == self:get_player_count() then return end
        return self:get_player(i)
    end
end

function munchkin_state.next_player_turn(self)
    self:set_current_player((self:get_current_player().id + 1) % self:get_player_count())
    self.turn_number = self.turn_number + 1
end

-- input

selection = {}

function selection.choose_card(filter)
    filter = filter or function() return true end

    repeat
        coroutine.yield()
    until game.last_event.type == event_type.card_clicked and filter(game.last_event.card_involved)

    return game.last_event.card_involved
end

-- Utils

local function wait_for_event(ev)
	repeat
		coroutine.yield()
	until game.last_event.type == ev
end

local function wait_for_ticks(ticks)
	local ticks_before = game:get_ticks()
	repeat
		coroutine.yield()
	until game:get_ticks() >= (ticks_before + ticks)
end

-- Waits for a number or ticks, or until an event has happened.
-- Returns true if waited for all the ticks, false if the event happened first.
local function wait_for_ticks_or_event(ev, ticks)
	local ticks_before = game:get_ticks()
	repeat
		coroutine.yield()
	until game:get_ticks() >= (game:get_ticks() + ticks) or game.last_event.type == ev
	return game:get_ticks() >= (game:get_ticks() + ticks)
end