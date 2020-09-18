#ifndef HTML_PARSE_H
#define HTML_PARSE_H

#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <libxml/HTMLparser.h>
#include "html_setup.h"
#include "website_globals.h"

#define MATCHING 0
#define ITEMS_PER_WEBSITE 3

//TODO::create a click function where we sort the output by featured

struct item {
    std::string name;
    std::string price;
    std::string url;
};

class Html_Parse {
private:
    bool search_properties(xmlAttrPtr html_properties_node, const char* properties_name, const char* properties_content);
    void save_content(xmlNodePtr html_tree_node, std::string &content, int url, bool is_finding_price);
    void find_newegg_price(xmlNodePtr html_tree_node, std::string &item_price, bool &found_price, int url, bool is_finding_price);
    void save_properties_content(xmlAttrPtr html_tree_node, std::string &content, int url);

public:
    //TODO::create function get_item() (for finding item price and item name)
    void find_search_results(xmlNodePtr html_tree_node, xmlNodePtr &search_result, bool &found_results, const char* node_name, const char* properties_name, const char* properties_content);
    void find_item_content(xmlNodePtr html_tree_node, std::string &item_content, bool &found_content, int url, bool is_finding_price, bool is_finding_url, const char *node_name, const char *properties_name, const char *properties_content);
    std::vector<item> get_website_items(Html_Setup *scraper_init, int url);
};


#endif //HTML_PARSE_H
