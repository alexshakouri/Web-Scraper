#include "initialization.h"
//TODO::implement for multiple websites
void save_content(xmlNode *html_tree_node, std::string &content);
void find_price(xmlNode *html_tree_node);

//TODO::include user input with error checks
int main(){
	std::string user_input = "switch";
	std::string URL_name = "https://www.amazon.com/s?k=";
	URL_name.append(user_input);
	Initialization scraper_init(URL_name);
	int curl_init_result = 0;

    curl_init_result = scraper_init.curl_setup();
    if (curl_init_result == -1) {
    	    return -1;
    }

    xmlNode *root_element;
    scraper_init.xml_setup();
    //TODO:: Fix bug where occasionally the html returns the wrong thing (html -> body)
    root_element = scraper_init.get_root_element();

    find_price(root_element);

    return 0;
}

//TODO::fix print_html to parse_html to find certain nodes within the html tree
void find_price(xmlNode *html_tree_node){
    std::string content;

    save_content(html_tree_node->children->next->next->children->children, content);

    std::cout << "Content: " << content << std::endl;
}

//
void save_content(xmlNode *html_tree_node, std::string &content){
    
    if(html_tree_node->type == XML_CDATA_SECTION_NODE){
        //TODO::Find way to save content here even if it contains '/n' (std::string s( reinterpret_cast<char const*>(uc), len)
        content = reinterpret_cast<char*>(html_tree_node->content);
    }
    else{
        std::cerr << "Node is not of type XML_CDATA_SECTION_NODE" << std::endl;
    }
  
}
