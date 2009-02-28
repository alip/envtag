#!/bin/zsh
# vim: set sw=4 et sts=4 tw=80 :
# Copyright 2009 Ali Polatel <polatel@gmail.com>
# Distributed under the terms of the GNU General Public License v2

# Find audio files that don't have a title tag
MUSIC_DIR="${HOME}"/mm/müzik

setopt extended_glob
autoload -U zargs

function tfilter() {
    for file in "$@"; do
        unset TITLE
        eval `envtag "$file"`
        [[ -z "${TITLE}" ]] && echo "$file"
    done
}

zargs (#iq)"${MUSIC_DIR}"/**/*.(flac|mp3|ogg|spx|tta) -- tfilter
