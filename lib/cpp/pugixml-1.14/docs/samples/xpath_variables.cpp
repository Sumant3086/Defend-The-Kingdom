#include "pugixml.hpp"

#include <iostream>
#include <string>

int main()
{
    xml_document doc;
    if (!doc.load_file("xgconsole.xml")) return -1;

// tag::code[]
    // Select nodes via compiled query
    xpath_variable_set vars;
    vars.add("remote", xpath_type_boolean);

    xpath_query query_remote_tools("/Profile/Tools/Tool[@AllowRemote = string($remote)]", &vars);

    vars.set("remote", true);
    xpath_node_set tools_remote = query_remote_tools.evaluate_node_set(doc);

    vars.set("remote", false);
    xpath_node_set tools_local = query_remote_tools.evaluate_node_set(doc);

    std::cout << "Remote tool: ";
    tools_remote[2].node().print(std::cout);

    std::cout << "Local tool: ";
    tools_local[0].node().print(std::cout);

    // You can pass the context directly to select_nodes/select_node
    xpath_node_set tools_local_imm = doc.select_nodes("/Profile/Tools/Tool[@AllowRemote = string($remote)]", &vars);

    std::cout << "Local tool imm: ";
    tools_local_imm[0].node().print(std::cout);
// end::code[]
}

// vim:et
