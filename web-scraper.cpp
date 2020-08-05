#include "Html_Setup.h"
#include <algorithm>
#include <string.h>

//TODO::determine values for other websites (e.g. newegg.com)
#define AMZN_PRICE_NODE_NAME "span"
#define AMZN_PRICE_PROPERTIES_NAME "class"
#define AMZN_PRICE_PROPERTIES_CONTENT "a-offscreen"

#define AMZN_SEARCH_NODE_NAME "div"
#define AMZN_SEARCH_PROPERTIES_NAME "data-index"
#define AMZN_SEARCH_PROPERTIES_CONTENT "0"

//TODO::implement for multiple websites
void save_content(xmlNode *html_tree_node, std::string &content);
bool search_html_properties(xmlAttr *html_properties_node, const char *properties_name, const char *properties_content);
void find_content(xmlNode *html_tree_node, std::string &price, bool &found_price);
void find_search_results(xmlNode *html_tree_node, xmlNode* &search_result, bool &found_results);
std::string spaces_to_underscores(std::string user_input);

//TODO::include user input with error checks
int main(int argc, char *argv[]){
	std::string user_input = "";
    if(argc == 1){
        std::cerr << "Defaulting search to 'switch'." << std::endl;
        user_input = "xbox";
    }
    else{
        user_input = spaces_to_underscores(argv[1]);
        std::cout << "User Input: " << user_input << std::endl;
    }

    std::vector<std::string> url_names;
    //TODO::amazon bot detector gives the wrong HTML possible fix is to change the header
    std::string amazon_url = "https://www.amazon.com/s?k=";
    //TODO::bestbuy URL curl output giving 403
    //possibly bestbuy doesn't allow webscraping
    std::string bestbuy_url = "https://www.bestbuy.com/site/searchpage.jsp?st=";
    std::string newegg_url = "https://www.newegg.com/p/pl?d=";

    amazon_url.append(user_input);
    bestbuy_url.append(user_input);
    newegg_url.append(user_input);
    url_names.push_back(amazon_url);
    url_names.push_back(bestbuy_url);
    url_names.push_back(newegg_url);

    std::vector<Html_Setup> scraper_init;
    int curl_init_result = 0;
    std::string price;
    xmlNode *AMZN_results = NULL;

    for(unsigned int i = 0; i < url_names.size(); i++){
        scraper_init.push_back(Html_Setup(url_names[i]));

        curl_init_result = scraper_init[i].curl_setup();
        if (curl_init_result == -1) {
            return -1;
        }

        scraper_init[i].xml_setup();

        bool found_price = false;
        bool found_results = false;

        //TODO::output multiple results from the search
        find_search_results(scraper_init[i].get_root_element(), AMZN_results, found_results);
        if(AMZN_results == NULL){
            std::cout << "No search results found" << std::endl;
            return -1;
        }
        find_content(AMZN_results, price, found_price);
        if(found_price == false){
            std::cout << "No Price Found" << std::endl;
        }
        else{
            std::cout << "price: " << price << std::endl;
        }
    }

    return 0;
}

//TODO::create function to return the title of the item being priced

bool search_html_properties(xmlAttr *html_properties_node, const char *properties_name, const char *properties_content){
    xmlAttr *search_html_node = html_properties_node;    

    while(search_html_node != NULL){
        if(strcmp(reinterpret_cast<const char*>(search_html_node->name), properties_name) == 0){
            if(strcmp(reinterpret_cast<const char*>(search_html_node->children->content), properties_content) == 0){
                return true;
            }
        }
        search_html_node = search_html_node->next;
    }
    return false;
}

//Returns the first search result in the amazon list in order to move to the next item go ->next->next (or find the next div)
void find_search_results(xmlNode *html_tree_node, xmlNode* &search_result, bool &found_results){
    if(html_tree_node == NULL || found_results){
        return;
    }   

    if(html_tree_node->properties != NULL && html_tree_node->properties->children != NULL){
        if(strcmp(reinterpret_cast<const char*>(html_tree_node->name), AMZN_SEARCH_NODE_NAME) == 0){
            if(search_html_properties(html_tree_node->properties, AMZN_SEARCH_PROPERTIES_NAME, AMZN_SEARCH_PROPERTIES_CONTENT)){
                found_results = true;
                search_result = html_tree_node;

            }
        }
    }

    find_search_results(html_tree_node->next, search_result, found_results);
    find_search_results(html_tree_node->children, search_result, found_results);
}


//TODO::Adapt this function for multi-use for finding specific html tags
void find_content(xmlNode *html_tree_node, std::string &price, bool &found_price){
    //In order to find the price I need to not only search through the children and next but properties node as well
    if(html_tree_node == NULL || found_price){
        return;
    }   

    if(html_tree_node->properties != NULL && html_tree_node->properties->children != NULL){
        if(strcmp(reinterpret_cast<const char*>(html_tree_node->name), AMZN_PRICE_NODE_NAME) == 0){
            //TODO::implement search_html_tree_properties functions here
            if(strcmp(reinterpret_cast<const char*>(html_tree_node->properties->name), AMZN_PRICE_PROPERTIES_NAME) == 0){
                if(strcmp(reinterpret_cast<const char*>(html_tree_node->properties->children->content), AMZN_PRICE_PROPERTIES_CONTENT) == 0){
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
