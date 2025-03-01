#include "pugixml.hpp"

#include <iostream>

int main()
{
// tag::code[]
    xml_document doc;

    xml_parse_result result = doc.load_file("tree.xml");

    std::cout << "Load result: " << result.description() << ", mesh name: " << doc.child("mesh").attribute("name").value() << std::endl;
// end::code[]
}

// vim:et
