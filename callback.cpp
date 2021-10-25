//
// Created by kvitadmin on 24.10.2021.
//
#include "callback.h"


void callback::reconnect() {
    std::this_thread::sleep_for(std::chrono::milliseconds(2500));
    try {
        cli_.connect(connOpts_, nullptr, *this);
    }
    catch (const mqtt::exception& exc) {
        std::cerr << "Error: " << exc.what() << std::endl;
        exit(1);
    }
}

void callback::on_failure(const mqtt::token& tok){
    std::cout << "Connection attempt failed" << std::endl;
    if (++nretry_ > N_RETRY_ATTEMPTS)
        exit(1);
    reconnect();
}

void callback::connected(const std::string& cause){
    std::cout << "\nConnection success" << std::endl;
    std::cout << "\nSubscribing to topic '" << TOPIC_S << "'\n"
    << "\tfor client " << CLIENT_ID
    << " using QoS" << QOS << "\n"
    << "\nPress Q<Enter> to quit\n" << std::endl;
     cli_.subscribe(TOPIC_S, QOS, nullptr, subListener_);
}

void callback::connection_lost(const std::string& cause){
    std::cout << "\nConnection lost" << std::endl;
    if (!cause.empty())
        std::cout << "\tcause: " << cause << std::endl;

    std::cout << "Reconnecting..." << std::endl;
    nretry_ = 0;
    reconnect();
}

void callback::message_arrived(mqtt::const_message_ptr msg){

    std::cout << "Message arrived" << std::endl;
    std::cout << "\ttopic: '" << msg->get_topic() << "'" << std::endl;
    std::cout << "\tpayload: '" << msg->to_string() << "'\n" << std::endl;
    std::string message = msg->to_string();
    int row_length = message_translate(message);
    if(row_length){
       int rsp_length = SendMessageToModbus(row_length);
       std::string response;
       if(rsp_length > 0) response = ModbusToString(rsp_length);
       else response = "ERROR";
       if(!response.empty()){
           mqtt::message_ptr pubmsg = mqtt::make_message(TOPIC_A, response);
           pubmsg->set_qos(QOS);
           cli_.publish(pubmsg)->wait_for(std::chrono::seconds(2));
       }
    }

}

std::string callback::ModbusToString(int resp_length){
    std::string resp;
    for(int i = 0; i < resp_length; ++i){
        resp = resp + std::string(1,HexToAscii((rsp[i] >> 4) & 0x0F)) + std::string(1, HexToAscii(rsp[i] & 0x0F));
    }
    return resp;
}
char callback::HexToAscii(char hex_char){
    if(hex_char >= 0 && hex_char <= 9) hex_char += 48;
    else if(hex_char >= 0xA && hex_char <= 0x0F) hex_char += 55;
    return hex_char;
}

uint8_t callback::AsciiToHex(char ascii_char){
    if(ascii_char >= 48 && ascii_char <= 57) ascii_char -= 48;
    else if(ascii_char >= 65 && ascii_char <= 70) ascii_char -= 55;
    return ascii_char;
}

char callback::message_translate(std::string& message){
    char k = 0;
    for(int i = 0; i < message.size(); i+=2){
        raw_req[k] = ((AsciiToHex(message.at(i)) << 4) & 0xF0) | (AsciiToHex(message.at(i+1)) & 0x0F);
        k++;
    }
    return k;

}
bool callback::ConnectToPort(){
    m_modbus = modbus_new_rtu("/dev/ttyUSB0", 9600, 'N',8, 1);
    modbus_set_debug(m_modbus, TRUE);
    //modbus_set_response_timeout(m_modbus, 1, 0);
    int portState = modbus_connect(m_modbus);
    m_portState = !portState;
    return m_portState;
}
void callback::DisconnectFromPort(){
    if(m_portState)
    {
        modbus_close(m_modbus);
        modbus_free(m_modbus);
        m_portState = false;
    }
}

int callback::SendMessageToModbus(int row_length){
    if(ConnectToPort()){
        int req_length = modbus_send_raw_request(m_modbus, raw_req, row_length * sizeof(uint8_t));
        if(req_length){
            int rsp_length = modbus_receive_confirmation(m_modbus, rsp);
            DisconnectFromPort();
            return rsp_length;
        }
    }
    return  -1;
}

callback::~callback(){
    if(m_portState) {
        DisconnectFromPort();
        modbus_free(m_modbus);
    }
}