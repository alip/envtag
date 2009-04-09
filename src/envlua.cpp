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

#include <lua.hpp>

#include <fileref.h>
#include <flacfile.h>
#include <mpcfile.h>
#include <mpegfile.h>
#include <oggflacfile.h>
#include <speexfile.h>
#include <vorbisfile.h>
#include <wavpackfile.h>
#include <trueaudiofile.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef ENABLE_MAGIC
#include "envmime.hh"
#endif

#include "envlua.hh"

using namespace TagLib;

#define SONG_T "EnvTag.Song"

struct song {
    char *path;
    FileRef *f;
};

static int song_new(lua_State *L)
{
    const char *path, *type;
    bool readprops;
    struct song *s;

    path = luaL_checkstring(L, 1);
    type = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    readprops = lua_toboolean(L, 3);

    s = (struct song *) lua_newuserdata(L, sizeof(struct song));
    luaL_getmetatable(L, SONG_T);
    lua_setmetatable(L, -2);

    s->path = strdup(path);
    if (NULL == s->path) {
        lua_pop(L, 1);
        lua_pushnil(L);
        lua_pushstring(L, "not enough memory");
        return 2;
    }

    s->f = 0;
    if (NULL == type) {
        s->f = new FileRef(path, readprops);
        if (!s->f || s->f->isNull()) {
            lua_pop(L, 1);
            lua_pushnil(L);
            lua_pushstring(L, "unknown file type");
            return 2;
        }
    }
    else if (0 == strncmp(type, "flac", 5)) {
        FLAC::File *nf = new FLAC::File(path, readprops);
        if (nf && nf->isValid())
            s->f = new FileRef(nf);
        else {
            delete nf;
            lua_pop(L, 1);
            lua_pushnil(L);
            lua_pushstring(L, "invalid flac");
            return 2;
        }
    }
    else if (0 == strncmp(type, "mpc", 4)) {
        MPC::File *nf = new MPC::File(path, readprops);
        if (nf && nf->isValid())
            s->f = new FileRef(nf);
        else {
            delete nf;
            lua_pop(L, 1);
            lua_pushnil(L);
            lua_pushstring(L, "invalid mpc");
            return 2;
        }
    }
    else if (0 == strncmp(type, "mpeg", 5)) {
        MPEG::File *nf = new MPEG::File(path, readprops);
        if (nf && nf->isValid())
            s->f = new FileRef(nf);
        else {
            delete nf;
            lua_pop(L, 1);
            lua_pushnil(L);
            lua_pushstring(L, "invalid mpeg");
            return 2;
        }
    }
    else if (0 == strncmp(type, "oggflac", 8)) {
        Ogg::FLAC::File *nf = new Ogg::FLAC::File(path, readprops);
        if (nf && nf->isValid())
            s->f = new FileRef(nf);
        else {
            delete nf;
            lua_pop(L, 1);
            lua_pushnil(L);
            lua_pushstring(L, "invalid oggflac");
            return 2;
        }
    }
    else if (0 == strncmp(type, "speex", 6)) {
        Ogg::Speex::File *nf = new Ogg::Speex::File(path, readprops);
        if (nf && nf->isValid())
            s->f = new FileRef(nf);
        else {
            delete nf;
            lua_pop(L, 1);
            lua_pushnil(L);
            lua_pushstring(L, "invalid speex");
            return 2;
        }
    }
    else if (0 == strncmp(type, "vorbis", 7)) {
        Ogg::Vorbis::File *nf = new Ogg::Vorbis::File(path, readprops);
        if (nf && nf->isValid())
            s->f = new FileRef(nf);
        else {
            delete nf;
            lua_pop(L, 1);
            lua_pushnil(L);
            lua_pushstring(L, "invalid vorbis");
            return 2;
        }
    }
    else if (0 == strncmp(type, "wavpack", 8)) {
        WavPack::File *nf = new WavPack::File(path, readprops);
        if (nf && nf->isValid())
            s->f = new FileRef(nf);
        else {
            delete nf;
            lua_pop(L, 1);
            lua_pushnil(L);
            lua_pushstring(L, "invalid wavpack");
            return 2;
        }
    }
    else if (0 == strncmp(type, "trueaudio", 10)) {
        TrueAudio::File *nf = new TrueAudio::File(path, readprops);
        if (nf && nf->isValid())
            s->f = new FileRef(nf);
        else {
            delete nf;
            lua_pop(L, 1);
            lua_pushnil(L);
            lua_pushstring(L, "invalid trueaudio");
            return 2;
        }
    }
    else {
        lua_pop(L, 1);
        return luaL_argerror(L, 2, "bad type");
    }
    return 1;
}

static int song_free(lua_State *L)
{
    struct song *s = (struct song *) luaL_checkudata(L, 1, SONG_T);

    if (NULL != s->path) {
        free(s->path);
        s->path = NULL;
    }
    if (0 != s->f) {
        delete s->f;
        s->f = 0;
    }

    return 0;
}

static int song_get(lua_State *L)
{
    struct song *s = (struct song *) luaL_checkudata(L, 1, SONG_T);
    const char *name = luaL_checkstring(L, 2);
    bool unicode = lua_toboolean(L, 3);

    if (!s->f || s->f->isNull())
        return luaL_argerror(L, 1, "file closed");
    else if (!s->f->tag()) {
        lua_pushnil(L);
        lua_pushstring(L, "no tag");
        return 2;
    }

    Tag *tag = s->f->tag();
    if (0 == strncmp(name, "title", 6)) {
        if (String::null == tag->title())
            lua_pushinteger(L, 0);
        else
            lua_pushstring(L, tag->title().toCString(unicode));
    }
    else if (0 == strncmp(name, "artist", 7)) {
        if (String::null == tag->artist())
            lua_pushinteger(L, 0);
        else
            lua_pushstring(L, tag->artist().toCString(unicode));
    }
    else if (0 == strncmp(name, "album", 6)) {
        if (String::null == tag->album())
            lua_pushinteger(L, 0);
        else
            lua_pushstring(L, tag->album().toCString(unicode));
    }
    else if (0 == strncmp(name, "comment", 8)) {
        if (String::null == tag->comment())
            lua_pushinteger(L, 0);
        else
            lua_pushstring(L, tag->comment().toCString(unicode));
    }
    else if (0 == strncmp(name, "genre", 6)) {
        if (String::null == tag->genre())
            lua_pushinteger(L, 0);
        else
            lua_pushstring(L, tag->genre().toCString(unicode));
    }
    else if (0 == strncmp(name, "year", 5))
        lua_pushinteger(L, tag->year());
    else if (0 == strncmp(name, "track", 6))
        lua_pushinteger(L, tag->track());
    else
        return luaL_argerror(L, 1, "invalid tag");
    return 1;
}

static int song_set(lua_State *L)
{
    struct song *s = (struct song *) luaL_checkudata(L, 1, SONG_T);
    const char *name = luaL_checkstring(L, 2);

    int tnum = 0;
    String tval = String::null;
    if (0 == strncmp(name, "year", 5) || 0 == strncmp(name, "track", 6))
        tnum = luaL_checkinteger(L, 3);
    else {
        const char *cval = luaL_checkstring(L, 3);
        if (0 != strncmp(cval, "", 1))
            tval = String(cval);
    }

    if (!s->f || s->f->isNull())
        return luaL_argerror(L, 1, "file closed");
    else if (!s->f->tag()) {
        lua_pushnil(L);
        lua_pushstring(L, "no tag");
        return 2;
    }

    Tag *tag = s->f->tag();
    if (0 == strncmp(name, "title", 6))
        tag->setTitle(tval);
    else if (0 == strncmp(name, "artist", 7))
        tag->setArtist(tval);
    else if (0 == strncmp(name, "album", 6))
        tag->setAlbum(tval);
    else if (0 == strncmp(name, "comment", 8))
        tag->setComment(tval);
    else if (0 == strncmp(name, "genre", 6))
        tag->setGenre(tval);
    else if (0 == strncmp(name, "year", 5))
        tag->setYear(tnum);
    else if (0 == strncmp(name, "track", 6))
        tag->setTrack(tnum);
    else
        return luaL_argerror(L, 2, "invalid tag");
    return 0;
}

static int song_save(lua_State *L)
{
    struct song *s = (struct song *) luaL_checkudata(L, 1, SONG_T);
    if (!s->f || s->f->isNull())
        return luaL_argerror(L, 1, "file closed");
    lua_pushboolean(L, s->f->save() ? 1 : 0);
    return 1;
}

static int song_property(lua_State *L)
{
    struct song *s = (struct song *) luaL_checkudata(L, 1, SONG_T);
    const char *name = luaL_checkstring(L, 2);

    if (!s->f || s->f->isNull())
        return luaL_argerror(L, 1, "file closed");
    if (!s->f->audioProperties()) {
        lua_pushnil(L);
        lua_pushstring(L, "no audio property");
        return 2;
    }

    AudioProperties *props = s->f->audioProperties();
    if (0 == strncmp(name, "length", 7))
        lua_pushinteger(L, props->length());
    else if (0 == strncmp(name, "bitrate", 8))
        lua_pushinteger(L, props->bitrate());
    else if (0 == strncmp(name, "samplerate", 11))
        lua_pushinteger(L, props->sampleRate());
    else if (0 == strncmp(name, "channels", 9))
        lua_pushinteger(L, props->channels());
    else
        return luaL_argerror(L, 2, "invalid audio property");
    return 1;
}

static int song_has_xiph(lua_State *L)
{
    struct song *s = (struct song *) luaL_checkudata(L, 1, SONG_T);

    if (!s->f || s->f->isNull())
        return luaL_argerror(L, 1, "file closed");

    File *file = s->f->file();
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

static int song_get_xiph(lua_State *L)
{
    struct song *s = (struct song *) luaL_checkudata(L, 1, SONG_T);
    const char *name = luaL_checkstring(L, 2);
    bool unicode = lua_toboolean(L, 3);

    if (!s->f || s->f->isNull())
        return luaL_argerror(L, 1, "file closed");
    else if (!s->f->tag()) {
        lua_pushnil(L);
        lua_pushstring(L, "no tag");
        return 2;
    }

    File *file = s->f->file();
    Ogg::XiphComment *xtag;
    if (dynamic_cast<Ogg::File *>(file))
        xtag = dynamic_cast<Ogg::XiphComment *>(s->f->tag());
    else if (dynamic_cast<FLAC::File *>(file)) {
        FLAC::File *ff = dynamic_cast<FLAC::File *>(file);
        if (!ff->xiphComment())
            return luaL_error(L, "no xiph comment");
        xtag = ff->xiphComment();
    }
    else
        return luaL_error(L, "no xiph comment");

    if (!isxiph(name))
        return luaL_argerror(L, 2, "invalid tag");

    const Ogg::FieldListMap flm = xtag->fieldListMap();
    String upname = String(name).upper();
    if (!xtag->contains(upname)) {
        lua_pushinteger(L, 0);
        return 1;
    }

    int tableind = 1;
    lua_newtable(L);
    StringList::ConstIterator valuesIt = flm[upname].begin();
    for (; valuesIt != flm[upname].end(); valuesIt++) {
        ByteVector vect = (*valuesIt).data(unicode ? String::UTF8 : String::Latin1);
        /* Make sure the data is NULL terminated */
        const char *data = vect.data();
        char *datac = strndup(data, vect.size());
        if (NULL == datac)
            return luaL_error(L, "out of memory");
        datac[vect.size()] = '\0';
        lua_pushinteger(L, tableind++);
        lua_pushstring(L, datac);
        lua_settable(L, -3);
        free(datac);
    }
    return 1;
}

static const luaL_reg song_methods[] = {
    {"get", song_get},
    {"set", song_set},
    {"save", song_save},
    {"property", song_property},
    {"has_xiph", song_has_xiph},
    {"get_xiph", song_get_xiph},
    {"close", song_free},
    {"__gc", song_free},
    {NULL, NULL}
};

static const luaL_reg envtag_methods[] = {
    {"Song", song_new},
    {NULL, NULL}
};

lua_State *init_lua(void)
{
    lua_State *L = lua_open();
    luaL_openlibs(L);

    luaL_register(L, "envtag", envtag_methods);
    luaL_newmetatable(L, SONG_T);
    luaL_register(L, NULL, song_methods);
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2); // push the metatable
    lua_settable(L, -3); // metatable.__index = metatable

    // Add LIBEXECDIR/PACKAGE/modules/?.lua to package.path
    if (0 != luaL_dostring(L, "package.path = '"LIBEXECDIR"/"PACKAGE"/modules/?.lua;' .. package.path"))
        std::cerr << PACKAGE": error adding envtag module directory to package.path: " \
            << lua_tostring(L, -1) << std::endl;

#ifdef ENABLE_MAGIC
    FileRef::addFileTypeResolver(new MagicFileTypeResolver);
#endif

    return L;
}

void close_lua(lua_State *L)
{
    lua_close(L);
}

int docommand(lua_State *L, int argc, char **argv)
{
    int ret;

    // command line args to Lua
    lua_newtable(L);
    for (int i = 0; i < argc; i++) {
        lua_pushinteger(L, i);
        lua_pushstring(L, argv[i]);
        lua_settable(L, -3);
    }
    lua_setglobal(L, "arg");

    std::string path = LIBEXECDIR"/"PACKAGE"/commands/" + std::string(argv[0]) + ".lua";
    ret = luaL_dofile(L, path.c_str());
    if (0 != ret) {
        std::cerr << PACKAGE": " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1);
        return EXIT_FAILURE;
    }
    else {
        lua_getglobal(L, "RETVAL");
        ret = lua_isnoneornil(L, -1) ? 0 : luaL_checkinteger(L, -1);
        lua_pop(L, 1);
    }
    return ret;
}
