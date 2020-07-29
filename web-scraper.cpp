#include "Html_Setup.h"
#include <algorithm>
#include <string.h>

#define AMZN_NODE_NAME "span"
#define AMZN_PROPERTIES_NAME "class"
#define AMZN_PROPERTIES_CONTENT "a-offscreen"

//TODO::implement for multiple websites
void save_content(xmlNode *html_tree_node, std::string &content);
void find_content(xmlNode *html_tree_node, std::string &price, bool &found_price);
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
    //TODO::bestbuy URL curl output giving 403
    //possibly bestbuy doesn't allow webscraping
    std::string bestbuy_URL = "https://www.bestbuy.com/site/searchpage.jsp?st=";
    amazon_URL.append(user_input);
    bestbuy_URL.append(user_input);
    url_names.push_back(amazon_URL);
    url_names.push_back(bestbuy_URL);

    Html_Setup scraper_init;
    std::vector<xmlNode *> root_element;
    int curl_init_result = 0;

    for(unsigned int i = 0; i < url_names.size(); i++){
        curl_init_result = scraper_init.curl_setup(url_names[i]);
        if (curl_init_result == -1) {
            return -1;
        }

        scraper_init.xml_setup();

        root_element.push_back(scraper_init.get_root_element());
    }

    std::string price;
    bool found_price = false;
    find_content(root_element[0], price, found_price);
    if(found_price == false){
        std::cout << "No Price Found" << std::endl;
    }
    else{
        std::cout << "price: " << price << std::endl;
    }

    return 0;
}

//TODO::Create another search function to return the pointer to the beginning of the search results

//TODO::Adapt this function for multi-use for finding specific html tags
void find_content(xmlNode *html_tree_node, std::string &price, bool &found_price){
    //In order to find the price I need to not only search through the children and next but properties node as well
    if(html_tree_node == NULL || found_price){
        return;
    }   

    if(html_tree_node->properties != NULL && html_tree_node->properties->children != NULL){
        if(strcmp(reinterpret_cast<const char*>(html_tree_node->name), AMZN_NODE_NAME) == 0){
            if(strcmp(reinterpret_cast<const char*>(html_tree_node->properties->name), AMZN_PROPERTIES_NAME) == 0){
                if(strcmp(reinterpret_cast<const char*>(html_tree_node->properties->children->content), AMZN_PROPERTIES_CONTENT) == 0){
                    found_price = true;
                    save_content(html_tree_node->children, price);
                }
            }
        }
    }

    find_content(html_tree_node->next, price, found_price);
    find_content(html_tree_node->children, price, found_price);
}

void save_content(xmlNode *html_tree_node, std::string &content){
    
    if(html_tree_node->type == XML_CDATA_SECTION_NODE || html_tree_node->type == XML_TEXT_NODE){
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
