#!/usr/bin/env lua
-- envtag script to set tags
-- vim: set ft=lua et sts=4 sw=4 ts=4 fdm=marker:
-- Copyright 2009 Ali Polatel <polatel@gmail.com>
-- Distributed under the terms of the GNU General Public License v2

do
    local getenv = os.getenv

    local title = getenv"TITLE"
    if title then tag.set("title", title) end

    local artist = getenv"ARTIST"
    if artist then tag.set("artist", artist) end

    local album = getenv"ALBUM"
    if album then tag.set("album", album) end

    local comment = getenv"COMMENT"
    if comment then tag.set("comment", comment) end

    local genre = getenv"GENRE"
    if genre then tag.set("genre", genre) end

    local year = getenv"YEAR"
    if year then tag.set("year", tonumber(year)) end

    local track = getenv"TRACK"
    if track then tag.set("track", tonumber(track)) end

    assert(file.save(), "failed to save `" .. file.name .. "'")
end
