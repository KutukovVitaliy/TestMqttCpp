//
// Created by kvitadmin on 24.10.2021.
//

#ifndef TESTMQTTCPP_CALLBACK_H
#define TESTMQTTCPP_CALLBACK_H

#include "mqtt/async_client.h"
#include "action_listener.h"
#include "modbus/modbus.h"
#include <chrono>

extern const std::string CLIENT_ID;
extern const std::string TOPIC_S;
extern const std::string TOPIC_A;
extern const int N_RETRY_ATTEMPTS;
extern const int QOS;

/**
 * Local callback & listener class for use with the client connection.
 * This is primarily intended to receive messages, but it will also monitor
 * the connection to the broker. If the connection is lost, it will attempt
 * to restore the connection and re-subscribe to the topic.
 */
class callback : public virtual mqtt::callback,
        public virtual mqtt::iaction_listener{
    //Arrays and variables for Modbus RTU
    modbus_t*   m_modbus = nullptr;
    bool        m_portState = false;
    uint8_t raw_req[256] = {0};
    //int req_length;
    uint8_t rsp[256] = {0};
    ///////////////////////////////////
    std::string ModbusToString(int resp_length);
    uint8_t AsciiToHex(char ascii_char);
    char HexToAscii(char hex_char);
    // Counter for the number of connection retries
    int nretry_;
    // The MQTT client
    mqtt::async_client& cli_;
    // Options to use if we need to reconnect
    mqtt::connect_options& connOpts_;
    // An action listener to display the result of actions.
    action_listener subListener_;

    // This deomonstrates manually reconnecting to the broker by calling
    // connect() again. This is a possibility for an application that keeps
    // a copy of it's original connect_options, or if the app wants to
    // reconnect with different options.
    // Another way this can be done manually, if using the same options, is
    // to just call the async_client::reconnect() method.
    void reconnect();

    // Re-connection failure
    void on_failure(const mqtt::token& tok) override;

    // (Re)connection success
    // Either this or connected() can be used for callbacks.
    void on_success(const mqtt::token& tok) override {}

    // (Re)connection success
    void connected(const std::string& cause) override;

    // Callback for when the connection is lost.
    // This will initiate the attempt to manually reconnect.
    void connection_lost(const std::string& cause) override;
    //This function translate message from MQTT string to MODBUS RTU message
    char message_translate(std::string& message);
    // Callback for when a message arrives.
    void message_arrived(mqtt::const_message_ptr msg) override;

    void delivery_complete(mqtt::delivery_token_ptr token) override {}
    bool ConnectToPort();
    void DisconnectFromPort();
    int SendMessageToModbus(int row_length);

        public:
            callback(mqtt::async_client& cli, mqtt::connect_options& connOpts)
            : nretry_(0), cli_(cli), connOpts_(connOpts), subListener_("Subscription") {}

            ~callback() override;
        };


/////////////////////////////////////////////////////////////////////////////
#endif //TESTMQTTCPP_CALLBACK_H
