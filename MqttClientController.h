/*
 * MqttClientController.h
 *
 *  Created on: 13.10.2016
 *      Author: nid
 */

#ifndef SRC_MQTTCLIENTCONTROLLER_H_
#define SRC_MQTTCLIENTCONTROLLER_H_

class PubSubClient;
class Timer;

const unsigned short int defaultMqttPort = 1883;

class MqttClientController
{
  friend class MqttClientCtrlReconnectTimerAdapter;

public:
  MqttClientController(Client* lanClient, const char* mqttServerAddr, unsigned short int mqttPort = defaultMqttPort);
  virtual ~MqttClientController();

  void setShallConnect(bool shallConnect);
  bool getShallConnect();

  void loop();

  int publish(const char* topic, const char* data);

protected:
  void connect();
  void reconnect();

private:
  PubSubClient* m_pubSubClient;
  Timer* m_reconnectTimer;
  bool m_isConnected;

private: // forbidden default functions
  MqttClientController();                                             // default constructor
  MqttClientController& operator = (const MqttClientController& src); // assignment operator
  MqttClientController(const MqttClientController& src);              // copy constructor
};


#endif /* SRC_MQTTCLIENTCONTROLLER_H_ */