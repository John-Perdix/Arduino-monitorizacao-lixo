import React, { useEffect, useState } from 'react';
import 'mapbox-gl/dist/mapbox-gl.css';
import '@mapbox/mapbox-gl-directions/dist/mapbox-gl-directions.css';
import mapboxgl from 'mapbox-gl';
import MapboxDirections from '@mapbox/mapbox-gl-directions/dist/mapbox-gl-directions';
import './App.css';
import iconEmpty from './assets/iconEmpty.png';
import iconHalfFull from './assets/iconHalfFull.png';
import iconFull from './assets/iconFull.png';
import iconEmptyGas from './assets/iconEmptyGas.png';
import iconHalfFullGas from './assets/iconHalfFullGas.png';
import iconFullGas from './assets/iconFullGas.png';

const MapComponent = ({ markerData }) => {
    const [map, setMap] = useState(null);
    const [marker, setMarker] = useState(null);
    //const [rendered, setRendered] = useState(false); // Track if component has been rendered
    const constantCoordinates = [-8.414758649055049, 40.186243201487606]; // Define constant coordinates 40.186243201487606, -8.414758649055049

    const iconMapping = {
        "Empty": iconEmpty,
        "Half-full": iconHalfFull,
        "Full": iconFull,
        "EmptyGas": iconEmptyGas,
        "Half-fullGas": iconHalfFullGas,
        "FullGas": iconFullGas
    };

    useEffect(() => {
        // Initialize map and marker only if they are not already set
        mapboxgl.accessToken = 'pk.eyJ1IjoiYW5hZmVycmVpcmEwNiIsImEiOiJjbHcwa3ZocDUwMmxuMmlwaDY0YWJwMmd3In0.Fl-VfouRqMFskxa6yUEu8w';

        function setupMap(center) {
            const map = new mapboxgl.Map({
                container: 'map',
                style: 'mapbox://styles/mapbox/streets-v12',
                center: center,
                zoom: 15
            });

            const nav = new mapboxgl.NavigationControl();
            map.addControl(nav);

            const directions = new MapboxDirections({
                accessToken: mapboxgl.accessToken
            });

            map.addControl(directions, 'top-left');

            const markerElement = document.createElement('div');
            //markerElement.style.backgroundImage = `url(${esp32Icon})`;
            markerElement.style.width = '30px';
            markerElement.style.height = '30px';
            markerElement.style.backgroundSize = '100%';

            const newMarker = new mapboxgl.Marker({
                element: markerElement,
                anchor: 'bottom'
            }).setLngLat(center).addTo(map);

            setMap(map);
            setMarker(newMarker);
        }
        setupMap(constantCoordinates);

    }, []);

    useEffect(() => {
        if (map && marker && markerData && markerData.lat && markerData.lng) {
            marker.setLngLat([markerData.lng, markerData.lat]);
            map.flyTo({
                center: [markerData.lng, markerData.lat],
                essential: true
            });

            // Update marker icon based on the status
            const markerElement = marker.getElement();
            const iconKey = markerData.gas === 'Detected' ? `${markerData.status}Gas` : markerData.status;
            const iconUrl = iconMapping[iconKey];
            if (iconUrl) {
                markerElement.style.backgroundImage = `url(${iconUrl})`;
            } else {
                console.error(`Icon URL not found for key: ${iconKey}`);
            }
        }
    }, [markerData, marker, map]);

    return (
        <div id="map" style={{ flex: 1 }}></div>
    );
};

export default MapComponent;
