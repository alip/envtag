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

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <unistd.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <taglib/tag_c.h>

#include "defs.h"

#include "script.h"
#define FILE_GLOBAL "__audio_file"
#define FILENAME_GLOBAL "FILE"
#define VERBOSE_GLOBAL "VERBOSE"
#define COUNT_GLOBAL "COUNT"
#define TOTAL_GLOBAL "TOTAL"

static int tag_get(lua_State *L) {
    const char *tname = luaL_checkstring(L, 1);

    lua_getglobal(L, FILE_GLOBAL);
    TagLib_File *fp = (TagLib_File *) lua_touserdata(L, 2);

    /* Clean the stack */
    lua_pop(L, 2);

    if (NULL == fp) {
        lua_pushnil(L);
        lua_pushstring(L, "audio file not available");
        return 2;
    }
    TagLib_Tag *tags = taglib_file_tag(fp);

    if (0 == strncmp(tname, "title", 6))
        lua_pushstring(L, taglib_tag_title(tags));
    else if (0 == strncmp(tname, "artist", 7))
        lua_pushstring(L, taglib_tag_artist(tags));
    else if (0 == strncmp(tname, "album", 6))
        lua_pushstring(L, taglib_tag_album(tags));
    else if (0 == strncmp(tname, "comment", 8))
        lua_pushstring(L, taglib_tag_comment(tags));
    else if (0 == strncmp(tname, "genre", 6))
        lua_pushstring(L, taglib_tag_genre(tags));
    else if (0 == strncmp(tname, "year", 5))
        lua_pushinteger(L, taglib_tag_year(tags));
    else if (0 == strncmp(tname, "track", 6))
        lua_pushinteger(L, taglib_tag_track(tags));
    else {
        taglib_tag_free_strings();
        return luaL_argerror(L, 2, "invalid tag");
    }

    taglib_tag_free_strings();
    return 1;
}

static int tag_set(lua_State *L) {
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
    TagLib_File *fp = (TagLib_File *) lua_touserdata(L, 3);

    /* Clean the stack */
    lua_pop(L, 3);

    if (NULL == fp) {
        lua_pushnil(L);
        lua_pushstring(L, "audio file not available");
        return 2;
    }
    TagLib_Tag *tags = taglib_file_tag(fp);

    if (0 == strncmp(tname, "title", 6))
        taglib_tag_set_title(tags, tval);
    else if (0 == strncmp(tname, "artist", 7))
        taglib_tag_set_artist(tags, tval);
    else if (0 == strncmp(tname, "album", 6))
        taglib_tag_set_album(tags, tval);
    else if (0 == strncmp(tname, "comment", 8))
        taglib_tag_set_comment(tags, tval);
    else if (0 == strncmp(tname, "genre", 6))
        taglib_tag_set_genre(tags, tval);
    else if (0 == strncmp(tname, "year", 5))
        taglib_tag_set_year(tags, tnum);
    else if (0 == strncmp(tname, "track", 6))
        taglib_tag_set_track(tags, tnum);
    else {
        taglib_tag_free_strings();
        return luaL_argerror(L, 2, "invalid tag");
    }

    taglib_file_save(fp);
    taglib_tag_free_strings();
    return 1;
}

static int prop_get(lua_State *L) {
    const char *pname = luaL_checkstring(L, 1);

    lua_getglobal(L, FILE_GLOBAL);
    TagLib_File *fp = (TagLib_File *) lua_touserdata(L, 2);

    /* Clean the stack */
    lua_pop(L, 2);

    if (NULL == fp) {
        lua_pushnil(L);
        lua_pushstring(L, "audio file not available");
        return 2;
    }
    const TagLib_AudioProperties *props = taglib_file_audioproperties(fp);

    if (0 == strncmp(pname, "length", 7))
        lua_pushinteger(L, taglib_audioproperties_length(props));
    else if (0 == strncmp(pname, "bitrate", 8))
        lua_pushinteger(L, taglib_audioproperties_bitrate(props));
    else if (0 == strncmp(pname, "samplerate", 11))
        lua_pushinteger(L, taglib_audioproperties_samplerate(props));
    else if (0 == strncmp(pname, "channels", 9))
        lua_pushinteger(L, taglib_audioproperties_channels(props));
    else
        return luaL_argerror(L, 1, "invalid audio property");
    return 1;
}

static const struct luaL_reg tag_methods[] = {
    {"get", tag_get},
    {"set", tag_set},
    {NULL, NULL}
};

static const struct luaL_reg prop_methods[] = {
    {"get", prop_get},
    {NULL, NULL}
};

lua_State *init_lua(void) {
    lua_State *L = lua_open();
    luaL_openlibs(L);
    luaL_register(L, "tag", tag_methods);
    luaL_register(L, "prop", prop_methods);

    /* Initialize using ENV_INIT */
    char *init = getenv(ENV_INIT);
    if (NULL != init) {
        if ('@' == init[0]) {
            init++;
            if (0 != luaL_dofile(L, init)) {
                lg("Error running init script `%s': %s", init, lua_tostring(L, -1));
                lua_pop(L, 1);
            }
        }
        else {
            if (0 != luaL_dostring(L, init)) {
                lg("Error running code from "ENV_INIT": %s", lua_tostring(L, -1));
                lua_pop(L, 1);
            }
        }
    }
    return L;
}

char *getscript(const char *script) {
    char *home;
    home = getenv("HOME");
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

    char *spath = malloc(strlen(home) + strlen(script) + strlen(SCRIPT_SEARCH_DIR) + 1);
    if (NULL == spath)
        return NULL;
    strcpy(spath, home);
    strcat(spath, SCRIPT_SEARCH_DIR);
    strcat(spath, script);

    struct stat buf;
    if (0 == stat(spath, &buf))
        return spath;
    return NULL;
}

void doscript(const char *script, lua_State *L, TagLib_File *fp,
        const char *file, int verbose, int count, int total) {
    lua_pushlightuserdata(L, fp);
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
    int ret;
    struct stat buf;
    if ('/' == script[0] || 0 == stat(script, &buf))
        ret = luaL_dofile(L, script);
    else {
        char *spath = getscript(script);
        if (NULL != spath) {
            ret = luaL_dofile(L, spath);
            free(spath);
        }
        else {
            lg("Failed to find script `%s': %s", script, strerror(errno));
            return;
        }
    }
    if (0 != ret) {
        lg("Error running script: %s", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
}
