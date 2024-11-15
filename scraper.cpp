#include <iostream>
#include <curl/curl.h>
#include <gumbo.h>



size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp){
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string fetchHTML(const std::string& url){
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();

    if(curl){
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        curl_easy_cleanup(curl);
    }
    return readBuffer;
}

std::string getText(GumboNode *node)
{
    if (node->type == GUMBO_NODE_TEXT)
    {
        return std::string(node->v.text.text);
    }
    else if (node->type == GUMBO_NODE_ELEMENT)
    {
        std::string result;
        GumboVector *children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i)
        {
            GumboNode *child = static_cast<GumboNode *>(children->data[i]);
            result += getText(child);
        }
        return result;
    }
    return "";
}

void search_for_claims(GumboNode* node)
{
    if(node-> type != GUMBO_NODE_ELEMENT) return;
    std::cout <<"Inspcting node.. \n ";
    GumboAttribute* classAttr;

    if(node-> v.element.tag == GUMBO_TAG_DIV && 
    (classAttr = gumbo_get_attribute(&node->v.element.attributes,"class"))){
          
          // Extract file
            std::cout << "Found dive with class: \n"<< classAttr-> value<<"\n";

    if(std::string(classAttr->value)== "white-bkgrnd open_to_claims"){
        std::cout << "found a claim block: \n";
    
            GumboVector* children = &node->v.element.children;

            for(unsigned int i =0; i< children->length; i++){
                GumboNode* child = static_cast< GumboNode* >(children-> data[i]);
                if(child-> type == GUMBO_NODE_ELEMENT && child->v.element.tag==GUMBO_TAG_H2){
                    //title
                    std::cout << "Title: " <<getText(child)<<std::endl;
                }
            }
        }

    }
        GumboVector* children = &node->v.element.children;
        for (unsigned int i =0; i<children-> length; i++){
            search_for_claims(static_cast<GumboNode*> (children->data[i]));
        }
}

std::string gumbo_get_inner_text(GumboNode* node){
    if(node->type == GUMBO_NODE_TEXT){
        return std::string(node->v.text.text);
    }else if(node->type == GUMBO_NODE_ELEMENT){
        std::string result;
        GumboVector* children = &node->v.element.children;
        for(unsigned int i =0; i < children->length; ++i){
            GumboNode* child = static_cast<GumboNode*>(children->data[i]);
            result += gumbo_get_inner_text(child);
        }
        return result;
    }
    return "";
}


int main(){
    std::string url = "https://www.consumer-action.org/lawsuits/by-status/open";
    std::string htmlContent = fetchHTML(url);
    std::cout << "Fetched HTML conten first 500 char : \n"<< htmlContent.substr(0,500)<< "\n\n";
    GumboOutput* output= gumbo_parse(htmlContent.c_str());
    search_for_claims(output->root);
    gumbo_destroy_output(&kGumboDefaultOptions, output);
    //std::cout << "HTML CONTENT: \n"<< htmlContent<<std::endl;
    return 0;
    }