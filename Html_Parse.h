#ifndef HTML_PARSE_H
#define HTML_PARSE_H

#include <iostream>
#include <string>
#include <string.h>
#include <libxml/HTMLparser.h>

#define MATCHING 0

//TODO::create a click function where we sort the output by featured
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

//Find if item is from the search result
#define AMZN_PROMO_NODE_NAME "span"
#define AMZN_PROMO_PROPERTIES_NAME "cel_widget_id"
#define AMZN_PROMO_PROPERTIES_CONTENT "MAIN-SEARCH_RESULTS"

//TODO::NEWEGG returns max 4 items atm, search for class="list_wrap"
#define EGGZ_SEARCH_NODE_NAME "div"
#define EGGZ_SEARCH_PROPERTIES_NAME "class"
#define EGGZ_SEARCH_PROPERTIES_CONTENT "item-cells-wrap border-cells items-grid-view four-cells expulsion-one-cell"

#define EGGZ_PRICE_NODE_NAME "span"
#define EGGZ_PRICE_PROPERTIES_NAME "class"
#define EGGZ_PRICE_PROPERTIES_CONTENT "price-current-label"

#define EGGZ_PRICE_DOLLAR "strong"
#define EGGZ_PRICE_CENT "sup"

#define EGGZ_NAME_NODE_NAME "a"
#define EGGZ_NAME_PROPERTIES_NAME "class"
#define EGGZ_NAME_PROPERTIES_CONTENT "item-title"

class Html_Parse {
private:
    bool search_properties(xmlAttrPtr html_properties_node, const char* properties_name, const char* properties_content);
    void save_content(xmlNodePtr html_tree_node, std::string &content, int url, bool is_finding_price);
    void find_newegg_price(xmlNodePtr html_tree_node, std::string &item_price, bool &found_price, int url, bool is_finding_price);

public:
    //TODO::create function get_item() (for finding item price and item name)
    void find_search_results(xmlNodePtr html_tree_node, xmlNodePtr &search_result, bool &found_results, const char* node_name, const char* properties_name, const char* properties_content);
    void find_item_content(xmlNodePtr html_tree_node, std::string &item_content, bool &found_content, int url, bool is_finding_price, const char *node_name, const char *properties_name, const char *properties_content);
};


#endif //HTML_PARSE_H
