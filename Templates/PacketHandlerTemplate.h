#pragma once
#include "Protocol.h"
#include "PacketHeader.h"
#include "PacketHandler.h"
#include "{{parser.package_name}}.pb.h"

enum {{parser.package_name_upper}} : int
{
{%- for pkt in parser.total_pkt %}
	PKT_{{pkt.name}} = {{pkt.id}},
{%- endfor %}

	{{parser.package_name_upper}}_START = {{parser.start_id}},
	{{parser.package_name_upper}}_END = {{parser.end_id}}
};

{%- for pkt in parser.total_pkt %}
USE_PROTOCOL({{parser.package_name}}::{{pkt.name}});
{%- endfor %}

class {{parser.package_name}}PacketHandler : public PacketHandler
{
public:
	virtual bool IsValidProtocol(int protocol) override
	{
		return protocol >= {{parser.package_name_upper}}_START && protocol <= {{parser.package_name_upper}}_END;
	}

	virtual void HandleRecv(SessionPtrCRef session, const PacketHeader& header, char* buffer) override
	{
		switch (header.protocol)
		{
{%- for pkt in parser.total_pkt %}
		case PKT_{{pkt.name}}:
			on{{pkt.name_res}}(session, parse<{{parser.package_name}}::{{pkt.name}}>(buffer, header.size));
			break;
{%- endfor %}
		}
	}

public:
{%- for pkt in parser.total_pkt%}
	virtual void on{{pkt.name_res}}(SessionPtrCRef, {{parser.package_name}}::{{pkt.name}} pkt) {}
{%- endfor%}
};