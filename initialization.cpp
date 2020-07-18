#include "initialization.h"

int curl_setup(const char *URL_name, std::string *html_buffer) {
    CURL *curl_connection;
    CURLcode curl_output;
    //std::string html_buffer;

    curl_connection = curl_easy_init();
    if(curl_connection == NULL){
        std::cerr << "Failed to create CURL connection" << std::endl;
        return -1;
    }

    curl_output = curl_initialization(curl_connection, URL_name, html_buffer);
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

xmlNodePtr xml_setup(std::string html_buffer) {
    //Read the HTML
    htmlDocPtr html_tree;
    xmlNode *root_element;

    html_tree = htmlReadMemory(html_buffer.c_str(), strlen(html_buffer.c_str()), NULL, NULL, HTML_PARSE_RECOVER|HTML_PARSE_NOERROR|HTML_PARSE_NOWARNING);
    root_element = xmlDocGetRootElement(html_tree);

    //TODO: fix cleanup for xml (e.g. create class for initialization)
    //xmlFreeDoc(html_tree);
    //xmlCleanupParser();

    return root_element;
}
