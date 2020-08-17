#include "Html_Setup.h"
#include <algorithm>
#include <string.h>
#include <vector>
#include <memory>

//TODO::move global variables to its own file
//TODO::determine values for other websites (e.g. newegg.com)
#define AMAZON 0
#define NEWEGG 1

//Find the first search result
#define AMZN_SEARCH_NODE_NAME "div"
#define AMZN_SEARCH_PROPERTIES_NAME "data-index"
#define AMZN_SEARCH_PROPERTIES_CONTENT "0"

//Find the price of the item
#define AMZN_PRICE_NODE_NAME "span"
#define AMZN_PRICE_PROPERTIES_NAME "class"
#define AMZN_PRICE_PROPERTIES_CONTENT "a-offscreen"

//Find the name of the item
#define AMZN_NAME_NODE_NAME "span"
#define AMZN_NAME_PROPERTIES_NAME "class"
#define AMZN_NAME_PROPERTIES_CONTENT "a-size-medium a-color-base a-text-normal"


#define EGGZ_SEARCH_NODE_NAME "div"
#define EGGZ_SEARCH_PROPERTIES_NAME "class"
#define EGGZ_SEARCH_PROPERTIES_CONTENT "item-cells-wrap border-cells items-grid-view four-cells expulsion-one-cell"

#define EGGZ_PRICE_NODE_NAME "span"
#define EGGZ_PRICE_PROPERTIES_NAME "class"
#define EGGZ_PRICE_PROPERTIES_CONTENT "price-current-label"

#define EGGZ_PRICE_STRONG "strong"
#define EGGZ_PRICE_SUP "sup"

#define EGGZ_NAME_NODE_NAME "a"
#define EGGZ_NAME_PROPERTIES_NAME "class"
#define EGGZ_NAME_PROPERTIES_CONTENT "item-title"

//TODO::implement for multiple websites
void save_content(xmlNode *html_tree_node, std::string &content, int url, bool is_finding_price);
bool search_html_properties(xmlAttr *html_properties_node, const char *properties_name, const char *properties_content);
void find_item_content(xmlNode *html_tree_node, std::string &item_price, bool &found_price, int url, bool is_finding_price, const char *node_name, const char *properties_name, const char *properties_content);
void find_newegg_price(xmlNode *html_tree_node, std::string &item_price, bool &found_price, int url, bool is_finding_price,  const char *node_name/*, const char *properties_name, const char *properties_content*/);
void find_search_results(xmlNode *html_tree_node, xmlNode* &search_result, bool &found_results, const char *node_name, const char *properties_name, const char *properties_content);
std::string spaces_to_underscores(std::string user_input);

//TODO::include user input with error checks
int main(int argc, char *argv[]){
    std::string user_input = "";
    if(argc == 1){
        user_input = "switch_games";
        std::cerr << "Defaulting search to '" << user_input << "'." << std::endl;
    }
    else{
        user_input = spaces_to_underscores(argv[1]);
        std::cout << "User Input: " << user_input << std::endl;
    }

    std::vector<std::string> url_names;
    std::string amazon_url = "https://www.amazon.com/s?k=";
    std::string newegg_url = "https://www.newegg.com/p/pl?d=";

    amazon_url.append(user_input);
    newegg_url.append(user_input);
    url_names.push_back(amazon_url);
    url_names.push_back(newegg_url);

    std::vector< std::unique_ptr<Html_Setup> > scraper_init;
    int curl_init_result = 0;
    bool found_results = false;
    bool found_price = false;
    bool found_name = false;
    bool is_finding_price = false;
    std::string item_price = "";
    std::string item_name = "";
    xmlNode *search_results = NULL;
    xmlNode *newegg_results = NULL;

    std::string search_node_name = "";
    std::string search_properties_name = "";
    std::string search_properties_content = "";

    std::string price_node_name = "";
    std::string price_properties_name = "";
    std::string price_properties_content = "";

    std::string name_node_name = "";
    std::string name_properties_name = "";
    std::string name_properties_content = "";

    for(unsigned int url = 0; url < url_names.size(); url++){
        curl_init_result = 0;
        found_results = false;
        search_results = NULL;
        newegg_results = NULL;

        switch(url){
            case AMAZON:
                search_node_name = AMZN_SEARCH_NODE_NAME;
                search_properties_name = AMZN_SEARCH_PROPERTIES_NAME;
                search_properties_content = AMZN_SEARCH_PROPERTIES_CONTENT;

                price_node_name = AMZN_PRICE_NODE_NAME;
                price_properties_name = AMZN_PRICE_PROPERTIES_NAME;
                price_properties_content = AMZN_PRICE_PROPERTIES_CONTENT;

                name_node_name = AMZN_NAME_NODE_NAME;
                name_properties_name = AMZN_NAME_PROPERTIES_NAME;
                name_properties_content = AMZN_NAME_PROPERTIES_CONTENT;
                break;
            case NEWEGG:
                search_node_name = EGGZ_SEARCH_NODE_NAME;
                search_properties_name = EGGZ_SEARCH_PROPERTIES_NAME;
                search_properties_content = EGGZ_SEARCH_PROPERTIES_CONTENT;

                price_node_name = EGGZ_PRICE_NODE_NAME;
                price_properties_name = EGGZ_PRICE_PROPERTIES_NAME;
                price_properties_content = EGGZ_PRICE_PROPERTIES_CONTENT;

                name_node_name = EGGZ_NAME_NODE_NAME;
                name_properties_name = EGGZ_NAME_PROPERTIES_NAME;
                name_properties_content = EGGZ_NAME_PROPERTIES_CONTENT;
                break;
            default:
                break;
        }

        std::unique_ptr<Html_Setup> new_url(new Html_Setup(url_names[url]));
        scraper_init.push_back(std::move(new_url));
        curl_init_result = scraper_init[url]->curl_setup();
        if (curl_init_result == -1) {
            return -1;
        }

        scraper_init[url]->xml_setup();

        //TODO::output multiple results from the search
        
        find_search_results(scraper_init[url]->get_root_element(), search_results, found_results, search_node_name.c_str(), search_properties_name.c_str(), search_properties_content.c_str());
        if(search_results == NULL){
            std::cout << "No search results found" << std::endl;
            return -1;
        }
       
        std::cout << "URL: " << url_names[url] << std::endl;

        for(int item_num = 0; item_num < 3; item_num++){
            found_price = false;
            found_name = false;
            item_price = "";
            item_name = "";
      
            if(search_results == NULL){
                std::cout << "No search results found" << std::endl;
                break;
            }

            //TODO:: only output the actual search results and not to promotional ones

            std::cout << "Item " << item_num+1 << ": "<< std::endl;

            is_finding_price = false;
            find_item_content(search_results->children, item_name, found_name, url, is_finding_price, name_node_name.c_str(), name_properties_name.c_str(), name_properties_content.c_str());
            
            if(found_name == false){
                std::cout << "No Item Name Found" << std::endl;
            } 
            else{
                std::cout << item_name << std::endl;
            }

            is_finding_price = true;
            find_item_content(search_results->children, item_price, found_price, url, is_finding_price, price_node_name.c_str(), price_properties_name.c_str(), price_properties_content.c_str());


            if(found_price == false){
                std::cout << "No Price Found" << std::endl;
            }
            else{
                std::cout << item_price << std::endl;
            }
            std::cout << std::endl;

            search_results = search_results->next->next;

        }
        std::cout << "-----" << std::endl;
    }

    return 0;
}

//TODO::create function to return the title of the item being priced

bool search_html_properties(xmlAttr *html_properties_node, const char *properties_name, const char *properties_content){
    xmlAttr *search_html_node = html_properties_node;    

    while(search_html_node != NULL){
        if(strcmp(reinterpret_cast<const char*>(search_html_node->name), properties_name) == 0
                && strcmp(reinterpret_cast<const char*>(search_html_node->children->content), properties_content) == 0){
            return true;
        }
        search_html_node = search_html_node->next;
    }
    return false;
}

//Returns the first search result in the amazon list in order to move to the next item go ->next->next (or find the next div)
void find_search_results(xmlNode *html_tree_node, xmlNode* &search_result, bool &found_results, const char *node_name, const char *properties_name, const char *properties_content){
    if(html_tree_node == NULL || found_results){
        return;
    }

    if(html_tree_node->properties != NULL && html_tree_node->properties->children != NULL
            && strcmp(reinterpret_cast<const char*>(html_tree_node->name), node_name) == 0
            && search_html_properties(html_tree_node->properties, properties_name, properties_content)){
        found_results = true;
        search_result = html_tree_node;
    }

    find_search_results(html_tree_node->next, search_result, found_results, node_name, properties_name, properties_content);
    find_search_results(html_tree_node->children, search_result, found_results, node_name, properties_name, properties_content);
}


void find_item_content(xmlNode *html_tree_node, std::string &item_content, bool &found_content, int url, bool is_finding_price, const char *node_name, const char *properties_name, const char *properties_content){
    //In order to find the price I need to not only search through the children and next but properties node as well
    if(html_tree_node == NULL || found_content){
        return;
    }   

    if(html_tree_node->properties != NULL && html_tree_node->properties->children != NULL
            && strcmp(reinterpret_cast<const char*>(html_tree_node->name), node_name) == 0
            && search_html_properties(html_tree_node->properties, properties_name, properties_content)){
        if(is_finding_price && url == NEWEGG){
            find_newegg_price(html_tree_node, item_content, found_content, url, is_finding_price, EGGZ_PRICE_STRONG);
        }
        else{
            found_content = true;

            if(url == NEWEGG){
                save_content(html_tree_node, item_content, url, is_finding_price);
            }
            else{
                save_content(html_tree_node, item_content, url, is_finding_price);
            }
        }
    }

    find_item_content(html_tree_node->next, item_content, found_content, url, is_finding_price, node_name, properties_name, properties_content);
    find_item_content(html_tree_node->children, item_content, found_content, url, is_finding_price, node_name, properties_name, properties_content);
}

void find_newegg_price(xmlNode *html_tree_node, std::string &item_content, bool &found_content, int url, bool is_finding_price, const char *node_name){
    //In order to find the price I need to not only search through the children and next but properties node as well
    if(html_tree_node == NULL || found_content){
        return;
    }

    if(html_tree_node->children != NULL && strcmp(reinterpret_cast<const char*>(html_tree_node->name), node_name) == 0){
        found_content = true;
        save_content(html_tree_node, item_content, url, is_finding_price);
    }

    find_newegg_price(html_tree_node->next, item_content, found_content, url, is_finding_price, node_name);
    find_newegg_price(html_tree_node->children, item_content, found_content, url, is_finding_price, node_name);
}

void save_content(xmlNode *html_tree_node, std::string &content, int url, bool is_finding_price){
    
    if(html_tree_node->children->type == XML_CDATA_SECTION_NODE || html_tree_node->children->type == XML_TEXT_NODE){
        //TODO::Find way to save content here even if it contains '/n' (std::string s( reinterpret_cast<char const*>(uc), len)
        if(url == NEWEGG){
            if(is_finding_price){
                content = content + '$' + reinterpret_cast<char*>(html_tree_node->children->content);
                content += reinterpret_cast<char*>(html_tree_node->next->children->content);
            }
            else{
                content = reinterpret_cast<char*>(html_tree_node->children->content);
            }
        }
        else{
            // Amazon: finding item name and item price don't require any additional manipulation
            content = reinterpret_cast<char*>(html_tree_node->children->content);
        }
    }
    else{
        std::cerr << "Node is not of type XML_CDATA_SECTION_NODE" << std::endl;
    }
  
}

std::string spaces_to_underscores(std::string input_string){
    std::replace(input_string.begin(), input_string.end(), ' ', '+');
    return input_string;
}
