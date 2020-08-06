#include "Html_Setup.h"

Html_Setup::Html_Setup(std::string url_name)
    : url_name(url_name),
      html_tree(NULL),
      root_element(NULL)
{

}

Html_Setup::~Html_Setup() {
	//xml_cleanup();
}

//Need this prototype to use with CURL_WRITEFUNCTION
int Html_Setup::curl_write(char *data, size_t size, size_t data_size, std::string *write_data){
    if(write_data == NULL){
       	std::cerr << "Writer function data is empty" << std::endl;
        return 0;
    }
    write_data->append(data, size*data_size);
    return size*data_size;
}

CURLcode Html_Setup::curl_initialization(CURL *curl_connection){
    CURLcode setup_output;

    setup_output = curl_easy_setopt(curl_connection, CURLOPT_URL, this->url_name.c_str());
    if(setup_output != CURLE_OK){
        std::cerr << "Failed to set URL" << std::endl;
        return setup_output;
    }

    setup_output = curl_easy_setopt(curl_connection, CURLOPT_WRITEFUNCTION, curl_write);
    if(setup_output != CURLE_OK){
        std::cerr << "Failed to set write function" << std::endl;
        return setup_output;
    }

    setup_output = curl_easy_setopt(curl_connection, CURLOPT_WRITEDATA, &this->html_buffer);
    if(setup_output != CURLE_OK){
        std::cerr << "Failed to write to html buffer" << std::endl;
        return setup_output;
    }

    //Enable built in compression encoding
    setup_output = curl_easy_setopt(curl_connection, CURLOPT_ACCEPT_ENCODING, "");
    if(setup_output != CURLE_OK){
        std::cerr << "Failed to set compression encoding" << std::endl;
        return setup_output;
    }

    //Set the user agent to get around bot detectors
    setup_output = curl_easy_setopt(curl_connection, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/84.0.4147.105 Safari/537.36");
    if(setup_output != CURLE_OK){
        std::cerr << "Failed to set user agent" << std::endl;
        return setup_output;
    }    

    return setup_output;
}

int Html_Setup::curl_setup() {
    CURL *curl_connection;
    CURLcode curl_output;

    curl_connection = curl_easy_init();
    if(curl_connection == NULL){
        std::cerr << "Failed to create CURL connection" << std::endl;
        return -1;
    }

    curl_output = curl_initialization(curl_connection);
    if(curl_output != CURLE_OK){
        std::cerr << "curl setup failed: " << curl_easy_strerror(curl_output) << std::endl;
        return -1;
    }

    //Output html
    curl_output = curl_easy_perform(curl_connection);
    if(curl_output != CURLE_OK){
        std::cerr << "curl_easy_perform failed: " << curl_easy_strerror(curl_output) << std::endl;
        return -1;
    }

    //Output response code for curl
    long response_code;
    curl_easy_getinfo(curl_connection, CURLINFO_RESPONSE_CODE, &response_code);
    std::cout << "response code: " << response_code << std::endl;

    curl_easy_cleanup(curl_connection);

    return 0;
}

void Html_Setup::xml_setup() {
    //Read the HTML
    this->html_tree = htmlReadMemory(this->html_buffer.c_str(), this->html_buffer.length(), NULL, NULL, HTML_PARSE_RECOVER|HTML_PARSE_NOERROR|HTML_PARSE_NOWARNING);
    this->root_element = xmlDocGetRootElement(this->html_tree);
}

void Html_Setup::xml_cleanup() {
    if (html_tree != NULL) {
        xmlFreeDoc(html_tree);
        xmlCleanupParser();
    }
}

xmlNodePtr Html_Setup::get_root_element() {
    return this->root_element;
}

std::string Html_Setup::get_html_buffer(){
    return this->html_buffer;
}

htmlDocPtr Html_Setup::get_html_tree(){
    return this->html_tree;
}
