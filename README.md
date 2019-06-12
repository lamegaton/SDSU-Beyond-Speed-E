# Although the project deadline is due, I still want to working on it to have a stable product!
Please take a look and build our wiki page https://github.com/lamegaton/Comm-and-Sensor---Speed-e/wiki  
new line = two space 

# Power
1. library we are using: https://github.com/rocketscream/Low-Power


# Sensor
1. confidence level of car is around **5-7**
2. our max distance is 14 feet from the curve to the first white stripe line
3. on **04/18/2019**, we remove calibration code because it's not important
4. change **trig time to 20s and echo to 2s**


# Communication
1. planning to use different spreading factor for diffrent node
2. We've trying to get the downlink from TTN from node 
3. For testing and increase the range, we set the gateway to single channel and single Spreading factor (10)
## Lmic
`os_setTimedCallback(&sendjob, os_getTime() + ms2osticks(10), do_send);  //do a send`  
  
Schedule time to send  
  
`#define ms2osticks(ms)   ((ostime_t)( ((s8_t)(ms) * OSTICKS_PER_SEC)    / 1000)) // or using us2osticks`  
`os_runloop_once(); //check status `


## Location
1. Gateway:  
32.777651, -117.070372  

2. Node 1:   
32°46'43.9"N 117°04'23.6"W  
32.778863, -117.073228 (end segment -- sensor position)  
32.778916, -117.074302 (start segment)  

|Point |	location |	note
|---|---|---|
|1	|32.778949, -117.072468	|only work at this point
|2	|32.778953, -117.071395	|the gateway recieves signal from node2 to node3
|3	|32.778955, -117.070642	|
|gateway|	32.777651, -117.070372|	


## User interface
https://developers.google.com/maps/documentation/javascript/overlays?csw=1#ImageMapTypes
https://docs.mapbox.com/mapbox-gl-js/api/#customlayerinterface  

https://angus.readthedocs.io/en/2014/amazon/transfer-files-between-instance.html  
  
what we want to achieve at this point is showing **red** segment when **car <= 8** and **car > 3**
and showing **green** segment when **car > 24** (look at matlab file to see our logic)

We can use this instead:  
https://developer.tomtom.com/maps-sdk-web/functional-examples#map-vector-geojson  
or another option: mapbox  
also we need to fetch data to mapbox...  
https://developer.mozilla.org/en-US/docs/Learn/JavaScript/Client-side_web_APIs/Fetching_data


## Notes for team:
1. Please write code in block or function so we can easily intergrate
2. we have only 18 days left babes!
3. If you want to upload a new code with lmic, you have to change NWKey and APPKey

# Our team member:  
1. Mahrlo Amposta (power, design PCB) (amposta.mahrlo@gmail.com)
2. Ernesto Rodriguez (presentation,power) (ernierodriguez619@gmail.com)
3. Christian Rizzo (antenna)(chrisrizzo.ee.117@gmail.com)
4. Adam Cruz - adamjohncruz (sensor, housing) (adamjohncruz@gmail.com)
5. Mark Ewest (user interface) (markewest021@gmail.com)
6. Mayra Pulido  (user interface)(pulido.mayra@hotmail.com)
7. Son Pham - lamegaton (sensor, communication) (sonpham995@gmail.com)
