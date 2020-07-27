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

CURLcode Initialization::curl_initialization(CURL *curl_connection, std::string URL_name){
    CURLcode setup_output;

    setup_output = curl_easy_setopt(curl_connection, CURLOPT_URL, URL_name.c_str());
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

    return setup_output;
}

int Initialization::curl_setup(std::string URL_name) {
    CURL *curl_connection;
    CURLcode curl_output;

    curl_connection = curl_easy_init();
    if(curl_connection == NULL){
        std::cerr << "Failed to create CURL connection" << std::endl;
        return -1;
    }

    curl_output = curl_initialization(curl_connection, URL_name.c_str());
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

void Initialization::xml_setup() {
    //Read the HTML
    this->html_tree.push_back(htmlReadMemory(this->html_buffer.c_str(), this->html_buffer.length(), NULL, NULL, HTML_PARSE_RECOVER|HTML_PARSE_NOERROR|HTML_PARSE_NOWARNING));
    //std::unique_ptr<xmlNode> temp(xmlDocGetRootElement(this->html_tree));
    //this->root_element.push_back(std::move(temp));
    this->root_element.push_back(xmlDocGetRootElement(this->html_tree.back()));

    //if(this->root_element.back().get() == NULL){
    //    std::cerr << "Unable to obtain html_tree" << std::endl;
    //}
}

void Initialization::xml_cleanup() {
    for(int i = 0; i < html_tree.size(); i++){
        xmlFreeDoc(html_tree[i]);
    }
    xmlCleanupParser();
}

xmlNodePtr Initialization::get_last_root_element() {
    //return this->root_element.back().get();
    return this->root_element.back();
}

std::string Initialization::get_html_buffer(){
    return this->html_buffer;
}

htmlDocPtr Initialization::get_html_tree(int index){
    return this->html_tree[index];
}
