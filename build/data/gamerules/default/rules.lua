-- rules.lua
-- Defines specific API functions that determine the rules of a game (Such as when a game is finished, or who has won the game)

function game.winner() 
    for player in game.iter_players() do
        if player.level >= 10 then
            return player
        end
    end
    return nil
end

function game.has_ended()
    return game.winner() ~= nil
end