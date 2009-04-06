#!/usr/bin/env lua
-- envtag script to set tags
-- vim: set ft=lua et sts=4 sw=4 ts=4 fdm=marker:
-- Copyright 2009 Ali Polatel <polatel@gmail.com>
-- Distributed under the terms of the GNU General Public License v2

do
    local sub = string.sub
    local upper = string.upper
    local insert = table.insert
    local getenv = os.getenv

    local function split(str, char)
        local chunk = ""
        local list = {}
        local i = 1

        while i <= #str do
            local c = string.sub(str, i, i)
            if c == char then
                table.insert(list, chunk)
                chunk = ""
            else
                chunk = chunk .. c
            end
            i = i + 1
        end
        table.insert(list, chunk)
        return list
    end

    if tag.has_xiph() then
        -- Xiph Comment
        xiph_tags = {"title", "version", "album", "artist",
                     "performer", "copyright", "organization", "description",
                     "genre", "date", "location", "contact", "isrc"}
        for _, tname in ipairs(xiph_tags) do
            local t = getenv(upper(tname))
            if t then
                tag.set_xiph(tname, opt.append, split(t, opt.delimiter))
            end
        end
    else
        -- Common tags
        common_tags = {"title", "artist", "album", "comment", "genre", "year", "track"}
        for _, tname in ipairs(common_tags) do
            local t = getenv(upper(tname))
            if t then tag.set(tname, (tname == "year" or tname == "track") and tonumber(t) or t) end
        end
    end
    assert(file.save(), "failed to save `" .. file.name .. "'")
end
