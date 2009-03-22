#!/usr/bin/env lua
-- envtag script to capitalize artist names
-- vim: set ft=lua et sts=4 sw=4 ts=4 fdm=marker:
-- Copyright 2009 Ali Polatel <polatel@gmail.com>
-- Distributed under the terms of the GNU General Public License v2

local gsub = string.gsub
local upper = string.upper
local sub = string.sub

function capitalize(str)
    return gsub(str, "(%w+)", function (c) return upper(sub(c, 1, 1)) .. sub(c, 2) end)
end

artist = assert(tag.get"artist")
cartist = capitalize(artist)
if VERBOSE then print(artist, "-->", cartist) end
tag.set("artist", cartist)
