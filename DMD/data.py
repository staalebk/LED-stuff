#!/usr/bin/env python3
import time
import os, sys
import datetime
import json
import urllib.request
from statistics import median
from datetime import datetime
from geopy.geocoders import Nominatim
from geopy.distance import geodesic
import patatmo
import settings
import finnhub

# Define your netatmo connect developer credentials




# configure the authentication
authentication = patatmo.api.authentication.Authentication(
    credentials=credentials,
    tmpfile = "temp_auth.json")

# create an api client
client = patatmo.api.client.NetatmoClient(authentication)

#place = "Eiriks Gate 18, Oslo"
#geolocator = Nominatim(user_agent="failbot 1.0")
#gc = geolocator.geocode(place)
#print("lat = " + str(gc.latitude))
#print("lon = " + str(gc.longitude))
#print("lat_ne = " + str(gc.latitude + 0.02))
#print("lat_sw = " + str(gc.latitude - 0.02))
#print("lon_ne = " + str(gc.longitude + 0.02))
#print("lon_sw = " + str(gc.longitude - 0.02))

lat = 59.912895
lon = 10.772968
lat_ne = 59.932895
lat_sw = 59.892894999999996
lon_ne = 10.792968
lon_sw = 10.752968000000001


location = (lat, lon)

# lat/lon outline of Area Of Interest (Here I took NewYorkCity region)
region = {
        "lat_ne": lat_ne,
        "lat_sw": lat_sw,
        "lon_ne": lon_ne,
        "lon_sw": lon_sw,
        }

# issue the API request for getting public data
nyc = client.Getpublicdata(region = region)

stations_by_dist = {}
for i in nyc.response["body"]:
        sloc = (i['place']['location'][1], i['place']['location'][0])
        dist = geodesic(location, sloc).meters
        stations_by_dist[dist] = i

temps = []
humiditys = []
rains = []
rainds = []
pressures = []
for dist in sorted(stations_by_dist)[:20]:
        s = stations_by_dist[dist]
        for dev_id in s['measures']:
                dev = s['measures'][dev_id]
                if "res" in dev:
                        for t in dev["res"]:
                                m = dev["res"][t]
                                for i in range(0, len(m)):
                                        if "temperature" in dev['type'][i]:
                                                temps.append(m[i])
                                        if "humidity" in dev['type'][i]:
                                                humiditys.append(m[i])
                                        if "pressure" in dev['type'][i]:
                                                pressures.append(m[i])
                if "rain_60min" in dev:
                        rains.append(dev['rain_60min'])
                        rainds.append(dev['rain_24h'])

data = {}
data['temp'] = -666
data['humidity'] = -1
data['rain'] = -1
data['rainday'] = -1
data['pressure'] = -1
temp = -666
hum = -1
rain = -1
raind = -1
pres = -1
if(len(temps)):
        temp = median(temps)
if(len(humiditys)):
        hum = median(humiditys)
if(len(rains)):
        rain = median(rains)
if(len(rainds)):
        raind = median(rainds)
if(len(pressures)):
        pres = median(pressures)
if temp == -666:
        pass
else:
        data['temp'] = temp
        data['humidity'] = hum
        data['rain'] = rain
        data['rainday'] = raind
        data['pressure'] = pres

fh = finnhub.Client(api_key = settings.FINNHUB_TOKEN)


ticker = "TSLA"
s = fh.quote(ticker)
data[ticker] = s['c']
data[ticker + '_change'] = ((s['c']/s['pc']-1)*100)

spacex = urllib.request.urlopen('https://api.spacexdata.com/v4/launches/upcoming')
launch = json.loads(spacex.read())[0]
data['spacex_launch'] = launch
data['spacex_launch']['date_unix'] = data['spacex_launch']['date_unix'] #- 60*36

print(data)

with open('data.json', 'w') as outfile:
        json.dump(data, outfile, indent = 4)
