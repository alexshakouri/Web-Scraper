#include "initialization.h"
//TODO::implement for multiple websites
void save_content(xmlNode *html_tree_node, std::string &content);
void find_price(xmlNode *html_tree_node);
std::string spaces_to_underscores(std::string user_input);

//TODO::include user input with error checks
int main(int argc, char *argv[]){
	std::string user_input = "";
    if(argc == 1){
    	    std::cerr << "Defaulting search to 'switch'." << std::endl;
    	    user_input = "switch";
    }
    else{
        user_input = spaces_to_underscores(argv[1]);
        std::cout << "User Input: " << user_input << std::endl;
    }

    std::vector<std::string> url_names;
	std::string amazon_URL = "https://www.amazon.com/s?k=";
	std::string bestbuy_URL = "https://www.bestbuy.com/site/searchpage.jsp?st=";
    amazon_URL.append(user_input);
    bestbuy_URL.append(user_input);
    url_names.push_back(amazon_URL);
    url_names.push_back(bestbuy_URL);

    Initialization scraper_init;
    std::vector<xmlNode *> root_element;
    int curl_init_result = 0;

    for(std::vector<std::string>::iterator url = url_names.begin(); url != url_names.end(); ++url){
    	    scraper_init.set_URL_name(*url);

    	    curl_init_result = scraper_init.curl_setup();
    	    if (curl_init_result == -1) {
    	    	    return -1;
    	    }

    	    scraper_init.xml_setup();

    	    root_element.push_back(scraper_init.get_last_root_element());
    }

    //find_price(root_element);

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
    std::replace(input_string.begin(), input_string.end(), ' ', '+');
    return input_string;
}
