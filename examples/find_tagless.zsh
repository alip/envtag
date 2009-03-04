#!/bin/zsh
# vim: set sw=4 et sts=4 tw=80 :
# Copyright 2009 Ali Polatel <polatel@gmail.com>
# Distributed under the terms of the GNU General Public License v2

# Find audio files that have missing tags
MUSIC_DIR="${HOME}"/mm/müzik

setopt extended_glob
autoload -U zargs

function tfilter() {
    for file in "$@"; do
        unset ARTIST TITLE ALBUM GENRE
        eval `envtag "$file"`
        [[ -z "${ARTIST}" ]] && echo "ARTIST $file" && continue
        [[ -z "${TITLE}" ]] && echo "TITLE $file" && continue
        [[ -z "${ALBUM}" ]] && echo "ALBUM $file" && continue
        [[ -z "${GENRE}" ]] && echo "GENRE $file" && continue
    done
}

zargs (#iq)"${MUSIC_DIR}"/**/*.(fla|flac|mp3|oga|ogg|spx|tta) -- tfilter
