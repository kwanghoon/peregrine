TEMPLATE = subdirs

SUBDIRS = peregrine \
	echo \
	website \
	file

peregrine.subdir = peregrine-desktop
echo.subdir = plugins/echo
website.subdir = plugins/website
file.subdir = plugins/file

peregrine.depends = echo website file