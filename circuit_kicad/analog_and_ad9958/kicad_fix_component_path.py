#kicad_fix_component_path.py
'''
Этот скрипт позволяет сделать правильные пути в kicad файлах после объединения.
1. Создаем новый проект.
2. Создаем hierarhical chematic sheet и копируем в них sch от проектов (копируем прямо файлы).
3. запускаем kicadpcb (напрямую из командной строки) и пользуемся File->Append Board пунктиом меню.
4. Запускакем этот скрипт с именем проекта. Получается файл с расширением .kicad_pcb.out в котором
   правильные reference на hierarhical sheet компоненты.
'''

class SchInfo:
	def __init__(self):
		name = None
		U = None
		components = None
		pass

def get_sch_name_and_U(root_sch_name):
	f = open(root_sch_name, "rt")
	names = []

	sheet_enter = False
	last_U = None
	last_Name = None

	for line in f:
		if len(line)==0:
			continue
		if line[-1]=="\n":
			line=line[:-1]

		if line == "$Sheet":
			sheet_enter = True
		if line == "$EndSheet":
			sheet_enter = False
			info = SchInfo()
			info.name = last_Name
			info.U = last_U
			names.append( info )

		if line[0]=='U':
			last_U = line[2:]

		if line[:2]=='F1':
			idx1 = line.find("\"")
			assert(idx1>0)
			idx1 += 1
			idx2 = line.find("\"", idx1)
			assert(idx2>0)
			last_Name = line[idx1:idx2]
		#F1 "analog_sa612A.sch" 60

	f.close()
	return names

def get_components(sch_name):
	f = open(sch_name, "rt")
	components = set()

	for line in f:
		if len(line)==0:
			continue
		if line[-1]=="\n":
			line=line[:-1]

		if line[0]=='U':
			idx = line.rfind(' ')
			assert(idx>0)
			U = line[idx+1:]
			assert(U not in components)
			components.add(U)
		pass

	f.close()
	return components

def replace_path(in_kicad_pcb, out_kicad_pcb, sch_infos):
	path_pattern = "    (path /"
	fin = open(in_kicad_pcb, "rt")
	fout = open(out_kicad_pcb, "wt")

	for line in fin:
		if len(line)==0:
			continue
		if line[-1]=="\n":
			line=line[:-1]

		if line[:len(path_pattern)]==path_pattern:
			sub = line[len(path_pattern):]
			sub_end = sub.rfind(')')
			assert(sub_end>0)
			U = sub[0:sub_end]
			found = False
			for sch in sch_infos:
				if U in sch.components:
					found = True
					line = path_pattern + sch.U + "/" + U + ")"
					break

			assert(found)

		print(line, file=fout)
		pass

	fin.close()
	fout.close()
	pass

def main():
	project_name = "analog_and_ad9958"

	sch_infos = get_sch_name_and_U(project_name+".sch")

	for info in sch_infos:
		info.components = get_components(info.name)
		print(info.name, info.U)
		print(info.components)

	replace_path(project_name+".kicad_pcb", project_name+".kicad_pcb.out", sch_infos)
	pass

main()