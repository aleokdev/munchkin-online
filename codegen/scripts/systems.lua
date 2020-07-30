local System = {
    new = function(self, o)
        o = o or {}
        setmetatable(o, self)
        self.__index = self
        return o
    end,
    __tostring = function(self) return string.format("systems::%s %s; /* priority: %i */", self.entity_name, self.member_name, (self.priority or 0)) end,
}
local game_systems = {}
local render_systems = {}

function process_entity(entity, filepath)
    for i, attr in ipairs(entity.attributes) do
        local t = {
            ["system_codegen::wrapper_instance"] = function()
                local sys = System:new { entity_name = entity.name, member_name = attr.arguments[1]:gsub("\"", ""), priority = tonumber(attr.arguments[2] or 0), filepath = filepath }
                table.insert(game_systems, sys);
            end,
            ["system_codegen::renderer_instance"] = function()
                local sys = System:new { entity_name = entity.name, member_name = attr.arguments[1]:gsub("\"", ""), priority = tonumber(attr.arguments[2] or 0), filepath = filepath }
                table.insert(render_systems, sys);
            end,
            __index = function()
                print("Unknown attribute: " .. attr.scope .. "::" .. attr.name)
            end
        }
        t[attr.scope .. "::" .. attr.name]()
    end
end

codegen:scan_dir("include/systems/", process_entity)

table.sort(game_systems, function(a, b)
    return (a.priority or 0) > (b.priority or 0)
end)
table.sort(render_systems, function(a, b)
    return (a.priority or 0) > (b.priority or 0)
end)

local game_systems_cg = codegen:new_output_file("include/game_systems_cg.hpp")
for i, system in ipairs(game_systems) do
    game_systems_cg:write("#include \"" .. system.filepath .. "\"\n")
end
game_systems_cg:write("#define MUNCHKIN_GAME_WRAPPER_SYSTEMS ")
for i, system in ipairs(game_systems) do
    game_systems_cg:write(tostring(system) .. (i < #game_systems and "\\\n\t\t" or "\n"))
end

local render_systems_cg = codegen:new_output_file("include/render_systems_cg.hpp")
for i, system in ipairs(render_systems) do
    render_systems_cg:write("#include \"" .. system.filepath .. "\"\n")
end
render_systems_cg:write("#define MUNCHKIN_RENDER_WRAPPER_SYSTEMS ")
for i, system in ipairs(render_systems) do
    render_systems_cg:write(tostring(system) .. (i < #render_systems and "\\\n\t\t" or "\n"))
end

local gamewrapper_cg = codegen:new_output_file("include/game_wrapper_defs_cg.hpp")
-- TODO: Make game/ai_manager more generic so that it can be constructed with GameWrapper
gamewrapper_cg:write([[
munchkin::GameWrapper::GameWrapper(size_t window_w,
                         size_t window_h,
                         size_t players_count,
                         size_t ai_count,
                         std::string gamerules_path) :
    game(players_count, window_w, window_h, gamerules_path),
    ai_manager(create_ai_manager(players_count, ai_count)),
    renderer(*this),
    ]])

for i, system in ipairs(game_systems) do
    gamewrapper_cg:write(system.member_name .. "(*this)" .. (i < #game_systems and ", " or " {}"))
end

gamewrapper_cg:write("\n\n" .. [[
void munchkin::GameWrapper::load_all_systems_content() {
    renderer::Background::load_content();]] .. "\n\t")

for i, system in ipairs(game_systems) do
    gamewrapper_cg:write(system.member_name .. ".load_content();\n\t")
end
for i, system in ipairs(render_systems) do
    gamewrapper_cg:write("renderer." .. system.member_name .. ".load_content();\n\t")
end

gamewrapper_cg:write("\n}")

local renderwrapper_cg = codegen:new_output_file("include/render_wrapper_defs_cg.hpp")
for i, system in ipairs(render_systems) do
    renderwrapper_cg:write("#include \"" .. system.filepath .. "\"\n")
end
-- TODO: Make game/ai_manager more generic so that it can be constructed with GameWrapper
renderwrapper_cg:write([[
munchkin::RenderWrapper::RenderWrapper(munchkin::GameWrapper& w) :
    wrapper(&w),
    framebuf(renderer::RenderTarget::CreateInfo{wrapper->game.window_w, wrapper->game.window_h}),
    projection(
        glm::ortho(0.0f, (float)wrapper->game.window_w, 0.0f, (float)wrapper->game.window_h)),
    ]])

for i, system in ipairs(render_systems) do
    renderwrapper_cg:write(system.member_name .. "(*this)" .. (i < #render_systems and ", " or " {}"))
end