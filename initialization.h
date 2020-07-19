#ifndef INITIALIZATION_H
#define INITIALIZATION_H

#include <iostream>
#include <string>
#include <curl/curl.h>
#include <libxml/HTMLparser.h>

class Initialization {
private:
    std::string URL_name;
	std::string html_buffer;
    CURL *curl_connection;
    CURLcode curl_output;
    htmlDocPtr html_tree;
    xmlNode *root_element;

    static int curl_write(char* data, size_t size, size_t nmemb, std::string *write_data);
    CURLcode curl_initialization();
    void xml_cleanup();

public:
	Initialization(std::string URL_name);
	~Initialization();

    int curl_setup();
    void xml_setup();
    xmlNodePtr get_root_element();
};

#endif //INITIALIZATION_H
