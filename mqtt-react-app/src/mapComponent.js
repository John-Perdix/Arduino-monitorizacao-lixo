import React, { useEffect, useState } from 'react';
import 'mapbox-gl/dist/mapbox-gl.css';
import mapboxgl from 'mapbox-gl';
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
    const constantCoordinates = [-8.414758649055049, 40.186243201487606]; // Define constant coordinates

    const iconMapping = {
        "Empty": iconEmpty,
        "Half-full": iconHalfFull,
        "Full": iconFull,
        "EmptyGas": iconEmptyGas,
        "Half-fullGas": iconHalfFullGas,
        "FullGas": iconFullGas
    };

    // Function to update favicon
    const updateFavicon = (iconUrl) => {
        const link = document.querySelector("link[rel~='icon']");
        if (!link) {
            const newLink = document.createElement('link');
            newLink.rel = 'icon';
            newLink.href = iconUrl;
            document.head.appendChild(newLink);
        } else {
            link.href = iconUrl;
        }
    };

    useEffect(() => {
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

            const markerElement = document.createElement('div');
            markerElement.style.width = '50px';
            markerElement.style.height = '50px';
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

            // Atualiza o icon baseando no status
            const markerElement = marker.getElement();
            const iconUrl = iconMapping[markerData.status + (markerData.gas === "Detected" ? "Gas" : "")];
            if (iconUrl) {
                markerElement.style.backgroundImage = `url(${iconUrl})`;
                // Update favicon
                updateFavicon(iconUrl);
            }

            // Adicionar popup ao icon
            const popup = new mapboxgl.Popup({ offset: 50 }).setHTML(
                `<div style="color: black;">
                    <strong>Nome:</strong> Lixo DEI 1<br/>
                    <strong>Lotação:</strong> ${markerData.status}<br/>
                    <strong>Qualidade de Ar:</strong> ${markerData.gas === "Detected" ? "Má" : "Boa"}
                </div>`
            );
            marker.setPopup(popup);
        }
    }, [markerData, marker, map]);

    return (
        <div id="map" style={{ flex: 1 }}></div>
    );
};

export default MapComponent;
