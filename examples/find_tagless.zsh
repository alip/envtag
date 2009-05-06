#!/usr/bin/env zsh
# vim: set sw=4 et sts=4 tw=80 :
# Copyright 2009 Ali Polatel <polatel@gmail.com>
# Distributed under the terms of the GNU General Public License v2

# Find audio files that have missing tags
if [[ 1 > $# ]]; then
    echo "usage: $(basename ${0}) /path/to/music/directory" >&2
    exit 1
elif [[ ! -d "${1}" ]]; then
    echo "Not a directory: ${1}" >&2
    exit 1
else
    dir="${1}"
fi

setopt extended_glob
autoload -U zargs

function tfilter() {
    for file in "$@"; do
        unset ARTIST TITLE ALBUM GENRE
        eval `envtag get "$file"`
        if [[ 0 != $? ]]; then
            echo "FAIL $file"
            continue
        fi
        [[ -z "${ARTIST}" ]] && echo "ARTIST $file" && continue
        [[ -z "${TITLE}" ]] && echo "TITLE $file" && continue
        [[ -z "${ALBUM}" ]] && echo "ALBUM $file" && continue
        [[ -z "${GENRE}" ]] && echo "GENRE $file" && continue
    done
}

zargs (#iq)"${dir}"/***/*.(flac|mp3|ogg|spx|tta) -- tfilter
