function onOpenGraph(graph_id) {
    console.log(`Graph ${graph_id} opened`);
    return "[]"
}

function onCloseGraph(graph_id) {
    console.log(`Graph ${graph_} closed`)
}

function onNodeCreated(graph_id, node_id, node_hdl) {
    console.log(`Node ${node_hdl} with id: ${node_id} created in graph: ${graph_id}`);
}

/*
function onEnumerateGraphs();
function onOpenGraph(graph_id);
function onCloseGraph(graph_id);
function onNodeCreated(graph_id, node_id, node_hdl);
function onNodeDestroyed(graph_id, node_hdl);
function onNodePropertyChanged(graph_id, node_hdl, property_name, type, new_value);
function onLinkCreated(graph_id, src_node_hdl, src_port_name, dst_node_hdl, dst_port_name, link_hdl);
function onLinkDestroyed(graph_id, link_hdl);

*/