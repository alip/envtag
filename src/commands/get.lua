#!/usr/bin/env lua
-- envtag get command
-- vim: set ft=lua et sts=4 sw=4 ts=4 fdm=marker:
-- Copyright 2009 Ali Polatel <polatel@gmail.com>
-- Distributed under the terms of the GNU General Public License v2

require "alt_getopt"
require "envutils"

local log = envutils.log
local logv = envutils.logv

local function usage()
    print"envtag get -- Get tags"
    print"Usage: envtag get [-hvnep] [-t TYPE] file..."
    print"Options:"
    print"\t-h, --help\t\tDisplay this help"
    print"\t-v, --verbose\t\tBe verbose"
    print"\t-t TYPE, --type=TYPE\tSpecify type"
    print"\t-n, --no-unicode\tDon't output unicode"
    print"\t-e, --export\t\tPrepend tag lines with export"
    print"\t-p, --properties\tPrint audio properties as well"
    print"Types:"
    print"\tflac, mpc, mpeg, vorbis, oggflac, speex, trueaudio, wavpack"
end

local function escapeq(str)
    return string.gsub(str, "'", "'\\''")
end

local long_opts = {
    help = "h",
    verbose = "v",
    type = "t",
    export = "e",
    properties = "p",
}
long_opts["no-unicode"] = "n"

local autype
local unicode = true
local export = false
local getprop = false
local opts, optind, optarg = alt_getopt.get_ordered_opts(arg, "hvt:nep", long_opts)
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
    elseif "p" == opt then
        getprop = true
    end
end

if optind > #arg then
    log"no file given"
    RETVAL = 1
    return
end

for i=optind,#arg do
    logv("processing `" .. arg[i] .. "'")
    song, msg = envtag.Song(arg[i], autype, getprop)
    if not song then
        log("failed to open `" .. arg[i] .. "': " .. msg)
        RETVAL = 1
    else
        for _, tag in ipairs(envutils.TAGS_COMMON) do
            t = song:get(tag, unicode)
            if not t then
                print("unset " .. string.upper(tag))
            else
                print(string.format("%s%s='%s'", export and "export " or "", string.upper(tag), escapeq(t)))
            end
        end
        if getprop then
            for _, prop in ipairs(envutils.PROPERTIES) do
                p, msg = song:property(prop)
                if not p then
                    log("failed to get property `" .. prop .. "' from file `" .. arg[i] .. "': " .. msg)
                    break
                end
                if 0 == p then
                    print("unset " .. string.upper(prop))
                else
                    print(string.format("%s%s=%s", export and "export " or "", string.upper(prop), p))
                end
            end
        end
    end
end

