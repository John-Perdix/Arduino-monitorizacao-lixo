import './App.css';
import React from 'react';
import MqttComponent from './MqttComponent';

function App() {
  return (
    <div className="App">
      <header className="App-header">
        <h1>React MQTT Example</h1>
        <MqttComponent />
      </header>
    </div>
  );
}

export default App;
