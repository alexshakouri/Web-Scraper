#include "html_parse.h"

bool Html_Parse::search_properties(xmlAttrPtr html_properties_node, const char* properties_name, const char* properties_content){
    xmlAttrPtr search_html_node = html_properties_node;

    while(search_html_node != NULL){
        if((strcmp(reinterpret_cast<const char*>(search_html_node->name), properties_name) == MATCHING)
               && (strcmp(reinterpret_cast<const char*>(search_html_node->children->content), properties_content) == MATCHING)){
            return true;
        }
        search_html_node = search_html_node->next;
    }
    return false;
}

void Html_Parse::save_content(xmlNodePtr html_tree_node, std::string &content, int url, bool is_finding_price){
    if(html_tree_node->children->type != XML_CDATA_SECTION_NODE && html_tree_node->children->type != XML_TEXT_NODE){
        std::cerr << "Invalid node type for save_content()" << std::endl;
        return;
    }

    //TODO::Find way to save content here even if it contains '/n' (std::string s( reinterpret_cast<char const*>(uc), len)
    if(url == NEWEGG && is_finding_price){
        content = content + '$' + reinterpret_cast<char*>(html_tree_node->children->content);
        content += reinterpret_cast<char*>(html_tree_node->next->children->content);
    }
    else{
        // Amazon: finding item name and item price don't require any additional manipulation
        content = reinterpret_cast<char*>(html_tree_node->children->content);
    }
}

void Html_Parse::save_properties_content(xmlAttrPtr html_tree_node_properties, std::string &content, int url){    
    if(url == AMAZON){
        content += "https://www.amazon.com";
    }
    
    while(html_tree_node_properties != NULL){
        if(strcmp(reinterpret_cast<const char*>(html_tree_node_properties->name), "href") == MATCHING){
            content += reinterpret_cast<char*>(html_tree_node_properties->children->content);
            break;
        }
        html_tree_node_properties = html_tree_node_properties->next;
    } 
}

void Html_Parse::find_newegg_price(xmlNodePtr html_tree_node, std::string &item_price, bool &found_price, int url, bool is_finding_price){
    //In order to find the price I need to not only search through the children and next but properties node as well
    if(html_tree_node == NULL || found_price){
        return;
    }

    if(html_tree_node->children != NULL
            && (strcmp(reinterpret_cast<const char*>(html_tree_node->name), EGGZ_PRICE_DOLLAR) == MATCHING)){
        found_price = true;
        save_content(html_tree_node, item_price, url, is_finding_price);
    }

    find_newegg_price(html_tree_node->next, item_price, found_price, url, is_finding_price);
    find_newegg_price(html_tree_node->children, item_price, found_price, url, is_finding_price);
}

//Returns the first search result in the amazon list in order to move to the next item go ->next->next (or find the next div)
void Html_Parse::find_search_results(xmlNodePtr html_tree_node, xmlNodePtr &search_result, bool &found_results, const char* node_name, const char* properties_name, const char* properties_content){
    if((html_tree_node == NULL) || found_results){
        return;
    }

    if(html_tree_node->properties != NULL && html_tree_node->properties->children != NULL){
        if((strcmp(reinterpret_cast<const char*>(html_tree_node->name), node_name) == MATCHING)
                && search_properties(html_tree_node->properties, properties_name, properties_content)){
            found_results = true;
            search_result = html_tree_node;
        }
    }

    find_search_results(html_tree_node->next, search_result, found_results, node_name, properties_name, properties_content);
    find_search_results(html_tree_node->children, search_result, found_results, node_name, properties_name, properties_content);
}

//TODO::create enum for is_finding*
void Html_Parse::find_item_content(xmlNodePtr html_tree_node, std::string &item_content, bool &found_content, int url, bool is_finding_price, bool is_finding_url, const char *node_name, const char *properties_name, const char *properties_content){
    //In order to find the price I need to not only search through the children and next but properties node as well
    if((html_tree_node == NULL) || found_content){
        return;
    }

    if(html_tree_node->properties != NULL && html_tree_node->properties->children != NULL){
        if((strcmp(reinterpret_cast<const char*>(html_tree_node->name), node_name) == MATCHING)
                && search_properties(html_tree_node->properties, properties_name, properties_content)){
            if(is_finding_url){
                found_content = true;
                save_properties_content(html_tree_node->properties, item_content, url);
            }
            else if(is_finding_price && (url == NEWEGG)){
                find_newegg_price(html_tree_node, item_content, found_content, url, is_finding_price);
            }
            else{
                found_content = true;
                save_content(html_tree_node, item_content, url, is_finding_price);
            }
        }
    }

    find_item_content(html_tree_node->next, item_content, found_content, url, is_finding_price, is_finding_url, node_name, properties_name, properties_content);
    find_item_content(html_tree_node->children, item_content, found_content, url, is_finding_price, is_finding_url, node_name, properties_name, properties_content);
}

std::vector<item> Html_Parse::get_website_items(Html_Setup *scraper_init, int url) {
    std::vector<item> items_list;
    struct item item_mem;

    xmlNode *search_results = NULL;
    xmlNode *search_results_not_promotional = NULL;
    bool is_finding_price = false;
    bool is_finding_url = false;
    bool found_results = false;
    bool found_not_promotional = false;
    bool found_name = false;
    bool found_price = false;
    bool found_url = false;
    std::string item_price = "";
    std::string item_name = "";
    std::string item_url = "";

    std::string search_node_name = "";
    std::string search_properties_name = "";
    std::string search_properties_content = "";

    std::string price_node_name = "";
    std::string price_properties_name = "";
    std::string price_properties_content = "";

    std::string name_node_name = "";
    std::string name_properties_name = "";
    std::string name_properties_content = "";

    std::string url_node_name = "";
    std::string url_properties_name = "";
    std::string url_properties_content = "";

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

            url_node_name = AMZN_URL_NODE_NAME;
            url_properties_name = AMZN_URL_PROPERTIES_NAME;
            url_properties_content = AMZN_URL_PROPERTIES_CONTENT;

            break;
        case NEWEGG:
            price_node_name = EGGZ_PRICE_NODE_NAME;
            price_properties_name = EGGZ_PRICE_PROPERTIES_NAME;
            price_properties_content = EGGZ_PRICE_PROPERTIES_CONTENT;

            name_node_name = EGGZ_NAME_NODE_NAME;
            name_properties_name = EGGZ_NAME_PROPERTIES_NAME;
            name_properties_content = EGGZ_NAME_PROPERTIES_CONTENT;

            url_node_name = EGGZ_URL_NODE_NAME;
            url_properties_name = EGGZ_URL_PROPERTIES_NAME;
            url_properties_content = EGGZ_URL_PROPERTIES_CONTENT;

            search_node_name = EGGZ_SEARCH_NODE_NAME;
            search_properties_name = EGGZ_SEARCH_PROPERTIES_NAME;
            search_properties_content = EGGZ_SEARCH_PROPERTIES_CONTENT;

            break;
        default:
            break;
    }

    //
    //found_results = false;
    //search_results = NULL;


    find_search_results(scraper_init->get_root_element(), search_results, found_results, search_node_name.c_str(), search_properties_name.c_str(), search_properties_content.c_str());
    if(search_results == NULL){
        std::cout << "No search results found" << std::endl;
        return items_list;
    }
    if(url == NEWEGG){
        search_results = search_results->children;
    }

    for(int item_num = 0; item_num < ITEMS_PER_WEBSITE; item_num++){
        found_price = false;
        found_name = false;
        found_url = false;
        item_price = "";
        item_name = "";
        item_url = "";
        found_not_promotional = false;
        search_results_not_promotional = NULL;

        if(search_results == NULL){
            break;
        }

        //only return main results for AMZN
        if(url == AMAZON){
            find_search_results(search_results, search_results_not_promotional, found_not_promotional, AMZN_PROMO_NODE_NAME, AMZN_PROMO_PROPERTIES_NAME, AMZN_PROMO_PROPERTIES_CONTENT);
            if(!found_not_promotional){
                std::cout << "Skipping promotional content" << std::endl;
                item_num--;
                search_results = search_results->next->next;
                continue;
            }
        }

        is_finding_price = false;
        is_finding_url = false;
        find_item_content(search_results->children, item_name, found_name, url, is_finding_price, is_finding_url, name_node_name.c_str(), name_properties_name.c_str(), name_properties_content.c_str());

        if(found_name == false){
            item_mem.name = "No Item Name Found";
        }
        else{
            item_mem.name = item_name;
        }

        is_finding_price = true;
        is_finding_url = false;
        find_item_content(search_results->children, item_price, found_price, url, is_finding_price, is_finding_url, price_node_name.c_str(), price_properties_name.c_str(), price_properties_content.c_str());

        if(found_price == false){
            item_mem.price = "No Price Found";
        }
        else{
            item_mem.price = item_price;
        }

        is_finding_price = false;
        is_finding_url = true;
        find_item_content(search_results->children, item_url, found_url, url, is_finding_price, is_finding_url, url_node_name.c_str(), url_properties_name.c_str(), url_properties_content.c_str());
        is_finding_url = false;

        if(found_url == false){
            item_mem.url = "No URL Found";
        }
        else{
            item_mem.url = item_url;
        }

        //TODO::create function that returns next search result for different websites
        if (url == AMAZON) {
            search_results = search_results->next->next;
        }
        else if (url == NEWEGG) {
            search_results = search_results->next;
        }

        items_list.push_back(item_mem);
    }

    return items_list;
}

