import os
import shutil
import stat
import sys
from pathlib import Path


xml_directory = Path(sys.argv[1])


def remove_readonly(function, path, error):
	os.chmod(path, stat.S_IWRITE)
	function(path)


if xml_directory.exists():
	shutil.rmtree(xml_directory, onerror=remove_readonly)
