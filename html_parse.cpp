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

void Html_Parse::find_item_content(xmlNodePtr html_tree_node, std::string &item_content, bool &found_content, int url, bool is_finding_price, const char *node_name, const char *properties_name, const char *properties_content){
    //In order to find the price I need to not only search through the children and next but properties node as well
    if((html_tree_node == NULL) || found_content){
        return;
    }

    if(html_tree_node->properties != NULL && html_tree_node->properties->children != NULL){
        if((strcmp(reinterpret_cast<const char*>(html_tree_node->name), node_name) == MATCHING)
                && search_properties(html_tree_node->properties, properties_name, properties_content)){
            if(is_finding_price && (url == NEWEGG)){
                find_newegg_price(html_tree_node, item_content, found_content, url, is_finding_price);
            }
            else{
                found_content = true;
                save_content(html_tree_node, item_content, url, is_finding_price);
            }
        }
    }

    find_item_content(html_tree_node->next, item_content, found_content, url, is_finding_price, node_name, properties_name, properties_content);
    find_item_content(html_tree_node->children, item_content, found_content, url, is_finding_price, node_name, properties_name, properties_content);
}

