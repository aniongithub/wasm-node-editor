var graphs = 
{
    "time": {
        "outputs": {
            "time_sec": "float"
        }
    },
    "functions/sine": {
        "inputs": {
            "baseline": "float",
            "phase": "float",
            "amplitude": "float",
            "frequencyHz": "float",
            "x": "float"
        },
        "outputs": {
            "y": "float"
        }
    },
    "functions/square": {
        "inputs": {
            "baseline": "float",
            "phase": "float",
            "amplitude": "float",
            "frequencyHz": "float",
            "dutyCycle": "float",
            "x": "float"
        },
        "outputs": {
            "y": "float"
        }
    },
    "functions/sawtooth": {
        "inputs": {
            "baseline": "float",
            "phase": "float",
            "amplitude": "float",
            "frequencyHz": "float",
            "x": "float"
        },
        "outputs": {
            "y": "float"
        }
    },
    "functions/triangle": {
        "inputs": {
            "baseline": "float",
            "phase": "float",
            "amplitude": "float",
            "frequencyHz": "float",
            "x": "float"
        },
        "outputs": {
            "y": "float"
        }
    },
    "math/const_float": {
        "outputs": {
            "value": "float"
        },
        "properties": {
            "value": "float"
        }
    },
    "math/add": {
        "inputs": {
            "a": "float",
            "b": "float"
        },
        "outputs": {
            "result": "float"
        }
    },
    "math/subtract": {
        "inputs": {
            "a": "float",
            "b": "float"
        },
        "outputs": {
            "result": "float"
        }
    },
    "math/multiply": {
        "inputs": {
            "a": "float",
            "b": "float"
        },
        "outputs": {
            "result": "float"
        }
    }
};

function* graphEnumerator() {
    for (let id in graphs) {
        result = {}
        result[id] = graphs[id]
        yield JSON.stringify(result);
    }
}

var graphGenerator = null;

function onEnumerateGraphs() {
    try {
        if (graphGenerator == null)
            graphGenerator = graphEnumerator();
        return graphGenerator.next().value;
    } catch (e) 
    {
        graphGenerator = null;
        return null;
    }
}

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