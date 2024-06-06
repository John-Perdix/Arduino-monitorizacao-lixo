import React, { useEffect, useState } from 'react';
import client from './mqttService';

const MqttComponent = () => {
    const [messages, setMessages] = useState([]);
    const topic = "contentor/lotacao";

    useEffect(() => {
        const handleMessage = (topic, message) => {
            const payload = message.toString();
            const msg = { topic, payload };
            setMessages((prevMessages) => [...prevMessages, msg ]);
            console.log(`Message received: ${topic} - ${payload}`);
        };

        client.on('message', handleMessage);

        // Subscreve ao topico
        client.subscribe(topic, { qos: 0 }, (error) => {
            if (error) {
                console.error('Subscribe error:', error);
            } else {
                console.log('Subscribed to topic:' + topic);
            }
        });

        return () => {
            client.off('message', handleMessage);
            client.unsubscribe(topic);
        };
    }, []);

    return (
        <div className='chat-container'>
            <ul className='chat-messages'>
                {messages.map((msg, index) => (
                    <li key={index} className='chat-message'>
                        <div className='chat-bubble'>
                            <strong>{msg.topic}:</strong> {msg.payload}
                        </div>
                    </li>
                ))}
            </ul>
        </div>
    );
};

export default MqttComponent;