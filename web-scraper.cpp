#include "Html_Setup.h"
#include "Html_Parse.h"
#include <algorithm>
#include <vector>
#include <memory>

//TODO::output value from each website sorted by (lowest price, rating, number of reviews)

//TODO::output the item URL so that the user can go straight to the item

//TODO::move global variables to its own file
#define ITEMS_PER_WEBSITE 3

//TODO::implement for multiple websites
//TODO::implement search functions as a class
//TODO::have functions return bool found_content
std::string spaces_to_underscores(std::string user_input);

//TODO::include user input with error checks (multiple arguements)
int main(int argc, char *argv[]){
    std::string user_input = "";
    if(argc == 1){
        user_input = "switch_games";
        std::cerr << "Defaulting search to '" << user_input << "'." << std::endl;
    }
    else if (argc > 2){
        for (int i = 1; i < argc; i++) {
            user_input = user_input + argv[i] + "_";
        }
        user_input.pop_back();
        std::cout << "User Input: " << user_input << std::endl;
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
    bool found_not_promotional = false;
    bool found_price = false;
    bool found_name = false;
    bool is_finding_price = false;
    std::string item_price = "";
    std::string item_name = "";
    xmlNode *search_results = NULL;
    xmlNode *search_results_not_promotional = NULL; //Variable current not in use

    std::string search_node_name = "";
    std::string search_properties_name = "";
    std::string search_properties_content = "";

    std::string price_node_name = "";
    std::string price_properties_name = "";
    std::string price_properties_content = "";

    std::string name_node_name = "";
    std::string name_properties_name = "";
    std::string name_properties_content = "";

    Html_Parse parse_websites;

    for(unsigned int url = 0; url < url_names.size(); url++){
        curl_init_result = 0;
        found_results = false;
        search_results = NULL;

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

        parse_websites.find_search_results(scraper_init[url]->get_root_element(), search_results, found_results, search_node_name.c_str(), search_properties_name.c_str(), search_properties_content.c_str());
        if(search_results == NULL){
            std::cout << "No search results found" << std::endl;
            return -1;
        }
         
        std::cout << "URL: " << url_names[url] << std::endl;

        for(int item_num = 0; item_num < ITEMS_PER_WEBSITE; item_num++){
            found_price = false;
            found_name = false;
            item_price = "";
            item_name = "";
            found_not_promotional = false;
            search_results_not_promotional = NULL;

            
            if(search_results == NULL){
                std::cout << "No search results found" << std::endl;
                break;
            }

            //only return main results for AMZN
            if(url == AMAZON){
                parse_websites.find_search_results(search_results, search_results_not_promotional, found_not_promotional, AMZN_PROMO_NODE_NAME, AMZN_PROMO_PROPERTIES_NAME, AMZN_PROMO_PROPERTIES_CONTENT);
                if(!found_not_promotional){
                    std::cout << "Skipping promotional content" << std::endl;
                    item_num--;
                    search_results = search_results->next->next;
                    continue;
                }
            }
        
            std::cout << "Item " << item_num+1 << ": "<< std::endl;

            is_finding_price = false;
            parse_websites.find_item_content(search_results->children, item_name, found_name, url, is_finding_price, name_node_name.c_str(), name_properties_name.c_str(), name_properties_content.c_str());
            
            if(found_name == false){
                std::cout << "No Item Name Found" << std::endl;
            } 
            else{
                std::cout << item_name << std::endl;
            }

            is_finding_price = true;
            parse_websites.find_item_content(search_results->children, item_price, found_price, url, is_finding_price, price_node_name.c_str(), price_properties_name.c_str(), price_properties_content.c_str());


            if(found_price == false){
                std::cout << "No Price Found" << std::endl;
            }
            else{
                std::cout << item_price << std::endl;
            }
            std::cout << std::endl;

            //TODO::create function that returns next search result for different websites
            search_results = search_results->next->next;

        }
        std::cout << "-----" << std::endl;
    }

    return 0;
}

std::string spaces_to_underscores(std::string input_string){
    std::replace(input_string.begin(), input_string.end(), ' ', '+');
    return input_string;
}
