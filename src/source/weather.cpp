/*
⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄
⠄⠄⠄⠄⠄⠄⠄⣀⣀⣐⡀⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄
⠄⠄⢠⠄⣠⣶⣿⣿⣿⠿⠿⣛⣂⣀⣀⡒⠶⣶⣤⣤⣬⣀⡀⠄⢀⠄⠄⠄⠄⠄⠄⠄
⠄⠄⢀⣾⣿⣿⣿⡟⢡⢾⣿⣿⣿⣿⣿⣿⣶⣌⠻⣿⣿⣿⣿⣷⣦⣄⡀⠄⠄⠄⠄⠄
⠄⠄⣈⣉⡛⣿⣿⣿⡌⢇⢻⣿⣿⣿⣿⣿⠿⠛⣡⣿⣿⣿⣿⣿⣿⣿⣿⣦⣄⠄⠄⠄
⠄⠺⠟⣉⣴⡿⠛⣩⣾⣎⠳⠿⠛⣋⣩⣴⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣆⠄⠄
⠄⠄⠄⠘⢋⣴⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡆⠄
⠄⠄⢀⢀⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠄
⠄⠄⠄⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠃⣀
⠄⠄⠄⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠃⠘⠛
⠄⠄⠄⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⠋⣀⣀⣠⣤
⠄⠄⣀⣀⡙⠻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⢛⣩⠤⠾⠄⠛⠋⠉⢉
⠄⠺⠿⠛⠛⠃⠄⠉⠙⠛⠛⠻⠿⠿⠿⠟⠛⠛⠛⠉⠁⠄⠄⣀⣀⣠⣤⣠⣴⣶⣼⣿

weather.cpp
=============
this code will fetch weather datas from OpenWeatherMap API. 
The data format will be on JSON and the structure is defined by the API.
==========================================================================
code collaborator: revin atmajaya, brodiono
Started in the beginning of January 2023 and will end when it has to be ended.
*/

#include "../include/weather.h"

WiFiClient client;

const char server[] = "api.openweathermap.org";
String nameOfCity = "Bandung,ID";

String apiKey = "[your api key]";

String text;

String weatherDesc = "";
String weatherTemp = "";
String weatherCity = "";

String* weatherData[3] = {&weatherDesc, &weatherTemp, &weatherCity};

int jsonend = 0;
bool startJson = false;
// int status = WL_IDLE_STATUS;
bool fetchedStatus = false;

void initWeather(){
    text.reserve(JSON_BUFF_DIMENSION);
}

String** fetchWeather(){
    client.stop();
    weatherDesc = "null";
    weatherTemp = "0";
    weatherCity = "null";
    if (client.connect(server, 80)){
        // client succesfully connected to server
        // send HTTP PUT request
        Serial.println("client connected");
        client.println("GET /data/2.5/weather?q=" + nameOfCity + "&appid=" + apiKey + "&mode=json&units=metric&cnt=2 HTTP/1.1");
        client.println("Host: api.openweathermap.org");
        client.println("User-Agent: ArduinoWiFi/1.1");
        client.println("Connection: close");
        client.println();

        unsigned long timeout = millis();
        while (client.available() == 0){
            if (millis() - timeout > 5000){
                Serial.println("client timeout");
                client.stop();
                weatherDesc = "timeout";
                weatherTemp = "0";
                weatherCity = "timeout";
                return weatherData;
            }
        }

        char c = 0;
        while (client.available() && !fetchedStatus){
            c = client.read();
            Serial.println(c);

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
                parseJson(text.c_str());
                Serial.println(text);
                text = "";
                startJson = false;
                fetchedStatus = true;
            }
        }

        return weatherData;

    } else { // client failed to connect
        Serial.println("connection failed");
        delay(100);
        weatherDesc = "failcon";
        weatherTemp = "0";
        weatherCity = "failcon";
        return weatherData;
    }
}

void parseJson(const char* jsonString){
    const size_t bufferSize = 2*JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(2) + 4*JSON_OBJECT_SIZE(1) + 3*JSON_OBJECT_SIZE(2) + 3*JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + 2*JSON_OBJECT_SIZE(7) + 2*JSON_OBJECT_SIZE(8) + 720;
    DynamicJsonBuffer jsonBuffer(bufferSize);

    // FIND FIELDS IN JSON TREE
    JsonObject& root = jsonBuffer.parseObject(jsonString);
    if (!root.success()) {
        Serial.println("parseObject() failed");
        weatherDesc = "failparse";
    } else {
        int temp = root["main"]["temp"];

        const String description = root["weather"][0]["main"];
        const String temperature = String(temp);
        const String city =  root["name"];

        weatherDesc = description;
        weatherTemp = temperature;
        weatherCity = city;
    }
}