import mqtt from 'mqtt';

const MQTT_URL = 'wss://y29efdb1.ala.us-east-1.emqxsl.com:8084/mqtt';
const MQTT_OPTIONS = {
    clientId: 'react_mqtt_client_' + Math.random().toString(16).substr(2, 8),
    username: 'arduino',
    password: 'arduino1234',
    keepalive: 60,
    clean: true,
    reconnectPeriod: 1000,
    connectTimeout: 30 * 1000,
    will: {
        topic: 'contentor/lotacao',
        payload: 'Connection Closed abnormally..!',
        qos: 0,
        retain: false
    },
    rejectUnauthorized: false
};

const client = mqtt.connect(MQTT_URL, MQTT_OPTIONS);

client.on('connect', () => {
    console.log('Connected to MQTT Broker');
});

client.on('error', (err) => {
    console.error('Connection error: ', err);
    client.end();
});

client.on('reconnect', () => {
    console.log('Reconnecting...');
});

client.on('close', () => {
    console.log('Connection closed');
});

client.on('offline', () => {
    console.log('Client went offline');
});

export default client;