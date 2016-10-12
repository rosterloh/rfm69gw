//------------------------------------------------------------------------------
// SET BY PLATFORMIO
//------------------------------------------------------------------------------

//#define DEBUG_PORT              Serial

// -----------------------------------------------------------------------------
// HARDWARE
// -----------------------------------------------------------------------------

#define SERIAL_BAUDRATE         115200
#define LED_PIN                 4

//------------------------------------------------------------------------------
// RADIO
//------------------------------------------------------------------------------

#define NODEID                  1
#define GATEWAYID               1
#define NETWORKID               164
#define PROMISCUOUS             0
#define FREQUENCY               RF69_433MHZ
#define ENCRYPTKEY              "fibonacci0123456"
#define SPI_CS                  SS
#define IRQ_PIN                 5
#define IS_RFM69HW              0

// -----------------------------------------------------------------------------
// WIFI
// -----------------------------------------------------------------------------

#define WIFI_RECONNECT_INTERVAL 300000
#define WIFI_MAX_NETWORKS       3
#define AP_PASS                 "fibonacci"
#define OTA_PASS                "fibonacci"
#define OTA_PORT                8266

// -----------------------------------------------------------------------------
// MQTT
// -----------------------------------------------------------------------------

#define MQTT_SERVER             "192.168.1.100"
#define MQTT_PORT               1883
#define MQTT_RETAIN             true
#define MQTT_RECONNECT_DELAY    10000
#define MQTT_USER               ""
#define MQTT_PASS               ""
#define MQTT_IP_TOPIC           "/raw/rfm69gw/ip"
#define MQTT_HEARTBEAT_TOPIC    "/raw/rfm69gw/ping"
#define MQTT_DEFAULT_TOPIC      "/raw/rfm69/{nodeid}/{key}"

// -----------------------------------------------------------------------------
// NTP
// -----------------------------------------------------------------------------

#define NTP_SERVER              "pool.ntp.org"
#define NTP_TIME_OFFSET         1
#define NTP_DAY_LIGHT           true
#define NTP_UPDATE_INTERVAL     1800

// -----------------------------------------------------------------------------
// DEFAULTS
// -----------------------------------------------------------------------------

#define HEARTBEAT_INTERVAL      60000
#define HOSTNAME                APP_NAME
#define DEVICE                  APP_NAME
