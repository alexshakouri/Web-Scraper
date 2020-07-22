#include "initialization.h"
//TODO::implement for multiple websites
void save_content(xmlNode *html_tree_node, std::string &content);
void find_price(xmlNode *html_tree_node);
std::string spaces_to_underscores(std::string user_input);

//TODO::include user input with error checks
int main(int argc, char *argv[]){
	std::string user_input = "";
    if(argc == 2){ //only accepting one search result currently
        user_input = spaces_to_underscores(argv[1]);
        std::cout << "User Input: " << user_input << std::endl;
    }
    else if(argc == 1){
    	    std::cerr << "Defaulting search to 'switch'." << std::endl;
    	    user_input = "switch";
    }
    else{
    		std::cerr << "Too many arguments given. Exiting program." << std::endl;
        return 0;
    }
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

std::string spaces_to_underscores(std::string input_string){
    std::replace(input_string.begin(), input_string.end(), ' ', '_');
    return input_string;
}
