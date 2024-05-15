// src/MqttComponent.js
import React, { useEffect, useState } from 'react';
import client from './mqttService';

const MqttComponent = () => {
    const [messages, setMessages] = useState([]);
   /*  const [inputValue, setInputValue] = useState(''); */
    const topic = "contentor/lotacao";

    useEffect(() => {
        const handleMessage = (topic, message) => {
            const payload = message.toString();
            setMessages((prevMessages) => [...prevMessages, { topic, payload }]);
            console.log(`Message received: ${topic} - ${payload}`);
        };

        client.on('message', handleMessage);

        // Subscribe to a topic
        client.subscribe(topic, { qos: 0 }, (error) => {
            if (error) {
                console.error('Subscribe error:', error);
            } else {
                console.log('Subscribed to topic:' + topic);
            }
        });

        // Cleanup on unmount
        return () => {
            client.off('message', handleMessage);
            client.unsubscribe(topic);
        };
    }, []);

    /*const handlePublish = () => {
        if (inputValue) {
            client.publish(topic, inputValue, (err) => {
                if (err) {
                    console.error('Publish error:', err);
                } else {
                    console.log('Message published');
                }
            });
            setInputValue('');
        }
    };*/

    return (
        <div>
            <h2>MQTT Messages</h2>
            <ul>
                {messages.map((msg, index) => (
                    <li key={index}>
                        <strong>{msg.topic}:</strong> {msg.payload}
                    </li>
                ))}
            </ul>
            {/* <input
                type="text"
                value={inputValue}
                onChange={(e) => setInputValue(e.target.value)}
            />
            <button onClick={handlePublish}>Publish</button> */}
        </div>
    );
};

export default MqttComponent;