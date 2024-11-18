
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include <gumbo.h>
#include "json.hpp"

using json = nlohmann::json;

size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *userp){
    size_t totalSize = size * nmemb;
    userp->append((char *)contents, totalSize);
    return totalSize;
}

std::string gumbo_get_inner_text(GumboNode *node){
    if (node->type == GUMBO_NODE_TEXT){
        return std::string(node->v.text.text);
    }
    else if (node->type == GUMBO_NODE_ELEMENT){
        std::string result;
        GumboVector *children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i){
            GumboNode *child = static_cast<GumboNode *>(children->data[i]);
            result += gumbo_get_inner_text(child);
        }
        return result;
    }
    return "";
}

void search_for_claims(GumboNode *node, json& lawsuits)
{
    if (node->type != GUMBO_NODE_ELEMENT)
        return;

    GumboAttribute *classAttr = gumbo_get_attribute(&node->v.element.attributes, "class");
    if (classAttr && std::string(classAttr->value).find("white-bkgrnd open") != std::string::npos)
    {
        json lawsuit;
        GumboVector *children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i)
        {
            GumboNode *child = static_cast<GumboNode *>(children->data[i]);
            if (child->type == GUMBO_NODE_ELEMENT)
            {
                GumboAttribute *childClassAttr = gumbo_get_attribute(&child->v.element.attributes, "class");
                if (childClassAttr)
                {
                    std::string classValue = std::string(childClassAttr->value);

                    // Extract the title
                    if (classValue == "action-title")
                    {
                        lawsuit["title"] = gumbo_get_inner_text(child);
                    }

                    // Extract the defendant information
                    if (classValue == "grey-note")
                    {
                        lawsuit["defendant"] = gumbo_get_inner_text(child);
                    }
                }
            }

            // Extract other details, such as claim deadlines
            if (child->type == GUMBO_NODE_ELEMENT && child->v.element.tag == GUMBO_TAG_P)
            {
                std::string text = gumbo_get_inner_text(child);
                if (text.find("Claim deadline") != std::string::npos)
                {
                    lawsuit["deadline"] = text;
                }
                else if (!text.empty() && lawsuit.find("details") == lawsuit.end())
                {
                    lawsuit["details"] = text;
                }
            }
        }
        lawsuits.push_back(lawsuit);
    }

    // Process child nodes recursively
    GumboVector *children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; ++i)
    {
        search_for_claims(static_cast<GumboNode *>(children->data[i]), lawsuits);
    }
}

int main()
{
    std::string url = "https://www.consumer-action.org/lawsuits";
    std::string htmlContent;

    CURL *curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &htmlContent);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36");

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "CURL Error: " << curl_easy_strerror(res) << std::endl;
            curl_easy_cleanup(curl);
            return 1;
        }
        curl_easy_cleanup(curl);
    }
    std::ofstream htmlFile("page.html");
    if (htmlFile.is_open())
    {
        htmlFile << htmlContent;
        htmlFile.close();
        std::cout << "Full HTML content saved to page.html\n";
    }

    // gumbo parsing html
    GumboOutput *output = gumbo_parse(htmlContent.c_str());
    json lawsuits = json::array(); 
    search_for_claims(output->root, lawsuits);
    std::cout <<" Extracted Lawsuits:" << lawsuits.dump(4)<<std::endl;
    gumbo_destroy_output(&kGumboDefaultOptions, output);

    std::ofstream outFile("lawsuits.json");
    if (outFile.is_open())
    {
        if (lawsuits.empty())
        {
            std::cerr << "No data to write. Lawsuits JSON is empty!" << std::endl;
        }
        else
        {
            outFile << lawsuits.dump(4);
            std::cout << "JSON data: " << lawsuits.dump(4) << std::endl;
            std::cout << "Lawsuits data saved to lawsuits.json" << std::endl;
        }
        outFile.close();
    }
    else
    {
        std::cerr << "Error opening file for writing!" << std::endl;
    }

    return 0;
}