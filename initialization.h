#ifndef INITIALIZATION_H
#define INITIALIZATION_H

#include <iostream>
#include <string>
#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <algorithm>

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
    std::string get_html_buffer();
<<<<<<< HEAD
=======
    htmlDocPtr get_html_tree();

>>>>>>> b9aac2d66d4f107a8d320144b4d0864f3dcd9941
};

#endif //INITIALIZATION_H
