//
// Created by kvitadmin on 24.10.2021.
//

#ifndef TESTMQTTCPP_ACTION_LISTENER_H
#define TESTMQTTCPP_ACTION_LISTENER_H
#include "mqtt/async_client.h"

// Callbacks for the success or failures of requested actions.
// This could be used to initiate further action, but here we just log the
// results to the console.

class action_listener : public virtual mqtt::iaction_listener
        {
    std::string name_;

    void on_failure(const mqtt::token& tok) override;

    void on_success(const mqtt::token& tok) override;

        public:
            action_listener(const std::string& name) : name_(name) {}
        };



#endif //TESTMQTTCPP_ACTION_LISTENER_H
