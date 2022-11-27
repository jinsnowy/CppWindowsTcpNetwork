
import os
import argparse
import jinja2
import ProtoParser

def main():
	arg_parser = argparse.ArgumentParser(description = 'ProtocolGenerator')
	arg_parser.add_argument('--path', type=str, default='C:/Users/yhjin/Desktop/CppProject/ynet/UserProtocol.proto', help='proto path')
	arg_parser.add_argument('--out', type=str, default='./Generated/')
	args = arg_parser.parse_args()

	my_parser = ProtoParser.ProtoParser(1000)
	my_parser.doParse(args.path)

	file_loader = jinja2.FileSystemLoader('Templates')
	env = jinja2.Environment(loader=file_loader)

	try:
		if not os.path.exists(args.out):
			os.makedirs(args.out)
	except OSError:
		print ('Error: Creating directory. ' +  args.out)

	outfile_name_base = args.out + my_parser.get_package_name() + 'Handler'

	target_template_files = ['PacketHandlerTemplate.h', 'PacketHandlerTemplate.cpp']

	for template_file in target_template_files:
		template = env.get_template(template_file)
		output = template.render(parser=my_parser, output=outfile_name_base)
		
		output_file = outfile_name_base + '.h' if '.h' in template_file else outfile_name_base + '.cpp' 
		f = open(output_file, 'w+')
		f.write(output)
		f.close()

		print(output)

	return

if __name__ == '__main__':
	main()