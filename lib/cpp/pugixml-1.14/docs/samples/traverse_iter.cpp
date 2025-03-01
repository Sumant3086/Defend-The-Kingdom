#include "pugixml.hpp"

#include <iostream>

int main()
{
    xml_document doc;
    if (!doc.load_file("xgconsole.xml")) return -1;

    xml_node tools = doc.child("Profile").child("Tools");

    // tag::code[]
    for (xml_node_iterator it = tools.begin(); it != tools.end(); ++it)
    {
        std::cout << "Tool:";

        for (xml_attribute_iterator ait = it->attributes_begin(); ait != it->attributes_end(); ++ait)
        {
            std::cout << " " << ait->name() << "=" << ait->value();
        }

        std::cout << std::endl;
    }
    // end::code[]
}

// vim:et
