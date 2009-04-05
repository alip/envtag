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
#include <typeinfo>

#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <unistd.h>

#include <lua.hpp>
#include <fileref.h>
#include <tag.h>

#include <flacfile.h>
#include <mpcfile.h>
#include <mpegfile.h>
#include <oggflacfile.h>
#include <speexfile.h>
#include <vorbisfile.h>
#include <wavpackfile.h>
#include <trueaudiofile.h>

#include "defs.hh"
#include "script.hh"

using namespace std;

#define FILE_GLOBAL "__audio_file"
#define FILENAME_GLOBAL "FILE"
#define VERBOSE_GLOBAL "VERBOSE"
#define COUNT_GLOBAL "COUNT"
#define TOTAL_GLOBAL "TOTAL"

static int file_type(lua_State *L)
{
    lua_getglobal(L, FILE_GLOBAL);
    TagLib::FileRef *f = (TagLib::FileRef *) lua_touserdata(L, 1);

    if (!f) {
        lua_pushnil(L);
        lua_pushstring(L, "no audio file");
        return 2;
    }

    TagLib::File *file = f->file();
    if (typeid(*file) == typeid(TagLib::MPEG::File))
        lua_pushstring(L, "mpeg");
    else if (typeid(*file) == typeid(TagLib::Ogg::Vorbis::File))
        lua_pushstring(L, "ogg");
    else if (typeid(*file) == typeid(TagLib::FLAC::File))
        lua_pushstring(L, "flac");
    else if (typeid(*file) == typeid(TagLib::MPC::File))
        lua_pushstring(L, "mpc");
    else if (typeid(*file) == typeid(TagLib::Ogg::FLAC::File))
        lua_pushstring(L, "oggflac");
    else if (typeid(*file) == typeid(TagLib::WavPack::File))
        lua_pushstring(L, "wavpack");
    else if (typeid(*file) == typeid(TagLib::Ogg::Speex::File))
        lua_pushstring(L, "speex");
    else if (typeid(*file) == typeid(TagLib::TrueAudio::File))
        lua_pushstring(L, "trueaudio");
    else
        lua_pushnil(L);
    return 1;
}

static int tag_get(lua_State *L)
{
    const char *tname = luaL_checkstring(L, 1);
    bool unicode = lua_toboolean(L, 2);
    int findex = lua_isnone(L, 2) ? 2 : 3;

    lua_getglobal(L, FILE_GLOBAL);
    TagLib::FileRef *f = (TagLib::FileRef *) lua_touserdata(L, findex);

    if (!f) {
        lua_pushnil(L);
        lua_pushstring(L, "no audio file");
        return 2;
    }
    else if (!f->tag()) {
        lua_pushnil(L);
        lua_pushstring(L, "failed to get tags");
        return 2;
    }

    TagLib::Tag *tag = f->tag();
    if (0 == strncmp(tname, "title", 6))
        lua_pushstring(L, tag->title().toCString(unicode));
    else if (0 == strncmp(tname, "artist", 7))
        lua_pushstring(L, tag->artist().toCString(unicode));
    else if (0 == strncmp(tname, "album", 6))
        lua_pushstring(L, tag->album().toCString(unicode));
    else if (0 == strncmp(tname, "comment", 8))
        lua_pushstring(L, tag->comment().toCString(unicode));
    else if (0 == strncmp(tname, "genre", 6))
        lua_pushstring(L, tag->genre().toCString(unicode));
    else if (0 == strncmp(tname, "year", 5))
        lua_pushinteger(L, tag->year());
    else if (0 == strncmp(tname, "track", 6))
        lua_pushinteger(L, tag->track());
    else
        return luaL_argerror(L, 1, "invalid tag");
    return 1;
}

static int tag_set(lua_State *L)
{
    const char *tname = luaL_checkstring(L, 1);

    int tnum = 0;
    const char *tval;
    if (0 == strncmp(tname, "year", 5) || 0 == strncmp(tname, "track", 6)) {
        tnum = luaL_checkinteger(L, 2);
        tval = NULL;
    }
    else
        tval = luaL_checkstring(L, 2);

    lua_getglobal(L, FILE_GLOBAL);
    TagLib::FileRef *f = (TagLib::FileRef *) lua_touserdata(L, 3);

    if (!f) {
        lua_pushnil(L);
        lua_pushstring(L, "no audio file");
        return 2;
    }
    else if (!f->tag()) {
        lua_pushnil(L);
        lua_pushstring(L, "failed to get tags");
        return 2;
    }

    TagLib::Tag *tag = f->tag();
    if (0 == strncmp(tname, "title", 6))
        tag->setTitle(tval);
    else if (0 == strncmp(tname, "artist", 7))
        tag->setArtist(tval);
    else if (0 == strncmp(tname, "album", 6))
        tag->setAlbum(tval);
    else if (0 == strncmp(tname, "comment", 8))
        tag->setComment(tval);
    else if (0 == strncmp(tname, "genre", 6))
        tag->setGenre(tval);
    else if (0 == strncmp(tname, "year", 5))
        tag->setYear(tnum);
    else if (0 == strncmp(tname, "track", 6))
        tag->setTrack(tnum);
    else
        return luaL_argerror(L, 1, "invalid tag");
    lua_pushboolean(L, f->save());
    return 1;
}

static int prop_get(lua_State *L)
{
    const char *pname = luaL_checkstring(L, 1);

    lua_getglobal(L, FILE_GLOBAL);
    TagLib::FileRef *f = (TagLib::FileRef *) lua_touserdata(L, 2);

    if (!f) {
        lua_pushnil(L);
        lua_pushstring(L, "no audio file");
        return 2;
    }
    else if (!f->audioProperties()) {
        lua_pushnil(L);
        lua_pushstring(L, "failed to get audio properties");
        return 2;
    }

    TagLib::AudioProperties *props = f->audioProperties();
    if (0 == strncmp(pname, "length", 7))
        lua_pushinteger(L, props->length());
    else if (0 == strncmp(pname, "bitrate", 8))
        lua_pushinteger(L, props->bitrate());
    else if (0 == strncmp(pname, "samplerate", 11))
        lua_pushinteger(L, props->sampleRate());
    else if (0 == strncmp(pname, "channels", 9))
        lua_pushinteger(L, props->channels());
    else
        return luaL_argerror(L, 1, "invalid audio property");
    return 1;
}

static const struct luaL_reg file_methods[] = {
    {"type", file_type},
    {NULL, NULL}
};

static const struct luaL_reg tag_methods[] = {
    {"get", tag_get},
    {"set", tag_set},
    {NULL, NULL}
};

static const struct luaL_reg prop_methods[] = {
    {"get", prop_get},
    {NULL, NULL}
};

lua_State *init_lua(void)
{
    lua_State *L = lua_open();
    luaL_openlibs(L);
    luaL_register(L, "file", file_methods);
    luaL_register(L, "tag", tag_methods);
    luaL_register(L, "prop", prop_methods);

    // Initialize using ENV_INIT
    char *init = getenv(ENV_INIT);
    if (NULL != init) {
        if ('@' == init[0]) {
            init++;
            if (0 != luaL_dofile(L, init)) {
                cerr << PACKAGE": error running init script `" << init << "': " << lua_tostring(L, -1) << endl;
                lua_pop(L, 1);
            }
        }
        else {
            if (0 != luaL_dostring(L, init)) {
                cerr << PACKAGE": error running code from "ENV_INIT": " << lua_tostring(L, -1) << endl;
                lua_pop(L, 1);
            }
        }
    }
    return L;
}

string getscript(const char *script)
{
    string script_path = "";
    char *home = getenv("HOME");
    if (NULL == home) {
        uid_t uid;
        struct passwd *pwd;

        uid = geteuid();
        errno = 0;
        pwd = getpwuid(uid);

        if (0 != errno)
            return NULL;
        home = pwd->pw_name;
    }
    script_path += home;
    script_path += SCRIPT_SEARCH_DIR;
    script_path += script;

    struct stat buf;
    if (0 == stat(script_path.c_str(), &buf))
        return script_path;
    return string("");
}

void doscript(const char *script, lua_State *L, TagLib::FileRef *f,
        const char *file, bool verbose, int count, int total)
{
    int ret;
    struct stat buf;

    lua_pushlightuserdata(L, f);
    lua_setglobal(L, FILE_GLOBAL);
    lua_pushstring(L, file);
    lua_setglobal(L, FILENAME_GLOBAL);
    lua_pushboolean(L, verbose);
    lua_setglobal(L, VERBOSE_GLOBAL);
    lua_pushinteger(L, count);
    lua_setglobal(L, COUNT_GLOBAL);
    lua_pushinteger(L, total);
    lua_setglobal(L, TOTAL_GLOBAL);

    if (0 == strncmp(script, "-", 2)) {
        // Read from standard input
        script = NULL;
    }

    if ('/' == script[0] || 0 == stat(script, &buf))
        ret = luaL_dofile(L, script);
    else {
        string script_path = getscript(script);
        if (0 != script_path.compare(""))
            ret = luaL_dofile(L, script_path.c_str());
        else {
            cerr << PACKAGE": failed to find script `" << script << "'" << endl;
            return;
        }
    }
    if (0 != ret) {
        cerr << PACKAGE": error running script: " << lua_tostring(L, -1) << endl;
        lua_pop(L, 1);
    }
}
