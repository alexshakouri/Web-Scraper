#include <iostream>
#include <string>
#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <tidy/tidy.h>
#include <tidy/tidybuffio.h>

//#include <libxml/tree.h>

//Need this prototype to use with CURL_WRITEFUNCTION
int curl_write(char* data, size_t size, size_t nmemb, std::string *write_data);
CURLcode curl_initialization(CURL* curl_connection, const char *URL_name, std::string *html_buffer);
void print_html(xmlNode *html_tree_node);

int main(){
    CURL *curl_connection;
    CURLcode curl_output;
    std::string html_buffer;
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

    //Clean up html and output xml    
    TidyDoc tidy_doc = tidyCreate();
    TidyBuffer tidy_html_buffer = {};
    int tidy_output = -1;    
    
    //TODO::Figure out why tidyOptSetBool() alwasys returns false
    tidy_output = tidyParseString(tidy_doc, html_buffer.c_str());
    tidy_output = tidyCleanAndRepair(tidy_doc);
    tidy_output = tidySaveBuffer(tidy_doc, &tidy_html_buffer);    

    std::cout << tidy_output << " output: " << tidy_html_buffer.bp << std::endl;
 
    tidyRelease(tidy_doc);

    const char* tidy_html_output = reinterpret_cast<const char*>(tidy_html_buffer.bp);
  
/*TODO::fix htmlReadMemory as it isn't parsing through the html
    //Read the HTML
    htmlDocPtr html_tree;
    xmlNode *root_element;    

    html_tree = htmlReadMemory(tidy_html_output,strlen(tidy_html_output), NULL, NULL, HTML_PARSE_RECOVER|HTML_PARSE_NOERROR|HTML_PARSE_NOWARNING);
    root_element = xmlDocGetRootElement(html_tree);    

    print_html(root_element);
*/

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

//TODO::fix print_html to print the entire tree or certain name
void print_html(xmlNode *html_tree_node){
    if(html_tree_node == NULL){
        std::cout << "NOTHING IN NODE" << std::endl;
        return;
    }

    std::cout << "Name: " << html_tree_node->name << " : Content" << html_tree_node->content << std::endl;
}
