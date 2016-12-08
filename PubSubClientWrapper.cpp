/*
 * PubSubClientWrapper.cpp
 *
 *  Created on: 18.10.2016
 *      Author: nid
 */

#include <DbgTracePort.h>
#include <DbgTraceLevel.h>
#include <PubSubClient.h>

#include <PubSubClientWrapper.h>

PubSubClientWrapper* PubSubClientWrapper::s_pubSubClientWrapper = 0;

PubSubClientWrapper::PubSubClientWrapper(Client& lanClient, const char* mqttServerAddr, unsigned short int mqttPort)
: m_pubSubClient(new PubSubClient(mqttServerAddr, mqttPort, lanClient))
, m_callbackAdapter(0)
{
  s_pubSubClientWrapper = this;
}

PubSubClientWrapper::~PubSubClientWrapper()
{ }

void pubSubClientCallback(char* topic, byte* payload, unsigned int length)
{
  if ((0 != PubSubClientWrapper::s_pubSubClientWrapper) && (0 != PubSubClientWrapper::s_pubSubClientWrapper->callbackAdapter()))
  {
    PubSubClientWrapper::s_pubSubClientWrapper->callbackAdapter()->messageReceived(topic, payload, length);
  }
}

void PubSubClientWrapper::setCallbackAdapter(IMqttClientCallbackAdapter* callbackAdapter)
{
  m_callbackAdapter = callbackAdapter;
  m_pubSubClient->setCallback(pubSubClientCallback);
}

IMqttClientCallbackAdapter* PubSubClientWrapper::callbackAdapter()
{
  return m_callbackAdapter;
}

bool PubSubClientWrapper::connect(const char* id)
{
  return m_pubSubClient->connect(id);
}

void PubSubClientWrapper::disconnect()
{
  m_pubSubClient->disconnect();
}

bool PubSubClientWrapper::connected()
{
  return m_pubSubClient->connected();
}

void PubSubClientWrapper::processMessages()
{
  m_pubSubClient->loop();
}

unsigned char PubSubClientWrapper::publish(const char* topic, const char* data)
{
  return m_pubSubClient->publish(topic, data);
}

unsigned char PubSubClientWrapper::subscribe(const char* topic)
{
  return m_pubSubClient->subscribe(topic);
}

unsigned char PubSubClientWrapper::unsubscribe(const char* topic)
{
  return m_pubSubClient->unsubscribe(topic);
}

IMqttClientWrapper::eIMqttClientState PubSubClientWrapper::state()
{
  int pubSubClientState = m_pubSubClient->state();
  eIMqttClientState iMqttClientState = eIMqttCS_Disconnected;

  switch (pubSubClientState)
  {
    case MQTT_CONNECTION_TIMEOUT       : iMqttClientState = IMqttClientWrapper::eIMqttCS_Connected             ; break;
    case MQTT_CONNECTION_LOST          : iMqttClientState = IMqttClientWrapper::eIMqttCS_ConnectBadProtocol    ; break;
    case MQTT_CONNECT_FAILED           : iMqttClientState = IMqttClientWrapper::eIMqttCS_ConnectBadClientId    ; break;
    case MQTT_DISCONNECTED             : iMqttClientState = IMqttClientWrapper::eIMqttCS_ConnectUnavailable    ; break;
    case MQTT_CONNECTED                : iMqttClientState = IMqttClientWrapper::eIMqttCS_ConnectBadCredentials ; break;
    case MQTT_CONNECT_BAD_PROTOCOL     : iMqttClientState = IMqttClientWrapper::eIMqttCS_ConnectUnauthorized   ; break;
    case MQTT_CONNECT_BAD_CLIENT_ID    : iMqttClientState = IMqttClientWrapper::eIMqttCS_ConnectionTimeout     ; break;
    case MQTT_CONNECT_UNAVAILABLE      : iMqttClientState = IMqttClientWrapper::eIMqttCS_ConnectionLost        ; break;
    case MQTT_CONNECT_BAD_CREDENTIALS  : iMqttClientState = IMqttClientWrapper::eIMqttCS_ConnectFailed         ; break;
    case MQTT_CONNECT_UNAUTHORIZED     : iMqttClientState = IMqttClientWrapper::eIMqttCS_Disconnected          ; break;
    default: break;
  }

  return iMqttClientState;
};

//-----------------------------------------------------------------------------

PubSubClientCallbackAdapter::PubSubClientCallbackAdapter()
: m_trPort(new DbgTrace_Port("mqttrx", DbgTrace_Level::info))
{ }

PubSubClientCallbackAdapter::~PubSubClientCallbackAdapter()
{
  delete m_trPort;
  m_trPort = 0;
}

void PubSubClientCallbackAdapter::messageReceived(char* topic, byte* payload, unsigned int length)
{
  char msg[length+1];
  memcpy(msg, payload, length);
  msg[length] = 0;

  TR_PRINT_STR(m_trPort, DbgTrace_Level::debug, "Message arrived, topic:");
  TR_PRINT_STR(m_trPort, DbgTrace_Level::debug, topic);
  TR_PRINT_STR(m_trPort, DbgTrace_Level::debug, msg);


  // TODO: remove this application specific code fragment below:
  bool pinState = atoi(msg);
  digitalWrite(BUILTIN_LED, pinState);
}
