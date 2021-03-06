#!/usr/bin/env lua
-- envtag utility functions
-- vim: set ft=lua et sts=4 sw=4 ts=4 fdm=marker:
-- Copyright 2009 Ali Polatel <alip@exherbo.org>
-- Distributed under the terms of the GNU General Public License v2

local unpack = unpack

local string = string
local table = table
local io = io
local os = os

module "envutils"

log_name = "envtag"
log_timestamp = "%s"
log_verbose = false

function log(fmt, ...)
    io.stderr:write(log_name .. "@" .. os.date(log_timestamp) .. ": ")
    io.stderr:write(string.format(fmt, unpack(arg)))
    io.stderr:write"\n"
end

function logv(fmt, ...)
    if log_verbose then log(fmt, unpack(arg)) end
end

function escapeq(src)
    return string.gsub(src, "'", "'\\''")
end

function split(src, char)
    local chunk = ""
    local list = {}
    local i = 1

    while i <= #src do
        local c = string.sub(src, i, i)
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

TAGS_COMMON = {
    "title", "artist", "album", "comment",
    "genre", "year", "track"
}
TAGS_XIPH = {
    "title", "version", "album", "artist",
    "performer", "copyright", "organization", "description",
    "genre", "date", "location", "contact", "isrc"
}
PROPERTIES = {"length", "bitrate", "samplerate", "channels"}
