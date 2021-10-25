#include <iostream>
//#include <cstdlib>
#include <string>
#include "callback.h"
//#include "action_listener.h"
#include <cctype>
//#include <thread>
#include <chrono>
#include "mqtt/async_client.h"
//#include <unistd.h>

const std::string SERVER_ADDRESS("tcp://194.8.130.147:1883");
const std::string CLIENT_ID("paho_cpp_async_subcribe");
const std::string TOPIC_S("TestTopic/question");
const std::string TOPIC_A("TestTopic/answer");
const std::string USERNAME("kvit");
const std::string PASSWORD("20281174");

const int	QOS = 1;
const int	N_RETRY_ATTEMPTS = 5;

const std::string SERVER_ADDRESS_1("dev.rightech.io");
const std::string CLIENT_ID_1("kvit_vn-9dzzqn");
const std::string TOPIC_1("base/state/temperature");
const std::string TOPIC_2("base/state/humidity");
const auto TIMEOUT = std::chrono::seconds(10);
int ready =0;
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////



int main(int argc, char* argv[])
{
    // A subscriber often wants the server to remember its messages when its
    // disconnected. In that case, it needs a unique ClientID and a
    // non-clean session.

    mqtt::async_client cli(SERVER_ADDRESS, CLIENT_ID);

    mqtt::connect_options connOpts;
    connOpts.set_user_name(USERNAME);
    connOpts.set_password(PASSWORD);
    connOpts.set_clean_session(false);

    // Install the callback(s) before connecting.
    callback cb(cli, connOpts);
    cli.set_callback(cb);

    // Start the connection.
    // When completed, the callback will subscribe to topic.

    try {
        std::cout << "Connecting to the MQTT server..." << std::flush;
        cli.connect(connOpts, nullptr, cb);
    }
    catch (const mqtt::exception& exc) {
        std::cerr << "\nERROR: Unable to connect to MQTT server: '"
        << SERVER_ADDRESS << "'" << exc << std::endl;
        return 1;
    }

    // Just block till user tells us to quit.

    /*std::cout << "\nSending message humidity..." << std::endl;
    mqtt::message_ptr pubmsg = mqtt::make_message(TOPIC_2, "43");
    pubmsg->set_qos(QOS);
    cli.publish(pubmsg)->wait_for(TIMEOUT);
    std::cout << "  ...OK" << std::endl;
    sleep(5);
    std::cout << "\nSending message temperature..." << std::endl;
    pubmsg = mqtt::make_message(TOPIC_1, "24");
    pubmsg->set_qos(QOS);
    cli.publish(pubmsg)->wait_for(TIMEOUT);
    std::cout << "  ...OK" << std::endl;
    sleep(5);*/
    while (std::tolower(std::cin.get()) != 'q');
    // Disconnect
    try {
        std::cout << "\nUnsubscribing from topic " << TOPIC_S << std::flush;
        //cli.unsubscribe("seeds/outHum");
        cli.unsubscribe(TOPIC_S);
        std::cout << "\nDisconnecting from the MQTT server..." << std::flush;
        cli.disconnect()->wait();
        std::cout << "OK" << std::endl;
    }
    catch (const mqtt::exception& exc) {
        std::cerr << exc << std::endl;
        return 1;
    }

    return 0;
}
