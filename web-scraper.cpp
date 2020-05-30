#include <iostream>
#include <string>
#include <curl/curl.h>
#include <libxml/HTMLparser.h>

//Need this prototype to use with CURL_WRITEFUNCTION
int curl_write(char* data, size_t size, size_t nmemb, std::string *write_data);
CURLcode curl_setup(CURL* curl_conn, const char *URL_name, std::string *html_buffer);

int main(){
    CURL *curl_conn;
    CURLcode curl_output;
    std::string html_buffer;
    const char *URL_name = "https://www.google.com";

    curl_conn = curl_easy_init();
    if(curl_conn == NULL){
        std::cerr << "Failed to create CURL connection" << std::endl;
        return -1;
    }

    curl_output = curl_setup(curl_conn, URL_name, &html_buffer);
    if(curl_output != CURLE_OK){
        std::cerr << "curl setup failed: " << curl_easy_strerror(curl_output) << std::endl;
        return -1;
    }

    //Output html
    curl_output = curl_easy_perform(curl_conn);
    std::cout << curl_easy_strerror(curl_output) << std::endl;
    curl_easy_cleanup(curl_conn);

    /* 
    //Read the HTML
    htmlParserCtxtPtr html_read;
    
    html_read = htmlCreateMemoryParserCtxt(html_buffer.c_str(), 0);
    if(html_read == NULL){
        std::cerr << "Failed to create html parser" << std::endl;
        return -1;
    }
    */
    return 0;
}

int curl_write(char* data, size_t size, size_t data_size, std::string *write_data){
    if(write_data == NULL){
        return 0;
    }

    write_data->append(data, size*data_size);
    return size*data_size;
}

CURLcode curl_setup(CURL*curl_conn, const char *URL_name, std::string *html_buffer){
    CURLcode setup_output; 

    setup_output = curl_easy_setopt(curl_conn, CURLOPT_URL, URL_name);
    if(setup_output != CURLE_OK){
        return setup_output;
    }
   
    setup_output = curl_easy_setopt(curl_conn, CURLOPT_WRITEFUNCTION, curl_write);
    if(setup_output != CURLE_OK){
        return setup_output;
    }
 
    setup_output = curl_easy_setopt(curl_conn, CURLOPT_WRITEDATA, html_buffer);
    if(setup_output != CURLE_OK){
        std::cerr << "Failed to write to html buffer" << std::endl;
        return setup_output;
    }
    
    return setup_output;
}

