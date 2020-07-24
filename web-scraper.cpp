#include "initialization.h"
#include <algorithm>
#include <string.h>

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

    std::string price;
    bool found_price = false;
    find_content(root_element, price, found_price);
    if(found_price == false){
        std::cout << "No Price Found" << std::endl;
    }

    std::cout << "price: " << price << std::endl;

    return 0;
}

//TODO::Create another search function to return the pointer to the beginning of the search results

//TODO::Adapt this function for multi-use for finding specific html tags
void find_content(xmlNode *html_tree_node, std::string &price, bool &found_price){
    //In order to find the price I need to not only search through the children and next but properties node as well
    if(html_tree_node == NULL || found_price){
        return;
    }   

    //TODO::Fix the if statements by putting them before
    //TODO::Make the magical strings into global variables
    if(html_tree_node->name != NULL){
        if(strcmp(reinterpret_cast<const char*>(html_tree_node->name),"span") == 0){
            if(html_tree_node->properties != NULL){
                if(strcmp(reinterpret_cast<const char*>(html_tree_node->properties->name), "class") == 0){
                    if(html_tree_node->properties->children != NULL){
                        if(strcmp(reinterpret_cast<const char*>(html_tree_node->properties->children->content), "a-offscreen") == 0){
                            found_price = true;
                            save_content(html_tree_node->children, price);
                        }
                    }
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
    std::replace(input_string.begin(), input_string.end(), ' ', '_');
    return input_string;
}
