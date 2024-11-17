
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <gumbo.h>

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

void search_for_claims(GumboNode *node)
{
    if (node->type != GUMBO_NODE_ELEMENT)
        return;

    GumboAttribute *classAttr = gumbo_get_attribute(&node->v.element.attributes, "class");
    if (classAttr && std::string(classAttr->value).find("white-bkgrnd open_to_claims") != std::string::npos)
    {
        // title
        GumboVector *children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i)
        {
            GumboNode *child = static_cast<GumboNode *>(children->data[i]);
            if (child->v.element.tag == GUMBO_TAG_H2)
            {
                std::cout << "Title: " << gumbo_get_inner_text(child) << std::endl;
            }
            if (child->v.element.tag == GUMBO_TAG_P)
            {
                std::cout << "Details: " << gumbo_get_inner_text(child) << std::endl;
            }
        }
    }

    //Children
    GumboVector *children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; ++i)
    {
        search_for_claims(static_cast<GumboNode *>(children->data[i]));
    }
}

int main(){

    std::string url = "https://www.consumer-action.org/lawsuits/by-status/open";

    CURL *curl;
    CURLcode res;
    std::string htmlContent;

    curl = curl_easy_init();
        if (curl){
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &htmlContent);
        res = curl_easy_perform(curl);

         if (res != CURLE_OK){
            std::cerr << "CURL failed: " << curl_easy_strerror(res) << std::endl;
            return 1;
        }

            curl_easy_cleanup(curl);
    }
            else {
        std::cerr << "CURL initialization failed." << std::endl;
        return 1;
    }

    std::cout << "Fetched HTML content first 500 characters:\n";
    std::cout << htmlContent.substr(0, 500) << std::endl;

    // html parsing 
    GumboOutput *output = gumbo_parse(htmlContent.c_str());
    search_for_claims(output->root);
    gumbo_destroy_output(&kGumboDefaultOptions, output);

    return 0;
}
