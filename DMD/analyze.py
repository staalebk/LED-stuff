#!/usr/bin/env python3
import json

times = {}
games = {}
cs = []

with open('json/dmd.json') as json_file:
    clips = json.load(json_file)
    for clip in clips:
        name = clip['name']
#        print(name)
        frames = clip['frames']
        anim = clip['animation']
        m = clip['mystery']
        if m[9] == 0:
            print(name)
#            print(m[2])
#        else:
#            continue
#        print(m[9])
        cs.append(clip)
#        game_name = '_'.join(name.split('_')[:-1])
#        if game_name not in games:
#            games[game_name] = {}
#            games[game_name]['colors'] = [0] * 16
#        with open('json/anim/' + name + '.json') as anim_file:
#            clip = {}
#            clip['name'] = name
#            colors = [0] * 16
#            a = json.load(anim_file)
#            for frame in a['frames']:
#                for pixel in frame:
#                    colors[pixel] = 
#                    games[game_name]['colors'][pixel] = games[game_name]['colors'][pixel] + 1
#            games[game_name]['clips']
#            print(name)
#    print(clips)

#for game in games:
#    print(game)
#    print(games[game]['colors'])
#    print(json.dumps(games))
