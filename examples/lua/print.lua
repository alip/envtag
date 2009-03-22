#!/usr/bin/env lua
-- envtag lua scripting example
-- vim: set ft=lua et sts=4 sw=4 ts=4 fdm=marker:
-- Copyright 2009 Ali Polatel <polatel@gmail.com>
-- Distributed under the terms of the GNU General Public License v2

function lprint(...)
    print("lua>", unpack(arg))
end

-- The filename is in the global FILE variable
lprint("Processing " .. FILE)
-- The global VERBOSE variable is set to true if envtag was invoked with -v
-- You can use this to make your scripts verbose as needed.
if VERBOSE then lprint("verbose")
else lprint("not verbose") end
-- The global COUNT variable gives the count of current processed file
-- The global TOTAL variables gives the total number of files that will be
-- processed
lprint("count: " .. COUNT .. " of " .. TOTAL)

-- tag.get() function is used to retrieve tags.
-- It returns the value as string or integer depending on tag.
-- On error it returns nil and error message
title = assert(tag.get"title")
artist = assert(tag.get"artist")
album = assert(tag.get"album")
comment = assert(tag.get"comment")
genre = assert(tag.get"genre")
year = assert(tag.get"year")
track = assert(tag.get"track")

lprint("title: '" .. title .. "'")
lprint("artist: '" .. artist .. "'")
lprint("album: '" .. album .. "'")
lprint("comment: '" .. comment .. "'")
lprint("genre: '" .. genre .. "'")
lprint("year: " .. year)
lprint("track: " .. track)

-- tag.set() function is used to set tags.
-- It returns nil.
-- if string.match(artist, "[Dd]e+p [Pp]urple") then
--    tag.set("artist", "Deep Purple")
-- end

-- prop.get() function can be used to retrieve audio properties
length = prop.get"length"
bitrate = prop.get"bitrate"
samplerate = prop.get"samplerate"
channels = prop.get"channels"

lprint("length: " .. length)
lprint("bitrate: " .. bitrate)
lprint("samplerate: " .. samplerate)
lprint("channels: " .. channels)
