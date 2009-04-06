#!/usr/bin/env lua
-- envtag script to capitalize artist names
-- vim: set ft=lua et sts=4 sw=4 ts=4 fdm=marker:
-- Copyright 2009 Ali Polatel <polatel@gmail.com>
-- Distributed under the terms of the GNU General Public License v2

local gsub = string.gsub
local upper = string.upper
local sub = string.sub
local insert = table.insert

function capitalize(str)
    return gsub(str, "(%w+)", function (c) return upper(sub(c, 1, 1)) .. sub(c, 2) end)
end

if tag.has_xiph() then -- Xiph Comment interface
    artist = tag.get_xiph("artist", opt.unicode)
else -- Common Tag interface
    artist = tag.get("artist", opt.unicode)
end

if artist then -- Artist set
    if tag.has_xiph() then -- Xiph Comment
        local at = {}
        for _, name in ipairs(artist) do
            local cname = capitalize(name)
            if opt.verbose then
                print(name, "-->", cname)
            end
            insert(at, cname)
        end
        -- Clear ARTIST field
        tag.set_xiph("artist", false, {""})
        -- Set new ARTIST field
        tag.set_xiph("artist", true, at)
    else -- Common Tag
        local cartist = capitalize(artist)
        if opt.verbose then
            print(artist, "-->", cartist)
        end
        tag.set("artist", cartist)
    end
    file.save()
end

