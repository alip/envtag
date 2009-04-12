#!/usr/bin/env lua
-- envtag get-xiph command
-- vim: set ft=lua et sts=4 sw=4 ts=4 fdm=marker:
-- Copyright 2009 Ali Polatel <polatel@gmail.com>
-- Distributed under the terms of the GNU General Public License v2

require "alt_getopt"
require "envutils"

local log = envutils.log
local logv = envutils.logv
local escapeq = envutils.escapeq

local function usage()
    print"envtag get-xiph -- Get Xiph comments"
    print"Usage: envtag get-xiph [-hvne] [-t TYPE] [-d DELIM] file..."
    print"Options:"
    print"\t-h, --help\t\tDisplay this help"
    print"\t-v, --verbose\t\tBe verbose"
    print"\t-t TYPE, --type=TYPE\tSpecify type"
    print"\t-n, --no-unicode\tDon't output unicode"
    print"\t-e, --export\t\tPrepend lines with export"
    print"\t-d DELIM, --delimiter=DELIM\n\t\t\t\tSpecify delimiter for multiple tags (default is '=')"
    print"Types:"
    print"\tflac, mpc, mpeg, vorbis, oggflac, speex, trueaudio, wavpack"
end

local long_opts = {
    help = "h",
    verbose = "v",
    type = "t",
    export = "e",
    delimiter = 'd',
}
long_opts["no-unicode"] = "n"

local autype
local unicode = true
local export = false
local delim = '='
local opts, optind, optarg = alt_getopt.get_ordered_opts(arg, "hvt:ned:", long_opts)
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
            log("-t option requires an argument")
            RETVAL = 1
            return
        end
    elseif "n" == opt then
        unicode = false
    elseif "e" == opt then
        export = true
    elseif "d" == opt then
        if optarg[index] then
            delim = optarg[index]
            if 1 ~= delim then
                log"delimiter must be a single character"
                RETVAL = 1
                return
            end
        else
            log"-d option requires an argument"
            RETVAL = 1
            return
        end
    end
end

if optind > #arg then
    log"no file given"
    RETVAL = 1
    return
end

for i=optind,#arg do
    logv("processing %d of %d: `%s'", i - optind + 1, #arg - optind + 1, arg[i])
    song, msg = envtag.Song(arg[i], autype, false)
    if not song then
        log("failed to open `%s': %s", arg[i], msg)
        RETVAL = 1
    elseif not song:has_xiph() then
        log("file `%s' has no xiph comments", arg[i])
        RETVAL = 1
    else
        for _, tag in ipairs(envutils.TAGS_XIPH) do
            t, msg = song:get_xiph(tag, unicode)
            if not t then
                log("failed to get xiph comment `%s' from file `%s': %s", tag, arg[i], msg)
            elseif 0 == t then
                print("unset " .. string.upper(tag))
            else
                print(string.format("%s%s='%s'", export and "export " or "", string.upper(tag),
                    escapeq(table.concat(t, delim))))
            end
        end
        song:close()
    end
end

