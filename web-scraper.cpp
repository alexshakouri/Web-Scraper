#include <iostream>
#include <string>
#include <curl/curl.h>
#include <libxml/HTMLparser.h>

CURLcode curl_setup(CURL* curl_conn, const char *URL_name);

int main(){
    CURL *curl_conn;
    CURLcode curl_output;
    char *html_buffer = NULL;
    const char *URL_name = "https://www.google.com";

    curl_conn = curl_easy_init();
    if(curl_conn == NULL){
        std::cerr << "Failed to create CURL connection" << std::endl;
        return -1;
    }

    curl_output = curl_setup(curl_conn, URL_name);
    if(curl_output != CURLE_OK){
        std::cerr << "curl setup failed: " << curl_easy_strerror(curl_output) << std::endl;
        return -1;
    }

    //Output html
    curl_output = curl_easy_perform(curl_conn);
    std::cout << curl_easy_strerror(curl_output) << std::endl;
   
    curl_easy_cleanup(curl_conn);
 
    //Read the HTML
    htmlParserCtxtPtr html_read;
    
    html_read = htmlCreateMemoryParserCtxt(html_buffer, 0);
    if(html_read == NULL){
        std::cerr << "Failed to create html parser" << std::endl;
        return -1;
    }

    return 0;
}

CURLcode curl_setup(CURL*curl_conn, const char *URL_name){
    CURLcode setup_output; 

    setup_output = curl_easy_setopt(curl_conn, CURLOPT_URL, URL_name);
    if(setup_output != CURLE_OK){
        return setup_output;
    }

    return setup_output;
}

