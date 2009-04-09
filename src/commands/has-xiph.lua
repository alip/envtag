#!/usr/bin/env lua
-- envtag has-xiph command
-- vim: set ft=lua et sts=4 sw=4 ts=4 fdm=marker:
-- Copyright 2009 Ali Polatel <polatel@gmail.com>
-- Distributed under the terms of the GNU General Public License v2

require "alt_getopt"
require "envutils"

local log = envutils.log
local logv = envutils.logv

local function usage()
    print"envtag has-xiph -- Determine whether the file has Xiph comments"
    print"Usage: envtag has-xiph [-hv] [-t TYPE] file"
    print"\t-h, --help\t\tDisplay this help"
    print"\t-v, --verbose\t\tBe verbose"
    print"\t-t TYPE, --type=TYPE\tSpecify type"
    print"Types:"
    print"\tflac, mpc, mpeg, vorbis, oggflac, speex, trueaudio, wavpack"
end

local long_opts = {
    help = "h",
    verbose = "v",
    type = "t",
}

local autype
local opts, optind, optarg = alt_getopt.get_ordered_opts(arg, "hvt:", long_opts)
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
    end
end

if optind > #arg then
    log"no file given"
    RETVAL = 1
    return
end

song, msg = envtag.Song(arg[optind], autype, false)
if not song then
    log("failed to open `" .. arg[optind] .. "': " .. msg)
    RETVAL = 1
elseif not song:has_xiph() then
    RETVAL = 1
end

