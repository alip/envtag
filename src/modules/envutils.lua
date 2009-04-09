#!/usr/bin/env lua
-- envtag utility functions
-- vim: set ft=lua et sts=4 sw=4 ts=4 fdm=marker:
-- Copyright 2009 Ali Polatel <polatel@gmail.com>
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

function log(...)
    io.stderr:write(log_name .. "@" .. os.date(log_timestamp) .. ": " .. table.concat(arg) .. "\n")
end

function logv(...)
    if log_verbose then log(unpack(arg)) end
end

function escapeq(src)
    return string.gsub(src, "'", "'\\''")
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

