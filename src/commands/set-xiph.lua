#!/usr/bin/env lua
-- envtag set-xiph command
-- vim: set ft=lua et sts=4 sw=4 ts=4 fdm=marker:
-- Copyright 2009 Ali Polatel <alip@exherbo.org>
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
    print"\t-a, --append\t\tAppend tags instead of replacing"
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
        return
    elseif "v" == opt then
        envutils.log_verbose = true
    elseif "t" == opt then
        if optarg[index] then
            autype = optarg[index]
        else
            log"-t option requires an argument"
            envtag.exit_code = envtag.EX_USAGE
            return
        end
    elseif "a" == opt then
        append = true
    elseif "d" == opt then
        if optarg[index] then
            delim = optarg[index]
            if 1 ~= #delim then
                log"delimiter must be a single character"
                envtag.exit_code = envtag.EX_USAGE
                return
            end
        else
            log"-d option requires an argument"
            envtag.exit_code = envtag.EX_USAGE
            return
        end
    elseif "n" == opt then
        unicode = false
    end
end

if optind > #arg then
    log"no file given"
    envtag.exit_code = envtag.EX_NOINPUT
    return
end

local taglist = {}
for _, tag in ipairs(envutils.TAGS_XIPH) do
    taglist[tag] = os.getenv(string.upper(tag))
end

for i=optind,#arg do
    logv("processing %d of %d: `%s'", i - optind + 1, #arg - optind + 1, arg[i])
    song, msg = envtag.Song(arg[i], autype, false)
    if not song then
        log("failed to open `%s': %s", arg[i], msg)
        envtag.exit_code = envtag.EX_DATAERR
    else
        for _, tag in ipairs(envutils.TAGS_XIPH) do
            local t = os.getenv(string.upper(tag))
            if t then
                song:set_xiph(tag, split(t, delim), append, unicode)
            end
        end
        if not song:save() then
            log("failed to save `%s'", arg[i])
            envtag.exit_code = envtag.EX_IOERR
        end
        song:close()
    end
end

