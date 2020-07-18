#include <iostream>
#include <string>
#include <curl/curl.h>
#include <libxml/HTMLparser.h>

int curl_setup(const char *URL_name, std::string *html_buffer);
xmlNodePtr xml_setup(std::string html_buffer);

int curl_write(char* data, size_t size, size_t nmemb, std::string *write_data);
CURLcode curl_initialization(CURL* curl_connection, const char *URL_name, std::string *html_buffer);
