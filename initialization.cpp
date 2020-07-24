#include "initialization.h"

Initialization::Initialization()
{

}

Initialization::~Initialization() {
	xml_cleanup();
}

//Need this prototype to use with CURL_WRITEFUNCTION
int Initialization::curl_write(char *data, size_t size, size_t data_size, std::string *write_data){
    if(write_data == NULL){
       	std::cerr << "Writer function data is empty" << std::endl;
        return 0;
    }
    write_data->append(data, size*data_size);
    return size*data_size;
}

CURLcode Initialization::curl_initialization(){
    CURLcode setup_output;

    setup_output = curl_easy_setopt(this->curl_connection, CURLOPT_URL, this->URL_name.c_str());
    if(setup_output != CURLE_OK){
        	std::cerr << "Failed to set URL" << std::endl;
        return setup_output;
    }

    setup_output = curl_easy_setopt(this->curl_connection, CURLOPT_WRITEFUNCTION, curl_write);
    if(setup_output != CURLE_OK){
    	    std::cerr << "Failed to set write function" << std::endl;
        return setup_output;
    }

    setup_output = curl_easy_setopt(this->curl_connection, CURLOPT_WRITEDATA, &this->html_buffer);
    if(setup_output != CURLE_OK){
        std::cerr << "Failed to write to html buffer" << std::endl;
        return setup_output;
    }

    //Enable built in compression encoding
    setup_output = curl_easy_setopt(this->curl_connection, CURLOPT_ACCEPT_ENCODING, "");
    if(setup_output != CURLE_OK){
        std::cerr << "Failed to set compression encoding" << std::endl;
        return setup_output;
    }

    return setup_output;
}

int Initialization::curl_setup() {
    this->curl_connection = curl_easy_init();
    if(this->curl_connection == NULL){
        std::cerr << "Failed to create CURL connection" << std::endl;
        return -1;
    }

    this->curl_output = curl_initialization();
    if(this->curl_output != CURLE_OK){
        std::cerr << "curl setup failed: " << curl_easy_strerror(this->curl_output) << std::endl;
        return -1;
    }

    //Output html
    this->curl_output = curl_easy_perform(this->curl_connection);
    if(this->curl_output != CURLE_OK){
        std::cerr << "curl_easy_perform failed: " << curl_easy_strerror(this->curl_output) << std::endl;
    }

    //Output response code for curl
    long response_code;
    curl_easy_getinfo(curl_connection, CURLINFO_RESPONSE_CODE, &response_code);
    std::cout << "response code: " << response_code << std::endl;

    curl_easy_cleanup(this->curl_connection);

    return 0;
}

void Initialization::xml_setup() {
    //Read the HTML
    this->html_tree = htmlReadMemory(this->html_buffer.c_str(), this->html_buffer.length(), NULL, NULL, HTML_PARSE_RECOVER|HTML_PARSE_NOERROR|HTML_PARSE_NOWARNING);
    this->root_element.push_back(xmlDocGetRootElement(this->html_tree));

    if(this->root_element.back() == NULL){
        std::cerr << "Unable to obtain html_tree" << std::endl;
    }
}

void Initialization::xml_cleanup() {
    xmlFreeDoc(html_tree);
    xmlCleanupParser();
}

xmlNodePtr Initialization::get_last_root_element() {
    return this->root_element.back();
}

std::string Initialization::get_html_buffer(){
    return this->html_buffer;
}

htmlDocPtr Initialization::get_html_tree(){
    return this->html_tree;
}

void Initialization::set_URL_name(std::string URL_name){
    this->URL_name = URL_name;
}
