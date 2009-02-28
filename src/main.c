/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Ali Polatel
 *
 * This file is part of the envtag audio tagger. envtag is free software;
 * you can redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2, as published by the Free Software Foundation.
 *
 * envtag is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <errno.h>
#include <getopt.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <time.h>

#include <taglib/tag_c.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

static struct type_table {
    int no;
    const char *name;
} types[] = {
    {TagLib_File_MPEG,      "mpeg"},
    {TagLib_File_OggVorbis, "vorbis"},
    {TagLib_File_FLAC,      "flac"},
    {TagLib_File_MPC,       "mpc"},
    {TagLib_File_OggFlac,   "oggflac"},
    {TagLib_File_WavPack,   "wavpack"},
    {TagLib_File_Speex,     "speex"},
    {TagLib_File_TrueAudio, "trueaudio"},
    {-1,                    NULL}
};

// Prototypes
void __lg(const char *func, size_t len, const char *fmt, ...)
    __attribute__ ((format (printf, 3, 4)));

void about(void) {
    fprintf(stderr, PACKAGE"-"VERSION);
#ifdef GITHEAD
    if (0 != strlen(GITHEAD))
        fprintf(stderr, "-"GITHEAD);
#endif
    fputc('\n', stderr);
}

void usage(void) {
    fprintf(stderr, PACKAGE"-"VERSION);
#ifdef GITHEAD
    if (0 != strlen(GITHEAD))
        fprintf(stderr, "-"GITHEAD);
#endif
    fprintf(stderr, " simple audio tagger for use in shell scripts\n");
    fprintf(stderr, "Usage: "PACKAGE" [-t type] [-svhV] file...\n\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "\t-h, --help\t\tYou're looking at it :)\n");
    fprintf(stderr, "\t-V, --version\t\tShow version information\n");
    fprintf(stderr, "\t-t TYPE --type=TYPE\tSpecify type\n");
    fprintf(stderr, "\t-s, --set\t\tSet tags\n");
    fprintf(stderr, "\t-v, --verbose\t\tBe verbose\n");
    fprintf(stderr, "\nTypes:\n");
    fprintf(stderr, "\tmpeg, vorbis, flac, mpc, oggflac, wavpack, speex, trueaudio\n");
    fprintf(stderr, "\tIf no type is given, "PACKAGE" tries to determine it using filename\n");
    fprintf(stderr, "\nEnvironment variables used in set mode:\n");
    fprintf(stderr, "\tTITLE, ARTIST, ALBUM, COMMENT, GENRE, YEAR, TRACK\n");
}

void __lg(const char *func, size_t len, const char *fmt, ...) {
    va_list args;

    fprintf(stderr, PACKAGE"@%ld: [%s():%zu] ", time(NULL), func, len);
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fputc('\n', stderr);
}
#define lg(...)     __lg(__func__, __LINE__, __VA_ARGS__)

void *xrealloc(void *ptr, size_t size) {
    void *t;

    t = realloc(ptr, size);
    if (NULL == t) {
        lg("realloc failed: %s", strerror(errno));
        abort();
    }

    return t;
}

char *escape_quotes(const char *src) {
    char *escaped_str = NULL;

    for (int i = 0, j = 0; i <= strlen(src); i++) {
        escaped_str = xrealloc(escaped_str, (j + 1) * sizeof(char));
        if ('\'' == src[i]) {
            escaped_str[j++] = '\\';
            escaped_str = xrealloc(escaped_str, (j + 1) * sizeof(char));
            escaped_str[j++] = '\'';
        }
        else
            escaped_str[j++] = src[i];
    }
    return escaped_str;
}

void tset(TagLib_File *fp) {
    static char *env_title, *env_artist, *env_album, *env_comment;
    static char *env_genre, *env_year, *env_track;

    env_title = getenv("TITLE");
    env_artist = getenv("ARTIST");
    env_album = getenv("ALBUM");
    env_comment = getenv("COMMENT");
    env_genre = getenv("GENRE");
    env_year = getenv("YEAR");
    env_track = getenv("TRACK");

    TagLib_Tag *tags = taglib_file_tag(fp);

    if (NULL != env_title)
        taglib_tag_set_title(tags, env_title);
    if (NULL != env_artist)
        taglib_tag_set_artist(tags, env_artist);
    if (NULL != env_album)
        taglib_tag_set_album(tags, env_album);
    if (NULL != env_comment)
        taglib_tag_set_comment(tags, env_comment);
    if (NULL != env_genre)
        taglib_tag_set_genre(tags, env_genre);
    if (NULL != env_year)
        taglib_tag_set_year(tags, (unsigned int) atoi(env_year));
    if (NULL != env_track)
        taglib_tag_set_track(tags, (unsigned int) atoi(env_track));

    taglib_file_save(fp);
    taglib_tag_free_strings();
}

void tprint(TagLib_File *fp) {
    // Tags
    const TagLib_Tag *tags = taglib_file_tag(fp);
    char *title = escape_quotes(taglib_tag_title(tags));
    char *artist = escape_quotes(taglib_tag_artist(tags));
    char *album = escape_quotes(taglib_tag_album(tags));
    char *comment = escape_quotes(taglib_tag_comment(tags));
    char *genre = escape_quotes(taglib_tag_genre(tags));
    printf("TITLE='%s'\n", title);
    printf("ARTIST='%s'\n", artist);
    printf("ALBUM='%s'\n", album);
    printf("COMMENT='%s'\n", comment);
    printf("GENRE='%s'\n", genre);
    printf("YEAR=%d\n", taglib_tag_year(tags));
    printf("TRACK=%d\n", taglib_tag_track(tags));

    // Cleanup
    taglib_tag_free_strings();
    free(title);
    free(artist);
    free(album);
    free(comment);
    free(genre);

    // Audio properties
    const TagLib_AudioProperties *props = taglib_file_audioproperties(fp);
    printf("LENGTH=%d\n", taglib_audioproperties_length(props));
    printf("BITRATE=%d\n", taglib_audioproperties_bitrate(props));
    printf("SAMPLERATE=%d\n", taglib_audioproperties_samplerate(props));
    printf("CHANNELS=%d\n", taglib_audioproperties_channels(props));

}

int main(int argc, char **argv) {
    TagLib_File *fp;

    // Parse command line
    int optc;
    int verbose = 0;
    int set = 0;
    int type = -1;

    static struct option long_options[] = {
        {"version", no_argument,        NULL, 'V'},
        {"help",    no_argument,        NULL, 'h'},
        {"type",    required_argument,  NULL, 't'},
        {"set",     no_argument,        NULL, 's'},
        {"verbose", no_argument,        NULL, 'v'},
        {0, 0, NULL, 0}
    };

    while (-1 != (optc = getopt_long(argc, argv, "Vht:sv", long_options, NULL))) {
        switch (optc) {
            case 'h':
                usage();
                return EXIT_SUCCESS;
            case 'V':
                about();
                return EXIT_SUCCESS;
            case 's':
                set = 1;
                break;
            case 'v':
                verbose = 1;
                break;
            case 't':
                for (int i = 0; types[i].name != NULL; i++) {
                    if (0 == strncmp(optarg, types[i].name, strlen(types[i].name)))
                        type = types[i].no;
                }
                if (-1 == type) {
                    lg("Unknown type `%s'", optarg);
                    return EX_USAGE;
                }
                break;
            case '?':
            default:
                fprintf(stderr, "try "PACKAGE" --help for more information\n");
                return EXIT_FAILURE;
        }
    }
    if (argc < optind + 1) {
        lg("No file given");
        return EXIT_FAILURE;
    }
    else {
        argc -= (optind - 1);
        argv += (optind - 1);
    }

    for (int i = 1; i < argc; i++) {
        if (-1 == type)
            fp = taglib_file_new(argv[i]);
        else
            fp = taglib_file_new_type(argv[i], type);

        if (NULL == fp) {
            lg("Unknown file type: `%s'", argv[i]);
            continue;
        }
        else if (!taglib_file_is_valid(fp)) {
            lg("No valid tag information: `%s'", argv[i]);
            taglib_file_free(fp);
            continue;
        }
        if (set) {
            lg("Setting tags of `%s'", argv[i]);
            tset(fp);
            if (verbose)
                tprint(fp);
        }
        else
            tprint(fp);
        taglib_file_free(fp);
    }
    return EXIT_SUCCESS;
}
