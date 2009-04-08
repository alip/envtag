#!/usr/bin/env lua
-- envtag script to print tags
-- vim: set ft=lua et sts=4 sw=4 ts=4 fdm=marker:
-- Copyright 2009 Ali Polatel <polatel@gmail.com>
-- Distributed under the terms of the GNU General Public License v2

require "envtag"

do
    local format = string.format
    local upper = string.upper
    local concat = table.concat

    local escapeq = envtag.escapeq
    local prefix = opt.export and "export " or ""

    if tag.has_xiph() then
        -- Xiph Comments
        for _, tname in ipairs(envtag.TAGS_XIPH) do
            local t = tag.get_xiph(tname, opt.unicode)
            if t then
                print(format("%s%s='%s'", prefix, upper(tname), escapeq(concat(t, opt.delimiter))))
            else
                print("unset " .. upper(tname))
            end
        end
    else
        -- Common Tags
        for _, tname in ipairs(envtag.TAGS_COMMON) do
            local t = tag.get(tname, opt.unicode)
            if t then
                print(format("%s%s='%s'", prefix, upper(tname), escapeq(t)))
            else
                print("unset " .. upper(tname))
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

