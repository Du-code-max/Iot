#define BLYNK_TEMPLATE_ID "TMPL6l56IRotU"
#define BLYNK_TEMPLATE_NAME "Smart Gas and Flow Notification System"
#define BLYNK_AUTH_TOKEN "-ZVOi8mWigo_m9qVRYmJjbh9Itf9cAMg"

#include <Arduino.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

// WiFi Credentials
char ssid[] = "Redmi K60E";
char pass[] = "99999999";

// Định nghĩa chân kết nối
#define DHT_PIN 4         // Cảm biến DHT11
#define MQ2_PIN 34        // Cảm biến khí gas MQ-2
#define LED_PIN 5         // LED cảnh báo
#define BUZZER_PIN 18     // Còi báo động
#define DHT_TYPE DHT11    // Kiểu cảm biến DHT11

// Ngưỡng cảnh báo
#define TEMP_THRESHOLD 33  // Ngưỡng nhiệt độ (°C)
#define GAS_THRESHOLD 1000 // Ngưỡng khí gas (ADC)

// Khởi tạo cảm biến DHT11
DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
    Serial.begin(115200);

    // Kết nối WiFi
    WiFi.begin(ssid, pass);
    Serial.print("🔗 Đang kết nối WiFi...");
    
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
    }

    Serial.println("\n✅ Kết nối WiFi thành công!");
    Serial.print("Địa chỉ IP: ");
    Serial.println(WiFi.localIP());

    // Kết nối Blynk
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

    // Cấu hình cảm biến và thiết bị ngoại vi
    dht.begin();
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(MQ2_PIN, INPUT);

    // Đảm bảo đèn & còi tắt khi khởi động
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);

    Serial.println("🚀 Hệ thống đo nhiệt độ & khí gas đã khởi động...");
}

void loop() {
    Blynk.run();  // Duy trì kết nối với Blynk

    // Đọc nhiệt độ & độ ẩm từ DHT11
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    
    // Đọc giá trị khí gas từ MQ-2
    int gasValue = analogRead(MQ2_PIN);

    // Kiểm tra lỗi cảm biến DHT11
    if (isnan(temp) || isnan(hum)) {
        Serial.println("⚠️ Lỗi: Không thể đọc giá trị từ DHT11!");
    } else {
        Serial.print("🌡 Nhiệt độ: ");
        Serial.print(temp);
        Serial.print("°C | 💧 Độ ẩm: ");
        Serial.print(hum);
        Serial.println("%");

        // Gửi dữ liệu lên Blynk
        Blynk.virtualWrite(V2, temp);  // Gửi nhiệt độ (Temperature)
        Blynk.virtualWrite(V3, hum);   // Gửi độ ẩm (Humidity)
    }

    // In giá trị MQ-2
    Serial.print("🔥 Mức khí gas: ");
    Serial.println(gasValue);

    // Gửi giá trị khí gas lên Blynk
    Blynk.virtualWrite(V1, gasValue);  // Gửi khí Gas (Gas)

    // Kiểm tra điều kiện cảnh báo
    if (temp < TEMP_THRESHOLD && gasValue < GAS_THRESHOLD) {
        Serial.println("✅ Khí GAS và Nhiệt độ an toàn. TẮT còi & đèn.");
        digitalWrite(LED_PIN, LOW);    // TẮT LED
        digitalWrite(BUZZER_PIN, LOW); // TẮT còi
        Blynk.virtualWrite(V0, 0);     // Cập nhật LED trên Blynk (tắt)
    } 
    else if (temp >= TEMP_THRESHOLD) {
        Serial.println("⚠️ CẢNH BÁO! Nhiệt độ vượt ngưỡng.");
        digitalWrite(LED_PIN, HIGH);    // BẬT LED
        digitalWrite(BUZZER_PIN, HIGH); // BẬT còi
        Blynk.virtualWrite(V0, 1);      // Cập nhật LED trên Blynk (bật)
    } 
    else if (gasValue >= GAS_THRESHOLD) {
        Serial.println("⚠️ CẢNH BÁO! Khí gas vượt ngưỡng.");
        digitalWrite(LED_PIN, HIGH);    // BẬT LED
        digitalWrite(BUZZER_PIN, HIGH); // BẬT còi
        Blynk.virtualWrite(V0, 1);      // Cập nhật LED trên Blynk (bật)
    } 
    else {
        Serial.println("🚨 CẢNH BÁO! Cả NHIỆT ĐỘ và KHÍ GAS đều vượt ngưỡng!");
        digitalWrite(LED_PIN, HIGH);    // BẬT LED
        digitalWrite(BUZZER_PIN, HIGH); // BẬT còi
        Blynk.virtualWrite(V0, 1);      // Cập nhật LED trên Blynk (bật)
    }

    delay(2000);  // Chờ 2 giây trước khi đo lại
}
