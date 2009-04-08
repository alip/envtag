#!/usr/bin/env lua
-- Common functions for envtag scripts
-- vim: set ft=lua et sts=4 sw=4 ts=4 fdm=marker:
-- Copyright 2009 Ali Polatel <polatel@gmail.com>
-- Distributed under the terms of the GNU General Public License v2

local sub = string.sub
local gsub = string.gsub
local insert = table.insert

module "envtag"

TAGS_COMMON = {
    "title", "artist", "album", "comment",
    "genre", "year", "track"
}
TAGS_XIPH = {
    "title", "version", "album", "artist",
    "performer", "copyright", "organization", "description",
    "genre", "date", "location", "contact", "isrc"
}

function escapeq(str)
    return gsub(str, "'", "'\\''")
end

function split(str, char)
    local chunk = ""
    local list = {}
    local i = 1

    while i <= #str do
        local c = sub(str, i, i)
        if c == char then
            insert(list, chunk)
            chunk = ""
        else
            chunk = chunk .. c
        end
        i = i + 1
    end
    insert(list, chunk)
    return list
end

