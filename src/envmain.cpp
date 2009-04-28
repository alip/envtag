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

#include <cstdlib>
#include <cstring>
#include <iostream>

#include <fileref.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "envlua.hh"
#ifdef ENABLE_MAGIC
#include "envmime.hh"
#endif

using namespace std;

void about(void) {
    cerr << PACKAGE"-"VERSION;
#if defined(GITHEAD)
    if (0 != strlen(GITHEAD))
        cerr << "-"GITHEAD;
#endif
    cerr << endl;
}

void usage(void) {
    cerr << PACKAGE" -- Simple audio tagger" << endl;
    cerr << "Usage: "PACKAGE" COMMAND [ARGS]" << endl << endl;
    cerr << "Commands:" << endl;
    cerr << "\tget\t\tGet tags" << endl;
    cerr << "\tset\t\tSet tags" << endl;
    cerr << "\tprop\t\tGet audio properties" << endl;
    cerr << "\tget-xiph\tGet Xiph comments" << endl;
    cerr << "\tset-xiph\tSet Xiph comments" << endl;
    cerr << "\tstats\t\tShow statistics" << endl;
    cerr << "See 'envtag COMMAND -h' for more information on a specific command." << endl << endl;
    cerr << "Environment variables:" << endl;
    cerr << "\t"ENV_INIT":\tRun initialization code or load from file simiar to LUA_INIT" << endl << endl;
    cerr << "Directories:" << endl;
    cerr << "\t~"EXTRA_COMMAND_DIR":\tDirectory for extra commands" << endl;
}

int main(int argc, char **argv)
{
    int ret;
    lua_State *L;

    if (2 > argc) {
        usage();
        return EXIT_FAILURE;
    }
    else if (0 == strncmp(argv[1], "-h", 3) || 0 == strncmp(argv[1], "--help", 7)) {
        usage();
        return EXIT_SUCCESS;
    }
    else if (0 == strncmp(argv[1], "-V", 3) || 0 == strncmp(argv[1], "--version", 10)) {
        about();
        return EXIT_SUCCESS;
    }
    else {
        --argc;
        ++argv;
    }

#ifdef ENABLE_MAGIC
    TagLib::FileRef::addFileTypeResolver(new MagicFileTypeResolver);
#endif

    L = init_lua();
    ret = docommand(L, argc, argv);
    close_lua(L);
    return ret;
}
