#!/usr/bin/env lua
-- envtag prop command
-- vim: set ft=lua et sts=4 sw=4 ts=4 fdm=marker:
-- Copyright 2009 Ali Polatel <polatel@gmail.com>
-- Distributed under the terms of the GNU General Public License v2

require "alt_getopt"
require "envutils"

local log = envutils.log
local logv = envutils.logv

local function usage()
    print"envtag prop -- Get audio properties"
    print"Usage: envtag prop [-hve] [-t TYPE] file..."
    print"\t-h, --help\t\tDisplay this help"
    print"\t-v, --verbose\t\tBe verbose"
    print"\t-t TYPE, --type=TYPE\tSpecify type"
    print"\t-e, --export\t\tPrepend lines with export"
    print"Types:"
    print"\tflac, mpc, mpeg, vorbis, oggflac, speex, trueaudio, wavpack"
end

local long_opts = {
    help = "h",
    verbose = "v",
    type = "t",
    export = "e",
}

local autype
local export = false
local opts, optind, optarg = alt_getopt.get_ordered_opts(arg, "hvt:e", long_opts)
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
    elseif "e" == opt then
        export = true
    end
end

if optind > #arg then
    log"no file given"
    RETVAL = 1
    return
end

for i=optind,#arg do
    logv("processing %d of %d: `%s'", i - optind + 1, #arg - optind + 1, arg[i])
    song, msg = envtag.Song(arg[i], autype, true)
    if not song then
        log("failed to open `%s': %s", arg[i], msg)
        RETVAL = 1
    else
        for _, prop in ipairs(envutils.PROPERTIES) do
            p, msg = song:property(prop)
            if not p then
                log("failed to get property `%s' from file `%s': %s", prop, arg[i], msg)
            --[[ elseif 0 == p then
                print("unset " .. string.upper(prop))
            --]]
            elseif 0 ~= p then
                print(string.format("%s%s=%s", export and "export " or "", string.upper(prop), p))
            end
        end
        song:close()
    end
end

