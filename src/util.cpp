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

#include <fileref.h>
#include <tag.h>
#include <id3v2tag.h>

#include <flacfile.h>
#include <mpcfile.h>
#include <mpegfile.h>
#include <oggflacfile.h>
#include <speexfile.h>
#include <vorbisfile.h>
#include <wavpackfile.h>
#include <trueaudiofile.h>

#include "defs.hh"
#include "util.hh"

using namespace TagLib;

static std::string escapeQuotes(const String src)
{
    string escaped_str = "";

    for (unsigned int i = 0; i < src.length(); i++) {
        if ('\'' == src[i])
            escaped_str.append("'\\''");
        else
            escaped_str.append(1, src[i]);
    }
    return escaped_str;
}

FileRef *openFile(const char *path, int type, bool readprops)
{
    if (TagLib_File_MPEG == type) {
        MPEG::File *nf = new MPEG::File(path, readprops);
        if (nf->isValid())
            return new FileRef(nf);
        else {
            delete nf;
            return 0;
        }
    }
    else if (TagLib_File_OggVorbis == type) {
        Ogg::Vorbis::File *nf = new Ogg::Vorbis::File(path, readprops);
        if (nf->isValid())
            return new FileRef(nf);
        else {
            delete nf;
            return 0;
        }
    }
    else if (TagLib_File_FLAC == type) {
        FLAC::File *nf = new FLAC::File(path, readprops);
        if (nf->isValid())
            return new FileRef(nf);
        else {
            delete nf;
            return 0;
        }
    }
    else if (TagLib_File_MPC == type) {
        MPC::File *nf = new MPC::File(path, readprops);
        if (nf->isValid())
            return new FileRef(nf);
        else {
            delete nf;
            return 0;
        }
    }
    else if (TagLib_File_OggFlac == type) {
        Ogg::FLAC::File *nf = new Ogg::FLAC::File(path, readprops);
        if (nf->isValid())
            return new FileRef(nf);
        else {
            delete nf;
            return 0;
        }
    }
    else if (TagLib_File_WavPack == type) {
        WavPack::File *nf = new WavPack::File(path, readprops);
        if (nf->isValid())
            return new FileRef(nf);
        else {
            delete nf;
            return 0;
        }
    }
    else if (TagLib_File_Speex == type) {
        Ogg::Speex::File *nf = new Ogg::Speex::File(path, readprops);
        if (nf->isValid())
            return new FileRef(nf);
        else {
            delete nf;
            return 0;
        }
    }
    else if (TagLib_File_TrueAudio == type) {
        TrueAudio::File *nf = new TrueAudio::File(path, readprops);
        if (nf->isValid())
            return new FileRef(nf);
        else {
            delete nf;
            return 0;
        }
    }
    else
        return new FileRef(path, readprops);
}

void setID3v2DefaultEncoding(int encoding)
{
    String::Type type = String::UTF8;

    switch (encoding) {
        case TagLib_ID3v2_Latin1:
            type = String::Latin1;
            break;
        case TagLib_ID3v2_UTF16:
            type = String::UTF16;
            break;
        case TagLib_ID3v2_UTF16BE:
            type = String::UTF16BE;
            break;
        case TagLib_ID3v2_UTF8:
            type = String::UTF8;
            break;
    }

    ID3v2::FrameFactory::instance()->setDefaultTextEncoding(type);
}

bool dumpTags(const FileRef f, bool export_vars)
{
    const char *prefix = export_vars ? "export " : "";
    if (!f.tag())
        return false;

    Tag *tag = f.tag();

    std::cout << prefix << "TITLE='" << escapeQuotes(tag->title()) << "'" << std::endl;
    std::cout << prefix << "ARTIST='" << escapeQuotes(tag->artist()) << "'" << std::endl;
    std::cout << prefix << "ALBUM='" << escapeQuotes(tag->album()) << "'" << std::endl;
    std::cout << prefix << "COMMENT='" << escapeQuotes(tag->comment()) << "'" << std::endl;
    std::cout << prefix << "GENRE='" << escapeQuotes(tag->genre()) << "'" << std::endl;
    std::cout << prefix << "YEAR=" << tag->year() << std::endl;
    std::cout << prefix << "TRACK=" << tag->track() << std::endl;
    return true;
}

bool dumpProperties(const FileRef f, bool export_vars)
{
    const char *prefix = export_vars ? "export " : "";
    if (!f.audioProperties())
        return false;

    AudioProperties *props = f.audioProperties();

    std::cout << prefix << "BITRATE=" << props->bitrate() << std::endl;
    std::cout << prefix << "SAMPLERATE=" << props->sampleRate() << std::endl;
    std::cout << prefix << "CHANNELS=" << props->channels() << std::endl;
    std::cout << prefix << "LENGTH=" << props->length() << std::endl;
    return true;
}

bool setTags(FileRef f, bool unicode)
{
    static char *env_title = NULL;
    static char *env_artist = NULL;
    static char *env_album = NULL;
    static char *env_comment = NULL;
    static char *env_genre = NULL;
    static char *env_year = NULL;
    static char *env_track = NULL;

    if (!f.tag())
        return false;
    Tag *tag = f.tag();

    env_title = NULL == env_title ? getenv("TITLE") : env_title;
    env_artist = NULL == env_artist ? getenv("ARTIST") : env_artist;
    env_album = NULL == env_album ? getenv("ALBUM") : env_album;
    env_comment = NULL == env_comment ? getenv("COMMENT") : env_comment;
    env_genre = NULL == env_genre ? getenv("GENRE") : env_genre;
    env_year = NULL == env_year ? getenv("YEAR") : env_year;
    env_track = NULL == env_track ? getenv("TRACK") : env_track;

    if (NULL != env_title)
        tag->setTitle(String(env_title, unicode ? String::UTF8 : String::Latin1));
    if (NULL != env_artist)
        tag->setArtist(String(env_artist, unicode ? String::UTF8 : String::Latin1));
    if (NULL != env_album)
        tag->setAlbum(String(env_album, unicode ? String::UTF8 : String::Latin1));
    if (NULL != env_comment)
        tag->setComment(String(env_comment, unicode ? String::UTF8 : String::Latin1));
    if (NULL != env_genre)
        tag->setGenre(String(env_genre, unicode ? String::UTF8 : String::Latin1));
    if (NULL != env_year)
        tag->setYear(atoi(env_year));
    if (NULL != env_track)
        tag->setTrack(atoi(env_track));

    return f.save();
}
