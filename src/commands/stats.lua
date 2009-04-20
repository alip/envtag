#!/usr/bin/env lua
-- envtag stats command
-- vim: set ft=lua et sts=4 sw=4 ts=4 fdm=marker:
-- Copyright 2009 Ali Polatel <polatel@gmail.com>
-- Distributed under the terms of the GNU General Public License v2

require "alt_getopt"
require "envutils"

local log = envutils.log
local logv = envutils.logv

local function top(list, count)
    local best = {}
    for i=1,count do
        local elem = {count = 0}
        for key, count in pairs(list) do
            if count > elem.count then
                elem = {key = key, count = count}
            end
        end
        if elem.key then
            list[elem.key] = nil
            table.insert(best, elem)
        end
    end
    return best
end

local function usage()
    print"envtag stats -- Show statistics"
    print"Usage: envtag stats [-hvn] [-t TYPE] file..."
    print"Options:"
    print"\t-h, --help\t\tDisplay this help"
    print"\t-v, --verbose\t\tBe verbose"
    print"\t-t TYPE, --type=TYPE\tSpecify type"
    print"\t-n, --no-unicode\tOperate on Latin1 strings"
    print"\t-p, --properties\tShow statistics about properties as well"
    print"\t-T COUNT, --top=COUNT\tShow top COUNT artists and genres (default is 5)"
    print"Types:"
    print"\tflac, mpc, mpeg, vorbis, oggflac, speex, trueaudio, wavpack"
end

local long_opts = {
    help = "h",
    verbose = "v",
    type = "t",
    properties = "p",
    top = "T",
}
long_opts["no-unicode"] = "n"

local autype
local unicode = true
local readprops = false
local topcount = 5
local opts, optind, optarg = alt_getopt.get_ordered_opts(arg, "hvt:npT:", long_opts)
for index, opt in ipairs(opts) do
    if "h" == opt then
        usage()
        return
    elseif "v" == opt then
        envutils.log_verbose = true
    elseif "t" == opt then
        if optarg[index] then
            autype = optarg[index]
        else
            log"-t option requires an argument"
            RETVAL = 1
            return
        end
    elseif "n" == opt then
        unicode = false
    elseif "p" == opt then
        readprops = true
    elseif "T" == opt then
        if optarg[index] then
            topcount = assert(tonumber(optarg[index]), "count not a number")
        else
            log"-T option requires an argument"
            RETVAL = 1
            return
        end
    end
end

if optind > #arg then
    log"no file given"
    RETVAL=1
    return
end

local length = 0
local album_list = {}
local artist_list = {}
local genre_list = {}
local nalbum = 0
local nartist = 0
local ngenre = 0
local nsong = #arg - optind + 1

for i=optind,#arg do
    logv("processing %d of %d: `%s'", i - optind + 1, #arg - optind + 1, arg[i])
    local song, msg = envtag.Song(arg[i], autype, readprops)
    if not song then
        log("failed to open `%s': %s", arg[i], msg)
        RETVAL=1
    else
        local album, msg = song:get("album", unicode)
        if not album then
            log("failed to get album from `%s': %s", arg[i], msg)
            RETVAL=1
        elseif album ~= 0 then
            if album_list[album] then
                album_list[album] = album_list[album] + 1
            else
                album_list[album] = 1
                nalbum = nalbum + 1
            end
        end

        local artist, msg = song:get("artist", unicode)
        if not artist then
            log("failed to get artist from `%s': %s", arg[i], msg)
            RETVAL=1
        elseif artist ~= 0 then
            if artist_list[artist] then
                artist_list[artist] = artist_list[artist] + 1
            else
                artist_list[artist] = 1
                nartist = nartist + 1
            end
        end

        local genre, msg = song:get("genre", unicode)
        if not genre then
            log("failed to get genre from `%s': %s", arg[i], msg)
            RETVAL=1
        elseif genre ~= 0 then
            if genre_list[genre] then
                genre_list[genre] = genre_list[genre] + 1
            else
                genre_list[genre] = 1
                ngenre = ngenre + 1
            end
        end

        length = length + (song:property("length") or 0)

        song:close()
    end
end

print("Albums: " .. nalbum)
print("Artists: " .. nartist)
print("Genres: " .. ngenre)
print("Songs: " .. nsong)
if readprops then
    print("Total play time: " .. envutils.pretty_time(length))
end

top_artists = top(artist_list, topcount)
for i=1,#top_artists do
    if i == 1 then print"\nTop artists:" end
    local artist = top_artists[i]
    print("\t" .. i .. ". " .. artist.key .. " (count: " .. artist.count .. ")")
end
top_genres = top(genre_list, topcount)
for i=1,#top_genres do
    if i == 1 then print"\nTop genres:" end
    local genre = top_genres[i]
    print("\t" .. i .. ". " .. genre.key .. " (count: " .. genre.count .. ")")
end

