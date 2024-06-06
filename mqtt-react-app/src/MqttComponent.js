import React, { useEffect, useState } from 'react';
import client from './mqttService';

const MqttComponent = () => {
    const [messages, setMessages] = useState([]);
    /*  const [inputValue, setInputValue] = useState(''); */
    const topic = "contentor/lotacao";

    useEffect(() => {
        const handleMessage = (topic, message) => {
            const payload = message.toString();
            const msg = { topic, payload };
            setMessages((prevMessages) => [...prevMessages, msg ]);
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
        <div className='chat-container'>
            <h1>MQTT Messages</h1>
            <ul className='chat-messages'>
                {messages.map((msg, index) => (
                    <li key={index} className='chat-message'>
                        <div className='chat-bubble'>
                            <strong>{msg.topic}:</strong> {msg.payload}
                        </div>
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