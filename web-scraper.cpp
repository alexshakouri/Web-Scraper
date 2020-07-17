#include <iostream>
#include <string>
#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <tidy.h>
#include <tidybuffio.h>
//TODO::implement for multiple websites

//Need this prototype to use with CURL_WRITEFUNCTION
int curl_write(char* data, size_t size, size_t nmemb, std::string *write_data);
CURLcode curl_initialization(CURL* curl_connection, const char *URL_name, std::string *html_buffer);
void print_html(xmlNode *html_tree_node);

//TODO::include user input with error checks
int main(){
    //TODO::create initialization.cpp/.h to read the html
    CURL *curl_connection;
    CURLcode curl_output;
    std::string html_buffer;
    
    //TODO::change this to amazon search URL where user inputs string
    const char *URL_name = "https://www.google.com";

    curl_connection = curl_easy_init();
    if(curl_connection == NULL){
        std::cerr << "Failed to create CURL connection" << std::endl;
        return -1;
    }

    curl_output = curl_initialization(curl_connection, URL_name, &html_buffer);
    if(curl_output != CURLE_OK){
        std::cerr << "curl setup failed: " << curl_easy_strerror(curl_output) << std::endl;
        return -1;
    }

    //Output html
    curl_output = curl_easy_perform(curl_connection);
    if(curl_output != CURLE_OK){
        std::cout << curl_easy_strerror(curl_output) << std::endl;
    }
    curl_easy_cleanup(curl_connection);
   
    //Read the HTML
    htmlDocPtr html_tree;
    xmlNode *root_element;    

    html_tree = htmlReadMemory(html_buffer.c_str(), strlen(html_buffer.c_str()), NULL, NULL, HTML_PARSE_RECOVER|HTML_PARSE_NOERROR|HTML_PARSE_NOWARNING);
    root_element = xmlDocGetRootElement(html_tree);

    xmlFreeDoc(html_tree);    
    print_html(root_element);
    xmlCleanupParser();

    return 0;
}

int curl_write(char* data, size_t size, size_t data_size, std::string *write_data){
    if(write_data == NULL){
        return 0;
    }
    write_data->append(data, size*data_size);
    return size*data_size;
}

CURLcode curl_initialization(CURL*curl_connection, const char *URL_name, std::string *html_buffer){
    CURLcode setup_output; 

    setup_output = curl_easy_setopt(curl_connection, CURLOPT_URL, URL_name);
    if(setup_output != CURLE_OK){
        return setup_output;
    }
   
    setup_output = curl_easy_setopt(curl_connection, CURLOPT_WRITEFUNCTION, curl_write);
    if(setup_output != CURLE_OK){
        return setup_output;
    }
 
    setup_output = curl_easy_setopt(curl_connection, CURLOPT_WRITEDATA, html_buffer);
    if(setup_output != CURLE_OK){
        std::cerr << "Failed to write to html buffer" << std::endl;
        return setup_output;
    }
    
    return setup_output;
}

//TODO::fix print_html to parse_html to find certain nodes within the html tree
void print_html(xmlNode *html_tree_node){
    if(html_tree_node == NULL){
        std::cout << "NOTHING IN NODE" << std::endl;
        return;
    }

    std::cout << "Name: " << html_tree_node->name << " | Content: " << html_tree_node->content << std::endl;
}

//TODO::create display function to output content of specific node in html tree
