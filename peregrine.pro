TEMPLATE = subdirs

SUBDIRS = peregrine \
	echo \
	file \
	website

peregrine.subdir = peregrine-desktop
echo.subdir = plugins/echo
file.subdir = plugins/file
website.subdir = plugins/website

peregrine.depends = echo file website