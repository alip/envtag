#!/usr/bin/env lua
-- envtag script to print statistics about your music database
-- vim: set ft=lua et sts=4 sw=4 ts=4 fdm=marker:
-- Copyright 2009 Ali Polatel <polatel@googlemail.com>
-- Distributed under the terms of the GNU General Public License v2

-- envtag uses the same lua_State for all processed files
-- run this like: envtag -S stats.lua **/*.(flac|mp3|ogg|spx|tta)
-- or copy this to ~/.envtag/lua/stats and run it like envtag -S stats ...

if loop.first then
    Song = {}
    function Song:new(tag, prop)
        local instance = {
            name = file.name,
            xiph = tag.has_xiph(),

            length = prop.get"length" or 0,
            bitrate = prop.get"bitrate" or 0,
            samplerate = prop.get"samplerate" or 0,
            channels = prop.get"channels" or 0,
        }

        title = tag.get("title", opt.unicode)
        instance.title = title and (instance.xiph and title[1] or title)
        artist = tag.get("artist", opt.unicode)
        instance.artist = artist and (instance.xiph and artist[1] or artist)
        album = tag.get("album", opt.unicode)
        instance.album = album and (instance.xiph and album[1] or album)
        genre = tag.get("genre", opt.unicode)
        instance.genre = genre and (instance.xiph and genre[1] or genre)

        return setmetatable(instance, {__index = Song})
    end

    SongList = {}
    function SongList:new()
        return setmetatable({}, {__index = SongList})
    end
    function SongList:getall(key)
        local t = {}
        local count = 0
        for _, song in ipairs(self) do
            value = song[key]
            if (type(value) == "number" and value ~= 0) or
                (type(value) == "string" and value ~= "") then
                if t[value] then
                    t[value] = t[value] + 1
                else
                    count = count + 1
                    t[value] = 1
                end
            end
        end
        return t, count
    end

    function top(alltable, count)
        local best = {}
        for i=1,count do
            local elem = {count = 0}
            for key, count in pairs(alltable) do
                if count > elem.count then
                    elem = {key = key, count = count}
                end
            end
            if elem.key then
                alltable[elem.key] = nil
                table.insert(best, elem)
            end
        end
        return best
    end

    SECSPERDAY = 86400
    SECSPERHOUR = 3600
    SECSPERMIN = 60
    function prettytime(t)
        days = t / SECSPERDAY
        t = t % SECSPERDAY
        hours = t / SECSPERHOUR
        t = t % SECSPERHOUR
        mins = t / SECSPERMIN
        t = t % SECSPERMIN
        secs = t

        return string.format("%d days, %d:%02d:%02d", days, hours, mins, secs)
    end

    db = SongList:new()
end

if opt.verbose then
    print(string.format("> (%d/%d) %s", loop.count, loop.total, file.name))
end
table.insert(db, Song:new(tag, prop))

if loop.last then
    artists, artists_count = db:getall"artist"
    albums, albums_count = db:getall"album"
    genres = db:getall"genre"

    print("Artists: " .. artists_count)
    print("Albums: " .. albums_count)
    print("Songs: " .. loop.total)

    length = 0
    for _, song in ipairs(db) do
        length = length + song.length
    end
    print("Total play time: " .. prettytime(length))

    top_artists = top(artists, 5)
    top_genres = top(genres, 5)
    for i=1,#top_artists do
        if i == 1 then print"\nTop artists:" end
        local artist = top_artists[i]
        print("\t" .. i .. ". " .. artist.key .. " (count: " .. artist.count .. ")")
    end
    for i=1,#top_genres do
        if i == 1 then print"\nTop genres:" end
        local genre = top_genres[i]
        print("\t" .. i .. ". " .. genre.key .. " (count: " .. genre.count .. ")")
    end
end
