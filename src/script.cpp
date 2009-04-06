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
#include <assert.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <unistd.h>

#include <lua.hpp>

#include <tstring.h>
#include <tstringlist.h>

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

#include <xiphcomment.h>

#include "defs.hh"
#include "script.hh"

using namespace TagLib;

static inline bool isxiph(const char *tag)
{
    if (0 == strncmp(tag, "title", 6))
        return true;
    else if (0 == strncmp(tag, "version", 8))
        return true;
    else if (0 == strncmp(tag, "album", 6))
        return true;
    else if (0 == strncmp(tag, "artist", 7))
        return true;
    else if (0 == strncmp(tag, "performer", 10))
        return true;
    else if (0 == strncmp(tag, "copyright", 10))
        return true;
    else if (0 == strncmp(tag, "organization", 13))
        return true;
    else if (0 == strncmp(tag, "description", 12))
        return true;
    else if (0 == strncmp(tag, "genre", 6))
        return true;
    else if (0 == strncmp(tag, "date", 5))
        return true;
    else if (0 == strncmp(tag, "location", 9))
        return true;
    else if (0 == strncmp(tag, "contact", 8))
        return true;
    else if (0 == strncmp(tag, "isrc", 5))
        return true;
    else
        return false;
}

static FileRef *getfp(lua_State *L)
{
    lua_getglobal(L, "file");
    lua_pushliteral(L, "udata");
    lua_gettable(L, -2);
    return (FileRef *) lua_touserdata(L, -1);
}

static int file_save(lua_State *L)
{
    FileRef *f = getfp(L);

    if (!f) {
        lua_pushnil(L);
        lua_pushstring(L, "no audio file");
        return 2;
    }

    lua_pushboolean(L, f->save());
    return 1;
}

static int file_type(lua_State *L)
{
    FileRef *f = getfp(L);

    if (!f) {
        lua_pushnil(L);
        lua_pushstring(L, "no audio file");
        return 2;
    }

    File *file = f->file();
    if (dynamic_cast<MPEG::File *>(file))
        lua_pushstring(L, "mpeg");
    else if (dynamic_cast<Ogg::Vorbis::File *>(file))
        lua_pushstring(L, "ogg");
    else if (dynamic_cast<FLAC::File *>(file))
        lua_pushstring(L, "flac");
    else if (dynamic_cast<MPC::File *>(file))
        lua_pushstring(L, "mpc");
    else if (dynamic_cast<Ogg::FLAC::File *>(file))
        lua_pushstring(L, "oggflac");
    else if (dynamic_cast<WavPack::File *>(file))
        lua_pushstring(L, "wavpack");
    else if (dynamic_cast<Ogg::Speex::File *>(file))
        lua_pushstring(L, "speex");
    else if (dynamic_cast<TrueAudio::File *>(file))
        lua_pushstring(L, "trueaudio");
    else
        lua_pushnil(L);
    return 1;
}

static int tag_get(lua_State *L)
{
    const char *tname = luaL_checkstring(L, 1);
    bool unicode = lua_toboolean(L, 2);
    FileRef *f = getfp(L);

    if (!f)
        return luaL_error(L, "no audio file");
    else if (!f->tag())
        return luaL_error(L, "no audio tags");

    Tag *tag = f->tag();
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

static int tag_hasxiph(lua_State *L)
{
    FileRef *f = getfp(L);
    if (!f)
        return luaL_error(L, "no audio file");

    File *file = f->file();
    if (dynamic_cast<Ogg::File *>(file))
        lua_pushboolean(L, 1);
    else if (dynamic_cast<FLAC::File *>(file)) {
        FLAC::File *ff = dynamic_cast<FLAC::File *>(file);
        lua_pushboolean(L, ff->xiphComment() ? 1 : 0);
    }
    else
        lua_pushboolean(L, 0);
    return 1;
}

static int tag_getxiph(lua_State *L)
{
    const char *tname = luaL_checkstring(L, 1);
    bool unicode = lua_toboolean(L, 2);
    FileRef *f = getfp(L);

    if (!f)
        return luaL_error(L, "no audio file");
    else if (!f->tag())
        return luaL_error(L, "no audio tags");

    File *file = f->file();
    Ogg::XiphComment *xtag;
    if (dynamic_cast<Ogg::File *>(file))
        xtag = dynamic_cast<Ogg::XiphComment *>(f->tag());
    else if (dynamic_cast<FLAC::File *>(file)) {
        FLAC::File *ff = dynamic_cast<FLAC::File *>(file);
        if (!ff->xiphComment())
            return luaL_error(L, "no xiph comment");
        xtag = ff->xiphComment();
    }
    else
        return luaL_error(L, "no xiph comment");

    if (!isxiph(tname))
        return luaL_argerror(L, 1, "invalid tag");

    const Ogg::FieldListMap flm = xtag->fieldListMap();
    String tupper = String(tname).upper();
    if (!xtag->contains(tupper)) {
        lua_pushnil(L);
        lua_pushstring(L, "not set");
        return 2;
    }

    int tableind = 1;
    lua_newtable(L);
    StringList::ConstIterator valuesIt = flm[tupper].begin();
    for (; valuesIt != flm[tupper].end(); valuesIt++) {
        lua_pushinteger(L, tableind++);
        lua_pushstring(L, (*valuesIt).data(unicode ? String::UTF8 : String::Latin1).data());
        lua_settable(L, -3);
    }
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

    FileRef *f = getfp(L);

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

    Tag *tag = f->tag();
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
    return 0;
}

static int prop_get(lua_State *L)
{
    const char *pname = luaL_checkstring(L, 1);
    FileRef *f = getfp(L);

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

    AudioProperties *props = f->audioProperties();
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
    {"save", file_save},
    {"type", file_type},
    {NULL, NULL}
};

static const struct luaL_reg tag_methods[] = {
    {"get", tag_get},
    {"set", tag_set},
    {"has_xiph", tag_hasxiph},
    {"get_xiph", tag_getxiph},
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

    // Modules
    luaL_register(L, "file", file_methods);
    luaL_register(L, "tag", tag_methods);
    luaL_register(L, "prop", prop_methods);

    // Tables
    lua_newtable(L);
    lua_setglobal(L, "opt");
    lua_newtable(L);
    lua_setglobal(L, "loop");

    // Initialize using ENV_INIT
    char *init = getenv(ENV_INIT);
    if (NULL != init) {
        if ('@' == init[0]) {
            init++;
            if (0 != luaL_dofile(L, init)) {
                std::cerr << PACKAGE": error running init script `" << init << "': " << lua_tostring(L, -1) << std::endl;
                lua_pop(L, 1);
            }
        }
        else {
            if (0 != luaL_dostring(L, init)) {
                std::cerr << PACKAGE": error running code from "ENV_INIT": " << lua_tostring(L, -1) << std::endl;
                lua_pop(L, 1);
            }
        }
    }
    return L;
}

std::string getscript(const char *script)
{
    std::string script_path = "";
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
    return std::string("");
}

static void initscript(lua_State *L, FileRef *f,
        const char *file, struct envtag_opts opts,
        int count, int total)
{
    lua_getglobal(L, "file");
    lua_pushliteral(L, "udata");
    lua_pushlightuserdata(L, f);
    lua_settable(L, -3);
    lua_pushliteral(L, "name");
    lua_pushstring(L, file);
    lua_settable(L, -3);
    lua_pop(L, 1);

    lua_getglobal(L, "opt");
    lua_pushliteral(L, "unicode");
    lua_pushboolean(L, opts.unicode);
    lua_settable(L, -3);
    lua_pushliteral(L, "export");
    lua_pushboolean(L, opts.export_vars);
    lua_settable(L, -3);
    lua_pushliteral(L, "verbose");
    lua_pushboolean(L, opts.verbose);
    lua_settable(L, -3);
    lua_pushliteral(L, "read_props");
    lua_pushboolean(L, opts.read_props);
    lua_settable(L, -3);
    lua_pushliteral(L, "delimiter");
    lua_pushstring(L, opts.delimiter);
    lua_settable(L, -3);
    lua_pop(L, 1);

    lua_getglobal(L, "loop");
    lua_pushliteral(L, "count");
    lua_pushinteger(L, count);
    lua_settable(L, -3);
    lua_pushliteral(L, "total");
    lua_pushinteger(L, total);
    lua_settable(L, -3);
    lua_pushliteral(L, "first");
    lua_pushboolean(L, 1 == count ? 1 : 0);
    lua_settable(L, -3);
    lua_pushliteral(L, "last");
    lua_pushboolean(L, total == count ? 1 : 0);
    lua_settable(L, -3);
    lua_pop(L, 1);
}

int dobuiltin(const char *name, lua_State *L, FileRef *f,
        const char *file, struct envtag_opts opts,
        int count, int total)
{
    std::string script = std::string(name);
    script = LIBEXECDIR"/"PACKAGE"/" + script + ".lua";

    return doscript(script.c_str(), L, f, file, opts, count, total);
}

int doscript(const char *script, lua_State *L, FileRef *f,
        const char *file, struct envtag_opts opts,
        int count, int total)
{
    int ret;
    struct stat buf;

    initscript(L, f, file, opts, count, total);
    if (0 == strncmp(script, "-", 2)) {
        // Read from standard input
        script = NULL;
    }

    if ('/' == script[0] || 0 == stat(script, &buf))
        ret = luaL_dofile(L, script);
    else {
        std::string script_path = getscript(script);
        if (0 != script_path.compare(""))
            ret = luaL_dofile(L, script_path.c_str());
        else {
            std::cerr << PACKAGE": failed to find script `" << script << "'" << std::endl;
            return 1;
        }
    }
    if (0 != ret) {
        std::cerr << PACKAGE": error running script: " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1);
    }
    return ret;
}
