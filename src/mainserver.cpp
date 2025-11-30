#include "mainserver.h"
#include <WiFi.h>
#include <WebServer.h>

bool led1_state = false;
bool led2_state = false;
bool isAPMode = true;

WebServer server(80);

unsigned long connect_start_ms = 0;
bool connecting = false;
bool connected = false;

String mainPage()
{
  float temperature = glob_temperature;
  float humidity = glob_humidity;
  float result = glob_result; 
  String led1 = led1_state ? "ON" : "OFF";
  String led2 = led2_state ? "ON" : "OFF";

  return R"rawliteral(
  <!DOCTYPE html>
  <html lang="vi">
  <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Dashboard</title>
    <style>
      body {
        font-family: "Segoe UI", Arial, sans-serif;
        background: #0d0d0d; /* Nền đen */
        color: #e0e0e0;      /* Chữ xám trắng */
        text-align: center;
        margin: 0;
        height: 100vh;
        display: flex;
        flex-direction: column;
        align-items: center;
        justify-content: center;
      }

      .container {
        /* Gradient Tím đen */
        background: linear-gradient(135deg, #1f0028, #3a0ca3); 
        padding: 35px 45px;
        border-radius: 20px;
        box-shadow: 0 0 20px rgba(138, 43, 226, 0.4); /* Đổ bóng tím */
        border: 1px solid #4a148c;
        width: 90%;
        max-width: 400px;
        backdrop-filter: blur(10px);
      }

      h1 {
        font-size: 1.8em;
        margin-bottom: 10px;
        color: #fff;
        text-shadow: 0 0 10px #bf5af2;
      }

      /* Style cho đồng hồ */
      .clock {
        font-size: 1.5em;
        font-weight: bold;
        color: #e0aaff;
        margin-bottom: 25px;
        font-family: monospace;
        letter-spacing: 2px;
      }

      .sensor {
        font-size: 1.1em;
        margin: 15px 0;
        background: rgba(0, 0, 0, 0.3);
        padding: 10px;
        border-radius: 10px;
      }

      .sensor span {
        font-weight: bold;
        color: #d0d0ff; /* Màu tím nhạt cho số liệu */
      }
      
      /* Style riêng cho Result để nổi bật hơn nếu cần */
      .result {
          font-size: 1.2em;
          margin: 15px 0 25px 0;
          background: #3a0ca3; /* Nền tím đậm */
          padding: 12px;
          border-radius: 10px;
          border: 1px solid #7c4dff;
      }
      
      .result span {
          font-weight: bold;
          color: #00ffcc; /* Màu xanh neon nổi bật */
      }

      button {
        margin: 10px;
        background: #6200ea; /* Nút màu tím đậm */
        color: #fff;
        font-weight: bold;
        border: none;
        border-radius: 20px;
        padding: 12px 25px;
        cursor: pointer;
        transition: all 0.3s;
        font-size: 1em;
        box-shadow: 0 4px 6px rgba(0,0,0,0.3);
      }

      button:hover {
        background: #7c4dff; /* Sáng hơn khi hover */
        transform: scale(1.05);
        box-shadow: 0 0 15px #7c4dff;
      }

      #settings {
        background: transparent;
        border: 2px solid #bb86fc;
        color: #bb86fc;
        margin-top: 20px;
      }

      #settings:hover {
        background: #bb86fc;
        color: #000;
      }
    </style>
  </head>

  <body onload="startTime()">
    <div class="container">
      <h1>📊 ESP32 Dashboard</h1>
      
      <div id="clock" class="clock">--:--:--</div>

      <div class="sensor">
        🌡️ Nhiệt độ: <span id="temp">)rawliteral" +
          String(temperature) + R"rawliteral(</span> &deg;C
      </div>
      <div class="sensor">
        💧 Độ ẩm: <span id="hum">)rawliteral" +
          String(humidity) + R"rawliteral(</span> %
      </div>
      
      <div class="result">
        🤖 Inference Result: <span id="res">)rawliteral" +
          String(result) + R"rawliteral(</span>
      </div>

      <div>
        <button onclick='toggleLED(1)'>💡 LED1: <span id="l1">)rawliteral" +
          led1 + R"rawliteral(</span></button>
        <button onclick='toggleLED(2)'>💡 LED2: <span id="l2">)rawliteral" +
          led2 + R"rawliteral(</span></button>
      </div>

      <button id="settings" onclick="window.location='/settings'">⚙️ Cài đặt</button>
    </div>

    <script>
      // Hàm hiển thị đồng hồ thời gian thực
      function startTime() {
        const today = new Date();
        let h = today.getHours();
        let m = today.getMinutes();
        let s = today.getSeconds();
        m = checkTime(m);
        s = checkTime(s);
        document.getElementById('clock').innerHTML =  h + ":" + m + ":" + s;
        setTimeout(startTime, 1000);
      }

      function checkTime(i) {
        if (i < 10) {i = "0" + i};  // Thêm số 0 trước các số nhỏ hơn 10
        return i;
      }

      function toggleLED(id) {
        fetch('/toggle?led='+id)
          .then(response=>response.json())
          .then(json=>{
            document.getElementById('l1').innerText=json.led1;
            document.getElementById('l2').innerText=json.led2;
          });
      }

      setInterval(()=>{
        fetch('/sensors')
          .then(res=>res.json())
          .then(d=>{
            document.getElementById('temp').innerText=d.temp;
            document.getElementById('hum').innerText=d.hum;
            // Giả định API /sensors cũng trả về d.result
            if (d.result !== undefined) { 
                document.getElementById('res').innerText=d.result; 
            }
          });
      },3000);
    </script>
  </body>
  </html>
  )rawliteral";
}

String settingsPage()
{
  return R"rawliteral(
  <!DOCTYPE html>
  <html lang="vi">
  <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Wi-Fi Settings</title>
    <style>
      body {
        font-family: "Segoe UI", Arial, sans-serif;
        background: #0d0d0d; /* Nền đen */
        color: #e0e0e0;      /* Chữ trắng xám */
        text-align: center;
        margin: 0;
        height: 100vh;
        display: flex;
        flex-direction: column;
        align-items: center;
        justify-content: center;
      }

      .container {
        /* Gradient Tím đen giống Main Page */
        background: linear-gradient(135deg, #1f0028, #3a0ca3);
        padding: 35px 45px;
        border-radius: 20px;
        box-shadow: 0 0 20px rgba(138, 43, 226, 0.4); /* Đổ bóng tím neon */
        border: 1px solid #4a148c;
        width: 90%;
        max-width: 400px;
        backdrop-filter: blur(10px);
      }

      h1 {
        font-size: 1.8em;
        margin-bottom: 25px;
        color: #fff;
        text-shadow: 0 0 10px #bf5af2;
      }

      /* Style lại Input cho nền tối */
      input[type=text], input[type=password] {
        width: 100%;
        padding: 12px;
        border: 1px solid #7c4dff; /* Viền tím */
        background: rgba(255, 255, 255, 0.1); /* Nền trong suốt nhẹ */
        color: #fff; /* Chữ khi nhập màu trắng */
        border-radius: 10px;
        font-size: 1em;
        box-sizing: border-box;
        margin-bottom: 20px;
        outline: none;
        transition: border 0.3s;
      }

      input::placeholder {
        color: #aaa; /* Màu chữ gợi ý */
      }

      input:focus {
        border-color: #d0d0ff;
        box-shadow: 0 0 8px rgba(124, 77, 255, 0.5);
      }

      button {
        margin-top: 10px;
        background: #6200ea; /* Tím đậm */
        color: #fff;
        font-weight: bold;
        border: none;
        border-radius: 25px;
        padding: 12px 25px;
        cursor: pointer;
        transition: all 0.3s;
        font-size: 1em;
        width: 100%; /* Nút full chiều ngang cho đẹp trên mobile */
        box-shadow: 0 4px 6px rgba(0,0,0,0.3);
      }

      button:hover {
        background: #7c4dff;
        transform: scale(1.02);
        box-shadow: 0 0 15px #7c4dff;
      }

      /* Nút Quay lại dạng Outline (trong suốt có viền) */
      #back {
        background: transparent;
        border: 2px solid #bb86fc;
        color: #bb86fc;
        margin-top: 15px;
      }

      #back:hover {
        background: #bb86fc;
        color: #000;
      }

      #msg {
        margin-top: 20px;
        font-weight: 500;
        color: #00e676; /* Màu xanh lá thông báo thành công */
        min-height: 1.2em;
      }
    </style>
  </head>

  <body>
    <div class="container">
      <h1>⚙️ Cấu hình Wi-Fi</h1>
      <form id="wifiForm">
        <input name="ssid" id="ssid" type="text" placeholder="Tên Wi-Fi (SSID)" required>
        <input name="password" id="pass" type="password" placeholder="Mật khẩu (bỏ trống nếu không có)">
        
        <button type="submit">Kết nối</button>
        <button type="button" id="back" onclick="window.location='/'">Quay lại</button>
      </form>
      <div id="msg"></div>
    </div>

    <script>
      document.getElementById('wifiForm').onsubmit = function(e){
        e.preventDefault();
        // Hiển thị trạng thái đang xử lý
        document.getElementById('msg').style.color = "#e0aaff";
        document.getElementById('msg').innerText = "Đang gửi yêu cầu kết nối...";

        let ssid = document.getElementById('ssid').value;
        let pass = document.getElementById('pass').value;
        fetch('/connect?ssid='+encodeURIComponent(ssid)+'&pass='+encodeURIComponent(pass))
          .then(r=>r.text())
          .then(msg=>{
            // Hiển thị thông báo kết quả (từ server) trong div #msg
            document.getElementById('msg').style.color = "#00e676"; 
            document.getElementById('msg').innerText = msg;
            
            // === PHẦN BỔ SUNG YÊU CẦU ===
            // Hiển thị alert sau khi nhận được phản hồi thành công từ server
            alert("✅ Yêu cầu kết nối Wi-Fi đã được gửi thành công!");
            // ============================
          })
          .catch(err => {
            // Xử lý lỗi fetch (thường là lỗi mất kết nối khi ESP32 restart)
             document.getElementById('msg').style.color = "#ff5252"; 
             document.getElementById('msg').innerText = "Lỗi kết nối hoặc thiết bị đang chuyển đổi chế độ mạng.";
          });
      };
    </script>
  </body>
  </html>
  )rawliteral";
}

// ========== Handlers ==========
void handleRoot() { server.send(200, "text/html", mainPage()); }

void handleToggle()
{
  int led = server.arg("led").toInt();
  if (led == 1)
  {
    led1_state = !led1_state;
    //Serial.println("YOUR CODE TO CONTROL LED1");
    if(led1_state){
      Serial.println("LED1 ON");
      xTaskCreate(turn_on_led1, "Task Turn On LED1" ,2048  ,NULL  ,2 , NULL);
    }
    else{
      Serial.println("LED1 OFF");
      xTaskCreate(turn_off_led, "Task Turn Off LED1" ,2048  ,NULL  ,2 , NULL);
      led1_state = 0;
      led2_state = 0;
    }
  }
  else if (led == 2)
  {
    led2_state = !led2_state;
    //Serial.println("YOUR CODE TO CONTROL LED2");
    if(led2_state){
      Serial.println("LED2 ON");
      xTaskCreate(turn_on_led2, "Task Turn On LED2" ,2048  ,NULL  ,2 , NULL);
    }
    else{
      Serial.println("LED2 OFF");
      xTaskCreate(turn_off_led, "Task Turn Off LED2" ,2048  ,NULL  ,2 , NULL);
      led1_state = 0;
      led2_state = 0;
    }
  }
  server.send(200, "application/json",
              "{\"led1\":\"" + String(led1_state ? "ON" : "OFF") +
                  "\",\"led2\":\"" + String(led2_state ? "ON" : "OFF") + "\"}");
}

void handleSensors()
{
  float t = glob_temperature;
  float h = glob_humidity;
  String json = "{\"temp\":" + String(t) + ",\"hum\":" + String(h) + "}";
  server.send(200, "application/json", json);
}

void handleSettings() { server.send(200, "text/html", settingsPage()); }

void handleConnect()
{
  wifi_ssid = server.arg("ssid");
  wifi_password = server.arg("pass");
  server.send(200, "text/plain", "Connecting....");
  //isAPMode = false;
  connecting = true;
  connect_start_ms = millis();
  //connectToWiFi();

  WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());

  Serial.print("Connecting to: ");
  Serial.print(wifi_ssid.c_str());

  Serial.print(" Password: ");
  Serial.println(wifi_password.c_str());

  Serial.println("Connected successfully");
  //server.send(200, "text/plain", "Connected");
}


// ========== WiFi ==========
void setupServer()
{
  server.on("/", HTTP_GET, handleRoot);
  server.on("/toggle", HTTP_GET, handleToggle);
  server.on("/sensors", HTTP_GET, handleSensors);
  server.on("/settings", HTTP_GET, handleSettings);
  server.on("/connect", HTTP_GET, handleConnect);
  server.begin();
}

void startAP()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid.c_str(), password.c_str());
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  isAPMode = true;
  connecting = false;
}

void connectToWiFi()
{
  // WiFi.mode(WIFI_STA);
  // if (wifi_password.isEmpty())
  // {
  //   WiFi.begin(wifi_ssid.c_str());
  // }
  // else
  // {
  //   WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());
  // }

  // WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());

  // Serial.print("Connecting to: ");
  // Serial.print(wifi_ssid.c_str());

  // Serial.print(" Password: ");
  // Serial.print(wifi_password.c_str());
}

// ========== Main task ==========
void main_server_task(void *pvParameters)
{
  pinMode(BOOT_PIN, INPUT_PULLUP);

  startAP();
  setupServer();

  while (1)
  {
    server.handleClient();

    // BOOT Button to switch to AP Mode
    if (digitalRead(BOOT_PIN) == LOW)
    {
      vTaskDelay(100);
      if (digitalRead(BOOT_PIN) == LOW)
      {
        if (!isAPMode)
        {
          startAP();
          setupServer();
        }
      }
    }

    // STA Mode
    if (connecting)
    {
      if (WiFi.status() == WL_CONNECTED)
      {
        if(!connected){
          xTaskCreate(neo_wifi_connected, "Task NEO WiFi Connected" ,2048  ,NULL  ,2 , NULL);
          connected = true;
        }
        else if (connected && connecting){
            xTaskCreate(neo_wifi_disconnected, "Task NEO WiFi Disconnected" ,2048  ,NULL  ,2 , NULL);
        }
        Serial.print("STA IP address: ");
        Serial.println(WiFi.localIP());
        isWifiConnected = true; // Internet access

        xSemaphoreGive(xBinarySemaphoreInternet);

        //isAPMode = false;
        connecting = false;
      }
      else if (millis() - connect_start_ms > 10000)
      { // timeout 10s
        Serial.println("WiFi connect failed! Back to AP.");
        //startAP();
        setupServer();
        connecting = false;
        isWifiConnected = false;
        connected = false;
      }
    }

    vTaskDelay(20); // avoid watchdog reset
  }
}