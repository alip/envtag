/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Ali Polatel <polatel@gmail.com>
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

#include <iostream>

#include <cstdlib>
#include <cstring>
#include <getopt.h>
#include <sysexits.h>

#include <fileref.h>
#include <tag.h>

#include "defs.hh"
#include "util.hh"

#ifdef ENABLE_MAGIC
#include "mime.hh"
#endif // ENABLE_MAGIC

#ifdef ENABLE_LUA
#include "script.hh"
#endif //ENABLE_LUA

using std::cerr;
using std::cout;
using std::endl;

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

static struct id3v2_encoding_table {
    int no;
    const char *name;
} id3v2_encodings[] = {
    {TagLib_ID3v2_Latin1,   "latin1"},
    {TagLib_ID3v2_UTF16,    "utf16"},
    {TagLib_ID3v2_UTF16BE,  "utf16be"},
    {TagLib_ID3v2_UTF8,     "utf8"},
    {-1,                    NULL}
};

void about(void) {
    cerr << PACKAGE"-"VERSION;
#if defined(GITHEAD)
    if (0 != strlen(GITHEAD))
        cerr << "-"GITHEAD;
#endif
    cerr << endl;
}

void usage(void) {
    cerr << PACKAGE"-"VERSION;
#if defined(GITHEAD)
    if (0 != strlen(GITHEAD))
        cerr << "-"GITHEAD;
#endif
    cerr << " simple audio tagger for use in shell scripts\n";
    cerr << "Usage: "PACKAGE" [-t type] [-e encoding] [-s|-S FILE] [-hVvpnE] file...\n\n";
    cerr << "Options:\n";
    cerr << "\t-h, --help\t\tYou're looking at it :)\n";
    cerr << "\t-V, --version\t\tShow version information\n";
    cerr << "\t-v, --verbose\t\tBe verbose\n";
    cerr << "\t-s, --set\t\tSet tags\n";
    cerr << "\t-S FILE, --script=FILE\tExecute script FILE on tags (use - for stdin)\n";
    cerr << "\t-p, --print\t\tWhen used with -s or -S, prints tags after the action\n";
    cerr << "\t-t TYPE, --type=TYPE\tSpecify type\n";
    cerr << "\t-n, --no-unicode\tOperate on Latin1 strings when setting tags\n";
    cerr << "\t-e ENC, --encoding=ENC\tSpecify ID3v2 encoding\n";
    cerr << "\t-E, --export\t\tPrepend tag lines with export\n";
    cerr << "\nTypes:\n";
    cerr << "\tmpeg, vorbis, flac, mpc, oggflac, wavpack, speex, trueaudio\n";
    cerr << "\tIf no type is given, "PACKAGE" tries to determine it using filename\n";
    cerr << "\nPossible ID3v2 encoding values:\n";
    cerr << "\tlatin1, utf16, utf16be, utf8 (default)\n";
    cerr << "\nEnvironment variables used in set mode:\n";
    cerr << "\tTITLE, ARTIST, ALBUM, COMMENT, GENRE, YEAR, TRACK\n";
    cerr << "\nEnvironment variables used by scripter:\n";
    cerr << "\t"ENV_INIT": Run initialization code or load from file similar to LUA_INIT\n";
    cerr << "\nPaths:\n";
    cerr << "\t~"SCRIPT_SEARCH_DIR": Search directory for scripts\n";
}

int main(int argc, char **argv)
{
    int optc;
    bool verbose = false;
    bool export_vars = false;
    bool set_tags = false;
    bool print_tags = false;
    bool unicode = true;
    int type = -1;
    bool encoding_set = false;
    char *script = NULL;
    static struct option long_options[] = {
        {"version",    no_argument,        NULL, 'V'},
        {"help",       no_argument,        NULL, 'h'},
        {"verbose",    no_argument,        NULL, 'v'},
        {"export",     no_argument,        NULL, 'E'},
        {"set",        no_argument,        NULL, 's'},
        {"print",      no_argument,        NULL, 'p'},
        {"no-unicode", no_argument,        NULL, 'n'},
        {"type",       required_argument,  NULL, 't'},
        {"encoding",   required_argument,  NULL, 'e'},
        {"script",     required_argument,  NULL, 'S'},
        {0, 0, NULL, 0}
    };

    while (-1 != (optc = getopt_long(argc, argv, "VhvEspnt:e:S:", long_options, NULL))) {
        switch (optc) {
            case 'h':
                usage();
                return EXIT_SUCCESS;
            case 'V':
                about();
                return EXIT_SUCCESS;
            case 'v':
                verbose = true;
                break;
            case 'E':
                export_vars = true;
                break;
            case 's':
                set_tags = true;
                break;
            case 'p':
                print_tags = true;
                break;
            case 'n':
                unicode = false;
            case 't':
                for (int i = 0; types[i].name != NULL; i++) {
                    if (0 == strncmp(optarg, types[i].name, strlen(types[i].name) + 1))
                        type = types[i].no;
                }
                if (-1 == type) {
                    cerr << PACKAGE": unknown type `" << optarg << "'" << endl;
                    return EX_USAGE;
                }
                break;
            case 'e':
                for (int i = 0; id3v2_encodings[i].name != NULL; i++) {
                    if (0 == strncmp(optarg, id3v2_encodings[i].name, strlen(id3v2_encodings[i].name) + 1))
                    {
                        encoding_set = true;
                        setID3v2DefaultEncoding(id3v2_encodings[i].no);
                        break;
                    }
                }
                if (!encoding_set) {
                    cerr << PACKAGE": unknown encoding `" << optarg << "'" << endl;
                    return EX_USAGE;
                }
                break;
            case 'S':
#ifdef ENABLE_LUA
                script = optarg;
                break;
#else
                cerr << PACKAGE": -S/--script requires Lua support enabled" << endl;
                return EX_USAGE;
#endif
            case '?':
            default:
                cerr << "try "PACKAGE" --help for more information" << endl;
                return EXIT_FAILURE;
        }
    }
    if (set_tags && script) {
        cerr << PACKAGE": options -s and -S are mutually exclusive" << endl;
        return EX_USAGE;
    }
    if (argc < optind + 1) {
        cerr << PACKAGE": no file given" << endl;
        return EXIT_FAILURE;
    }
    else {
        argc -= (optind - 1);
        argv += (optind - 1);
    }

    // Make sure the default encoding is UTF8
    if (!encoding_set)
        setID3v2DefaultEncoding(-1);

#ifdef ENABLE_MAGIC
    TagLib::FileRef::addFileTypeResolver(new MagicFileTypeResolver);
#endif

#ifdef ENABLE_LUA
    lua_State *lstate = NULL;
    if (script)
        lstate = init_lua();
#endif

    int ret = EXIT_SUCCESS;
    TagLib::FileRef *f = 0;
    for (int i = 1; i < argc; i++) {
        if (f) delete f;
        f = openFile(argv[i], type);
        if (f && !f->isNull()) {
            if (set_tags) {
                if (verbose)
                    cerr << PACKAGE": setting tags of `" << argv[i] << "'" << endl;
                if (!setTags(*f, unicode)) {
                    ret = EXIT_FAILURE;
                    cerr << PACKAGE": failed to write tags to `" << argv[i] << "'" << endl;
                    continue;
                }
                if (!print_tags)
                    continue;
            }
#ifdef ENABLE_LUA
            else if (script) {
                doscript(script, lstate, f, argv[i], verbose, i, argc - 1);
                if (!print_tags)
                    continue;
            }
#endif
            dumpTags(*f, export_vars);
            dumpProperties(*f, export_vars);
        }
        else {
            ret = EXIT_FAILURE;
            cerr << PACKAGE": unknown file type `" << argv[i] << "'" << endl;
        }
    }
    return ret;
}
