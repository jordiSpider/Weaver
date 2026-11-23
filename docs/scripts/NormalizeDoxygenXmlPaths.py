import pathlib
import re
import sys


xml_directory = pathlib.Path(sys.argv[1])
location_file = re.compile(r'(<location\b[^>]*\bfile=")([^"]*)(")')

for xml_file in xml_directory.rglob("*.xml"):
	content = xml_file.read_bytes().decode("utf-8")
	normalized = location_file.sub(
		lambda match: match.group(1) + match.group(2).replace("\\", "/") + match.group(3),
		content,
	)
	if normalized != content:
		xml_file.write_bytes(normalized.encode("utf-8"))
