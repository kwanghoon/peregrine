TEMPLATE = subdirs

SUBDIRS = peregrine \
	echo \
	website

peregrine.subdir = peregrine-desktop
echo.subdir = plugins/echo
website.subdir = plugins/website

peregrine.depends = echo website