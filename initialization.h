#ifndef INITIALIZATION_H
#define INITIALIZATION_H

#include <iostream>
#include <string>
#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <vector>

class Initialization {
private:
    std::string URL_name;
	std::string html_buffer;
    CURL *curl_connection;
    CURLcode curl_output;
    htmlDocPtr html_tree;
    //TODO::make root_element a uniquer pointer to deal with dynamic allocation 
    //std::vector< std::unique_ptr<xmlNode> > root_element;
    std::vector<xmlNode *> root_element;

    static int curl_write(char* data, size_t size, size_t nmemb, std::string *write_data);
    CURLcode curl_initialization();
    void xml_cleanup();

public:
	Initialization();
	~Initialization();

    int curl_setup();
    void xml_setup();
    xmlNodePtr get_last_root_element();
    std::string get_html_buffer();
    htmlDocPtr get_html_tree();
    void set_URL_name(std::string URL_name);
};

#endif //INITIALIZATION_H
