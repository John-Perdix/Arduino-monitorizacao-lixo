import './App.css';
import React, { useState, useEffect } from 'react';
import MqttComponent from './MqttComponent';
import MapComponent from './mapComponent';
import client from './mqttService';
import iconEmpty from './assets/iconEmpty.png';

const LOCAL_STORAGE_KEY = 'markerData';

function App() {
  const [markerData, setMarkerData] = useState(() => {
    const savedData = localStorage.getItem(LOCAL_STORAGE_KEY);
    return savedData ? JSON.parse(savedData) : { lat: 40.186243201487606, lng: -8.414758649055049, status: "Empty", gas: "NotDetected" };
  });

  useEffect(() => {
    const handleMessage = (topic, message) => {
      const payload = JSON.parse(message.toString());
      if (topic === 'contentor/lotacao' && payload.lat && payload.lng && payload.msg && payload.gas) {
        const newMarkerData = { lat: payload.lat, lng: payload.lng, status: payload.msg, gas: payload.gas };
        setMarkerData(newMarkerData);
        localStorage.setItem(LOCAL_STORAGE_KEY, JSON.stringify(newMarkerData));
      }
    };

    client.on('message', handleMessage);

    client.subscribe('contentor/lotacao', { qos: 0 }, (error) => {
      if (error) {
        console.error('Subscribe error:', error);
      } else {
        console.log('Subscribed to topic');
      }
    });

    return () => {
      client.off('message', handleMessage);
      client.unsubscribe('contentor/lotacao');
    };
  }, []);

  return (
    <div className="App">
      <div className="navbar">
        <img src={iconEmpty} alt="Empty Icon" width="5%"></img>
        <h2>Central de Monitoramento do Lixo</h2>
      </div>
      <div className='map-parent'>
        <div className='container-mapa'>
          <MapComponent markerData={markerData} />
        </div>
        {/* <div className="App-header">
          <MqttComponent />
        </div> */}
      </div>
    </div>
  );
}

export default App;