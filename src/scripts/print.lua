#!/usr/bin/env lua
-- envtag script to print tags
-- vim: set ft=lua et sts=4 sw=4 ts=4 fdm=marker:
-- Copyright 2009 Ali Polatel <polatel@gmail.com>
-- Distributed under the terms of the GNU General Public License v2

do
    local gsub = string.gsub
    local format = string.format

    local function escapeq(str)
        return gsub(str, "'", "'\\''")
    end

    local prefix = opt.export and "export " or ""

    -- Tags
    local title = tag.get("title", opt.unicode)
    title = title and escapeq(title) or ""
    local artist = tag.get("artist", opt.unicode)
    artist = artist and escapeq(artist) or ""
    local album = tag.get("album", opt.unicode)
    album = album and escapeq(album) or ""
    local comment = tag.get("comment", opt.unicode)
    comment = comment and escapeq(comment) or ""
    local genre = tag.get("genre", opt.unicode)
    genre = genre and escapeq(genre) or ""
    local year = tag.get"year" or 0
    local track = tag.get"track" or 0

    -- Audio properties
    local length = prop.get"length" or 0
    local bitrate = prop.get"bitrate" or 0
    local samplerate = prop.get"samplerate" or 0
    local channels = prop.get"channels" or 0

    -- Output
    print(format("%sTITLE='%s'", prefix, title))
    print(format("%sARTIST='%s'", prefix, artist))
    print(format("%sALBUM='%s'", prefix, album))
    print(format("%sCOMMENT='%s'", prefix, comment))
    print(format("%sGENRE='%s'", prefix, genre))
    print(format("%sYEAR=%d", prefix, year))
    print(format("%sTRACK=%d", prefix, track))
    if opt.read_props then
        print""
        print(format("%sLENGTH=%d", prefix, length))
        print(format("%sBITRATE=%d", prefix, bitrate))
        print(format("%sSAMPLERATE=%d", prefix, samplerate))
        print(format("%sCHANNELS=%d", prefix, channels))
    end
end

