#include "library.h"
#include "wifiManager.h"


volatile bool wifiConnected = false;

static EventGroupHandle_t s_myWifiEventGroup;
#define WIFI_CONNECTED_BIT BIT0
static const char *TAG = "WIFI_MANAGER";

static void myWifiEventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
static void myWifiInitSta(void);
static void myWifiConnectionTask(void *pvParameter);

static void myWifiEventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
    esp_wifi_connect();
  } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
    ESP_LOGW(TAG, "Conexión WiFi perdida. Reintentando...");
    wifiConnected = false;
    esp_wifi_connect();
    xEventGroupClearBits(s_myWifiEventGroup, WIFI_CONNECTED_BIT);
  } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
    ESP_LOGI(TAG, "¡WiFi Conectado! IP: " IPSTR, IP2STR(&event->ip_info.ip));
    wifiConnected = true;
    xEventGroupSetBits(s_myWifiEventGroup, WIFI_CONNECTED_BIT);
  }
}

static void myWifiInitSta(void) {
  s_myWifiEventGroup = xEventGroupCreate();

  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &myWifiEventHandler, NULL, NULL));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &myWifiEventHandler, NULL, NULL));

  wifi_config_t wifi_config = {
    .sta = {
      .ssid = WIFI_SSID,
      .password = WIFI_PASSWORD,
      .threshold.authmode = WIFI_AUTH_WPA2_PSK,
    },
  };
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  xEventGroupWaitBits(s_myWifiEventGroup, WIFI_CONNECTED_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
}

static void myWifiConnectionTask(void *pvParameter) {
  ESP_LOGI(TAG, "Tarea de conexión WiFi iniciada.");
  myWifiInitSta();

  ESP_LOGI(TAG, "Inicialización finalizada. El dispositivo está online.");
    
  vTaskDelete(NULL);
}

void myWifiStartConnection(void) {
    esp_log_level_set("wifi", ESP_LOG_WARN);
    esp_log_level_set("esp_netif_sta", ESP_LOG_WARN);

    xTaskCreate(&myWifiConnectionTask, "wifi_connection_task", 4096, NULL, 5, NULL);
}