#include "html_setup.h"
#include "html_parse.h"
#include <algorithm>
#include <memory>

//TODO::output value from each website sorted by (lowest price, rating, number of reviews)

//TODO::output the item URL so that the user can go straight to the item

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
    std::vector<std::vector<item>> items_list;
    std::vector<item> website_items;
    int curl_init_result = 0;

    Html_Parse parse_websites;

    for(unsigned int url = 0; url < url_names.size(); url++){
        curl_init_result = 0;

        std::unique_ptr<Html_Setup> new_url(new Html_Setup(url_names[url]));
        scraper_init.push_back(std::move(new_url));
        curl_init_result = scraper_init[url]->curl_setup();
        if (curl_init_result == -1) {
            std::cout << "Curl initialization failed." << std::endl;
            break;
        }
        scraper_init[url]->xml_setup();

        website_items = parse_websites.get_website_items(scraper_init[url].get(), url);
        items_list.push_back(website_items);
        website_items.clear();

        std::cout << "URL: " << url_names[url] << std::endl << std::endl;

        for (unsigned int i = 0; i < items_list[url].size(); i++) {
            if(!items_list[url].empty()) {
                std::cout << "Item " << i+1 << ": "<< std::endl;
                std::cout << items_list[url][i].name << std::endl;
                std::cout << items_list[url][i].price << std::endl;
                std::cout << items_list[url][i].url << std::endl;
                std::cout << std::endl;
            }
        }

        std::cout << "-----" << std::endl;
    }

    return 0;
}

std::string spaces_to_underscores(std::string input_string){
    std::replace(input_string.begin(), input_string.end(), ' ', '+');
    return input_string;
}
