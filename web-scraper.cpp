#include "initialization.h"
//TODO::implement for multiple websites

void print_html(xmlNode *html_tree_node);

//TODO::include user input with error checks
int main(){
    //TODO::change this to amazon search URL where user inputs string
    const char *URL_name = "https://www.google.com";
	Initialization scraper_init(URL_name);
	int curl_init_result = 0;

    curl_init_result = scraper_init.curl_setup();
    if (curl_init_result == -1) {
    	    return -1;
    }

    xmlNode *root_element;
    scraper_init.xml_setup();
    root_element = scraper_init.get_root_element();

    print_html(root_element);

    return 0;
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
