#include <iostream>
#include <curl/curl.h>

int main(){
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if(curl){
        curl_easy_setopt(curl, CURLOPT_URL, "https://www.google.com");
        res = curl_easy_perform(curl);
        std::cout << res << std::endl;
       
    }
    curl_easy_cleanup(curl);
        return 0;
}
