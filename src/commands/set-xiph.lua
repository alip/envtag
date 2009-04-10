#!/usr/bin/env lua
-- envtag set-xiph command
-- vim: set ft=lua et sts=4 sw=4 ts=4 fdm=marker:
-- Copyright 2009 Ali Polatel <polatel@gmail.com>
-- Distributed under the terms of the GNU General Public License v2

require "alt_getopt"
require "envutils"

local log = envutils.log
local logv = envutils.logv
local split = envutils.split

local function usage()
    print"envtag set-xiph -- Set Xiph comments"
    print"Usage: envtag set-xiph [-hvan] [-t TYPE] [-d DELIM] file..."
    print"Options:"
    print"\t-h, --help\t\tDisplay this help"
    print"\t-v, --verbose\t\tBe verbose"
    print"\t-t TYPE, --type=TYPE\tSpecify type"
    print"\t-d DELIM, --delimiter=DELIM\n\t\t\t\tSpecify delimiter for multiple tags (default is '=')"
    print"\t-a, --append\tAppend tags instead of replacing"
    print"\t-n, --no-unicode\tOperate on Latin1 strings"
    print"Types:"
    print"\tflac, mpc, mpeg, vorbis, oggflac, speex, trueaudio, wavpack"
    print"Environment variables:"
    print"\tTITLE, VERSION, ALBUM, ARTIST, PERFORMER, COPYRIGHT, ORGANIZATION,"
    print"\tDESCRIPTION, GENRE, DATE, LOCATION, CONTACT, ISRC"
end

local long_opts = {
    help = "h",
    verbose = "v",
    type = "t",
    append = "a",
    delimiter = "d",
}
long_opts["no-unicode"] = "n"

local autype
local append = false
local unicode = true
local delim = '='
local opts, optind, optarg = alt_getopt.get_ordered_opts(arg, "hvt:ad:n", long_opts)
for index, opt in ipairs(opts) do
    if "h" == opt then
        usage()
        return 0
    elseif "v" == opt then
        envutils.log_verbose = true
    elseif "t" == opt then
        if optarg[index] then
            autype = optarg[index]
        else
            log("-t option requires an argument")
            return 1
        end
    elseif "a" == opt then
        append = true
    elseif "d" == opt then
        if optarg[index] then
            delim = optarg[index]
            if 1 ~= delim then
                log("delimiter must be a single character")
                RETVAL = 1
                return
            end
        else
            log("-d option requires an argument")
            RETVAL = 1
            return
        end
    elseif "n" == opt then
        unicode = false
    end
end

if optind > #arg then
    log"no file given"
    RETVAL=1
    return
end

taglist = {}
for _, tag in ipairs(envutils.TAGS_COMMON) do
    taglist[tag] = os.getenv(string.upper(tag))
    if tag == "year" or tag == "track" then
        taglist[tag] = taglist[tag] == "" and 0 or tonumber(taglist[tag])
    end
end

for i=optind,#arg do
    logv("processing `" .. arg[i] .. "'")
    song, msg = envtag.Song(arg[i], autype, false)
    if not song then
        log("failed to open `" .. arg[i] .. "': " .. msg)
        RETVAL=1
    elseif not song:has_xiph() then
        log("file `" .. arg[i] .. "' has no xiph comments")
        RETVAL = 1
    else
        for _, tag in ipairs(envutils.TAGS_XIPH) do
            local t = os.getenv(string.upper(tag))
            if t then
                song:set_xiph(tag, split(t, delim), append, unicode)
            end
        end
        if not song:save() then
            log("failed to save `" .. arg[i] .. "'")
            RETVAL=1
        end
    end
end

