#!/usr/bin/env lua
-- envtag script to set tags
-- vim: set ft=lua et sts=4 sw=4 ts=4 fdm=marker:
-- Copyright 2009 Ali Polatel <polatel@gmail.com>
-- Distributed under the terms of the GNU General Public License v2

require "envtag"

do
    local sub = string.sub
    local upper = string.upper
    local insert = table.insert
    local getenv = os.getenv

    local split = envtag.split

    if tag.has_xiph() then
        -- Xiph Comment
        for _, tname in ipairs(envtag.TAGS_XIPH) do
            local t = getenv(upper(tname))
            if t then
                tag.set_xiph(tname, opt.append, split(t, opt.delimiter))
            end
        end
    else
        -- Common tags
        for _, tname in ipairs(envtag.TAGS_COMMON) do
            local t = getenv(upper(tname))
            if t then tag.set(tname, (tname == "year" or tname == "track") and tonumber(t) or t) end
        end
    end
    assert(file.save(), "failed to save `" .. file.name .. "'")
end

