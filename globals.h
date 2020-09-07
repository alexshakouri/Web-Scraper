#ifndef GLOBALS_H
#define GLOBALS_H

#define MATCHING 0

#define AMAZON 0
#define NEWEGG 1

//Find the first search result
#define AMZN_SEARCH_NODE_NAME "div"
#define AMZN_SEARCH_PROPERTIES_NAME "data-index"
#define AMZN_SEARCH_PROPERTIES_CONTENT "1"

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
#define EGGZ_SEARCH_PROPERTIES_CONTENT "item-cell"

#define EGGZ_PRICE_NODE_NAME "span"
#define EGGZ_PRICE_PROPERTIES_NAME "class"
#define EGGZ_PRICE_PROPERTIES_CONTENT "price-current-label"

#define EGGZ_PRICE_DOLLAR "strong"
#define EGGZ_PRICE_CENT "sup"

#define EGGZ_NAME_NODE_NAME "a"
#define EGGZ_NAME_PROPERTIES_NAME "class"
#define EGGZ_NAME_PROPERTIES_CONTENT "item-title"

#endif //GLOBALS_H
