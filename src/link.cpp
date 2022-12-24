#include "link.h"

#include "id_generator.h"
#include "utils.h"

Link::Link(std::shared_ptr<Node> start_node,
        std::shared_ptr<OutputPort> start_port,
        std::shared_ptr<Node> end_node,
        std::shared_ptr<InputPort> end_port):
    _start_node(start_node),
    _start_port(start_port),
    _end_node(end_node),
    _end_port(end_port)
{
    name_to_id::instance().getId(generate_uuid_v4(), _render_id);
}