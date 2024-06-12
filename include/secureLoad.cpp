#ifndef SECURELOAD
#define SECURELOAD
#include <secureLoad.h>
#include <secret.h>
//#include <googleCert.h>
//#include <Stream.h>
std::string out_str; // = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXhttps://docs.google.com/spXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX/pub?gid=656806664&single=true&output=csv";

//extern SecretC2 C2;
extern std::string g_tmp_url;

class secureLoad_CL  {
    public:
        
                
        WiFiClientSecure client;
        WiFiClientSecure * client_ptr = &client;
        std::string URL = "";
        
        
        
    void begin(){
        
        Serial.println("secureLoadB CLASS begin()");
        //client_ptr->setTrustAnchors(&cert);
        client_ptr->setBufferSizes(4096, 2048);
        //out_str.reserve(2048);
    }

    void reset() {
        Serial.println("secureLoadB CLASS reset()");
       
        *client_ptr = WiFiClientSecure();
        begin();

    }


    std::string secureLoadB(std::string InputUrlS = "", uint8 IS_flag_b = 0){
        if (InputUrlS == ""){
            InputUrlS = URL;
        }
        Serial.println("secureLoadB CLASS A1");
        reset();
        //out_str.reserve(1024);
        if (IS_flag_b == 1) {
            //client_ptr->setTrustAnchors(&cert);
            Serial.println("Do not use cert");
        }
        else if (IS_flag_b == 0 || IS_flag_b == 2) {
            client_ptr->setInsecure();
            //client_ptr->setBufferSizes(4096,2048);
        }
        
        //
        HTTPClient https;
        //https._location.reserve(500);

        https.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
        //https.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        https.setRedirectLimit(10);
        
        Serial.println("secureLoadB CLASS A2");
        if (!https.begin(*client_ptr, InputUrlS.c_str())) {
            Serial.println("secureLoadB CLASS failed http begin");
            //OutputStringS = "";
            out_str = "";
            //return out_str ;
        }  
        else {
            
            //Serial.println(https.GET());
            //Serial.println(https.getSize());
            Serial.println("secureLoadB CLASS BEFORE https.GET()");
            //byte httpCode = https.GET();
            //Serial.println("secureLoadB CLASS AFTER https.GET()");
            //Serial.println("httpCode=");
            //Serial.println(httpCode);
            int httpCode = 0;
            

            if (IS_flag_b == 2){

                httpCode = https.GET();  
                //httpCode =  https.sendRequest("POST");
                Serial.println("httpCode");
                Serial.println(httpCode);
                httpCode = 0;
                out_str = "OK";           

            }

            if (IS_flag_b != 2){
                
                httpCode = https.GET();                        
                Serial.println("httpCode");
                Serial.println(httpCode);
                Serial.println("InputUrlS.c_str()");
                Serial.println(InputUrlS.c_str());
                //Serial.println(https.getLocation().c_str() );
            }
            if (httpCode > 0) {
                Serial.println("secureLoadB CLASS http https.getSize();");
                Serial.println(https.getSize());
                Serial.println("secureLoadB CLASS http httpCode > 0");
                //out_str  = std::string(https.getString().c_str());

                Serial.printf("[HTTP] GET... code: %d\n", httpCode);
                Serial.println(httpCode);

                if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                    //OutputStringS = https.getString().c_str();
                    if (IS_flag_b == 2){
                        out_str = "OK";
                    }
                    else{
                        out_str = std::string(https.getString().c_str());                        
                    }
                    //out_str = std::string(https. c_str());
                }
            }
            //Serial.println("httpCode=");
            //Serial.println(httpCode);
            https.end();
            Serial.println("secureLoadB CLASS https.end()");
        }
        //client.disableKeepAlive();
        //reset();
        return out_str;   
    }

    

    void lineSend(std::string message){
        std::string token_str = std::string("Bearer ") + std::string(LINE_TOKEN).c_str();
        //WiFiClientSecure client;
        //HTTPClient http;
        client_ptr->setInsecure();
        if (WiFi.status() == WL_CONNECTED) {
            HTTPClient http;

            http.begin(*client_ptr, LINE_NOTIFY_URL);
            //http.addHeader("Authorization", "Bearer " + String(LINE_TOKEN));

            http.addHeader("Authorization", token_str.c_str());
            http.addHeader("Content-Type", "application/x-www-form-urlencoded");

            message = "message=" + message;
            Serial.println("message.c_str()");Serial.println(message.c_str());
            int httpResponseCode = http.POST(message.c_str());

            if (httpResponseCode > 0) {
            std::string response_str = std::string("Response String: ") + std::string(http.getString().c_str());
            std::string response_code = std::string("Response Code: ") + std::to_string(httpResponseCode);
            //Serial.println("HTTP Response Code: " + String(httpResponseCode));
            //Serial.println("HTTP Response Code: " + std::string(httpResponseCode).c_str());
            Serial.println(response_code.c_str());
            Serial.println(response_str.c_str());
            } else {
            Serial.print("Error sending POST request: ");
            Serial.println(httpResponseCode);
            }

            http.end();
        } else {
            Serial.println("WiFi not connected");
        }
    }

    void formSend(std::string message){
        //std::string token_str = std::string("Bearer ") + std::string(C2.LINE_TOKEN).c_str();
        //WiFiClientSecure client;
        //HTTPClient http;
        reset();
        client_ptr->setInsecure();
        //client_ptr->setTrustAnchors(&cert);
        
        if (WiFi.status() == WL_CONNECTED) {
            HTTPClient http;
            http.useHTTP10(false);
            http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
            http.setRedirectLimit(10);
            
            
            http.begin(*client_ptr,  g_tmp_url.c_str());
            http.addHeader("Host", "docs.google.com");            
            //http.addHeader("HTTP","2");
            http.addHeader("accept", "*/*");
            http.addHeader("content-length",std::to_string (message.length()).c_str() );
            //http.addHeader("content-Type", "application/x-www-form-urlencoded");
            http.addHeader("content-Type", "text/plain");
            //message = "message=" + message;
            //message = "entry.66851863=MAC4&entry.1699332649=3&submit=Submit&entry.1458667389=" + message;
            Serial.println(g_tmp_url.c_str());
            Serial.println("message.c_str()");Serial.println(message.c_str());
            
            int httpResponseCode = http.POST(message.c_str());

            if (httpResponseCode > 0) {
            //std::string response_str = std::string("Response String: ") + std::string(http.getString().c_str());
            //std::string response_code = std::string("Response Code: ") + std::to_string(httpResponseCode);
            //Serial.println("HTTP Response Code: " + String(httpResponseCode));
            //Serial.println("HTTP Response Code: " + std::string(httpResponseCode).c_str());
                Serial.println(httpResponseCode);
            //Serial.println(response_str.c_str());
            } else {
            Serial.print("Error sending POST request: ");
            Serial.println(httpResponseCode);
            
            }
            http.end();
            reset();
            

            //http.end();
        } else {
            Serial.println("WiFi not connected");
        }
    }
        
};


#endif //SECURELOAD