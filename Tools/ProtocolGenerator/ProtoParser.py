
class ProtoParser():
	def __init__(self, start_id):
		self.total_pkt = []
		self.start_id = start_id
		self.end_id = 0
		self.id = start_id
		self.package_name = ''
		self.package_name_upper = ''

	def get_package_name(self):
		return self.package_name

	def doParse(self, path):
		f = open(path, 'r')
		lines = f.readlines()

		for line in lines:
			if line.startswith('package'):
				self.package_name = line.split()[1].strip()[:-1]
				self.package_name_upper=  self.package_name.upper()

			if line.startswith('message') == False:
				continue

			pkt_name = line.split()[1]
			pkt_name_res = ""
			pkt_name_items = pkt_name.split('_')
			for item in pkt_name_items:
				pkt_name_res += item[0].upper() + item[1:].lower()

			self.total_pkt.append(Packet(pkt_name, pkt_name_res, self.id))
			self.id += 1

		self.end_id = self.id - 1

		f.close()

class Packet:
	def __init__(self, name, name_res, id):
		self.name = name
		self.name_res = name_res
		self.id = id