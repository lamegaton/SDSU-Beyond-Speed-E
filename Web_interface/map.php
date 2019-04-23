<!DOCTYPE html>
<html>
<head>
    <meta charset='utf-8' />
    <title>Add a GeoJSON line</title>
    <meta name='viewport' content='initial-scale=1,maximum-scale=1,user-scalable=no' />
    <script src='https://api.tiles.mapbox.com/mapbox-gl-js/v0.53.1/mapbox-gl.js'></script>
    <link href='https://api.tiles.mapbox.com/mapbox-gl-js/v0.53.1/mapbox-gl.css' rel='stylesheet' />
    <style>
        body { margin:0; padding:0; }
        #map { position:absolute; top:0; bottom:0; width:100%; }
    </style>
</head>
<body>

<div id='map'></div>
<script>
mapboxgl.accessToken = 'pk.eyJ1IjoibGFtZWdhdG9uIiwiYSI6ImNqdXJldTlidTE3Zjg0M3A3YWRvdzJ2dGwifQ.A_XWW2We6NIr_Ys48wspPg';
var map = new mapboxgl.Map({
    container: 'map',
    style: 'mapbox://styles/mapbox/streets-v11',
    center: [ -117.073812,32.778647],
    zoom: 15
});

map.on('load', function () {

    map.addLayer({
        "id": "route",
        "type": "line",
        "source": {
            "type": "geojson",
            "data": {
                "type": "Feature",
                "properties": {},
                "geometry": {
                    "type": "LineString",
                    "coordinates": [
                        [-117.073230,32.778938],
                        [ -117.074297,32.778976]
                    ]
                }
            }
        },
        "layout": {
            "line-join": "round",
            "line-cap": "round"
        },
        "paint": {
            "line-color": "#33FF9A",
            "line-width": 12
        }
    });
});
</script>

</body>
</html>