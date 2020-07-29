#ifndef HTML_SETUP_H
#define HTML_SETUP_H

#include <iostream>
#include <string>
#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <vector>

class Html_Setup {
private:
    std::string html_buffer;
    htmlDocPtr html_tree;
    xmlNode * root_element;

    static int curl_write(char* data, size_t size, size_t nmemb, std::string *write_data);
    CURLcode curl_initialization(CURL *curl_connection, std::string url_name);
    void xml_cleanup();

public:
    Html_Setup();
    ~Html_Setup();

    int curl_setup(std::string URL_name);
    void xml_setup();
    xmlNodePtr get_root_element();
    std::string get_html_buffer();
    htmlDocPtr get_html_tree();
};

#endif //HTML_SETUP_H
