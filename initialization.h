#ifndef INITIALIZATION_H
#define INITIALIZATION_H

#include <iostream>
#include <string>
#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <vector>

class Initialization {
private:
    std::string html_buffer;
    std::vector<htmlDocPtr> html_tree;
    //TODO::make root_element a uniquer pointer to deal with dynamic allocation 
    //std::vector< std::unique_ptr<xmlNode> > root_element;
    std::vector<xmlNode *> root_element;

    static int curl_write(char* data, size_t size, size_t nmemb, std::string *write_data);
    CURLcode curl_initialization(CURL *curl_connection, std::string URL_name);
    void xml_cleanup();

public:
	Initialization();
	~Initialization();

    int curl_setup(std::string URL_name);
    void xml_setup();
    xmlNodePtr get_last_root_element();
    std::string get_html_buffer();
    htmlDocPtr get_html_tree(int index);
};

#endif //INITIALIZATION_H
