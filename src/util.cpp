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
