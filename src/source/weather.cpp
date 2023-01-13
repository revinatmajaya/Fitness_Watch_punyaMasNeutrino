#include "../include/weather.h"

WiFiClient client;

const char server[] = "api.openweathermap.org";
String nameOfCity = "Malang,ID";

String apiKey = "b32f13fa9998bc87e94738c7b9889bcf";

String text;

String* weatherData;

int jsonend = 0;
bool startJson = false;
// int status = WL_IDLE_STATUS;
int fetchedStatus = false;

void initWeather(){
    text.reserve(JSON_BUFF_DIMENSION);
}

String* fetchWeather(){
    client.stop();
    if (client.connect(server, 80)){
        // client succesfully connected to server
        // send HTTP PUT request
        Serial.println("client connected");
        client.println("GET /data/2.5/forecast?q=" + nameOfCity + "&appid=" + apiKey + "&mode=json&units=metric&cnt=2 HTTP/1.1");
        client.println("Host: api.openweathermap.org");
        client.println("User-Agent: ArduinoWiFi/1.1");
        client.println("Connection: close");
        client.println();

        unsigned long timeout = millis();
        while (client.available() == 0){
            if (millis() - timeout > 5000){
                Serial.println("client timeout");
                client.stop();
                return nullptr;
            }
        }

        char c = 0;
        while (client.available()){
            c = client.read();

            if (c == '{'){
                startJson = true;
                jsonend++;
            }
            if (c == '}'){
                jsonend--;
            }
            if (startJson){
                text += c;
            }
            if (jsonend == 0 && startJson){
                weatherData = parseJson(text.c_str());
                text = "";
                startJson = false;
                fetchedStatus = true;
                break;
            }
        }
        return weatherData;
    } else { // client failed to connect
        Serial.println("connection failed");
        return nullptr;
    }
}

String* parseJson(const char* jsonString){
    const size_t bufferSize = 2*JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(2) + 4*JSON_OBJECT_SIZE(1) + 3*JSON_OBJECT_SIZE(2) + 3*JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + 2*JSON_OBJECT_SIZE(7) + 2*JSON_OBJECT_SIZE(8) + 720;
    DynamicJsonBuffer jsonBuffer(bufferSize);

    // FIND FIELDS IN JSON TREE
    JsonObject& root = jsonBuffer.parseObject(jsonString);
    if (!root.success()) {
        Serial.println("parseObject() failed");
        return nullptr;
    }

    JsonArray& list = root["list"];
    JsonObject& nowData = list[0];

    int temp = nowData["main"]["temp"];

    const String weatherID = nowData["weather"][0]["main"];
    const String temperature = String(temp);
    const String city =  root["city"]["name"];
    static String weatherData[3] = {weatherID, temperature, city};
    Serial.println(weatherData[0]);
    Serial.println(weatherData[1]);
    Serial.println(weatherData[2]);

    return weatherData;
}