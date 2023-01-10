function onOpenGraph(graph_id) {
    console.log(`Graph ${graph_id} opened`);
}

function onCloseGraph(graph_id) {
    console.log(`Graph ${graph_} closed`)
}

function onNodeCreated(graph_id, node_id, node_hdl) {
    console.log(`Node ${node_hdl} with id: ${node_id} created in graph: ${graph_id}`);
}