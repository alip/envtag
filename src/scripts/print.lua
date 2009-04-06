#!/usr/bin/env lua
-- envtag script to print tags
-- vim: set ft=lua et sts=4 sw=4 ts=4 fdm=marker:
-- Copyright 2009 Ali Polatel <polatel@gmail.com>
-- Distributed under the terms of the GNU General Public License v2

do
    local gsub = string.gsub
    local format = string.format
    local upper = string.upper
    local concat = table.concat

    local function escapeq(str)
        return gsub(str, "'", "'\\''")
    end

    local prefix = opt.export and "export " or ""

    if tag.has_xiph() then
        -- Xiph Comments
        xiph_tags = {"title", "version", "album", "artist",
                     "performer", "copyright", "organization", "description",
                     "genre", "date", "location", "contact", "isrc"}
        for _, tname in ipairs(xiph_tags) do
            local t = tag.get_xiph(tname, opt.unicode)
            if t then
                print(format("%s%s='%s'", prefix, upper(tname), escapeq(concat(t, opt.delimiter))))
            else
                print("unset " .. upper(tname))
            end
        end
    else
        -- Common Tags
        common_tags = {"title", "artist", "album", "comment",
                       "genre", "year", "track"}
        for _, tname in ipairs(common_tags) do
            local t = tag.get(tname, opt.unicode)
            if t then
                print(format("%s%s='%s'", prefix, upper(t), escapeq(t)))
            else
                print("unset " .. upper(t))
            end
        end
    end

    -- Audio properties
    if opt.read_props then
        local length = prop.get"length" or 0
        local bitrate = prop.get"bitrate" or 0
        local samplerate = prop.get"samplerate" or 0
        local channels = prop.get"channels" or 0

        print""
        print(format("%sLENGTH=%d", prefix, length))
        print(format("%sBITRATE=%d", prefix, bitrate))
        print(format("%sSAMPLERATE=%d", prefix, samplerate))
        print(format("%sCHANNELS=%d", prefix, channels))
    end
end

