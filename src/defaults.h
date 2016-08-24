#define APP_NAME                "RFM69 GW"
#define APP_VERSION             "0.1.2"

#define NODEID                  1
#define NETWORKID               164
#define FREQUENCY               RF69_868MHZ
#define ENCRYPTKEY              "fibonacci0123456"
#define IS_RFM69HW              0

#define HOSTNAME                "RFM69_GATEWAY"
#define AP_PASS                 "fibonacci"

#define OTA_PASS                "fibonacci"
#define OTA_PORT                8266

#define MQTT_RECONNECT_DELAY    5000
#define MQTT_RETAIN             true
#define MQTT_SERVER             "192.168.1.100"
#define MQTT_PORT               1883
#define MQTT_USER               ""
#define MQTT_PASS               ""
#define IP_TOPIC                "/raw/rfm69gw/ip"
#define HEARTBEAT_TOPIC         "/raw/rfm69gw/ping"
#define DEFAULT_TOPIC           "/raw/rfm69/{nodeid}/{key}"
#define HEARTBEAT_INTERVAL      60000

#define SERIAL_BAUD             115200
#define LED_PIN                 4
