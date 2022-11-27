#include "{{parser.package_name}}Handler.h"

{%- for pkt in parser.total_pkt %}
DEFINE_PROTOCOL({{parser.package_name}}::{{pkt.name}}, PKT_{{pkt.name}});
{%- endfor %}