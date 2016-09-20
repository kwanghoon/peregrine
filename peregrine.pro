TEMPLATE = subdirs

SUBDIRS = peregrine \
	echo \
	file \
	website \
	calculator \
	console-testbed \
	peregrine-test

peregrine.subdir = peregrine-desktop
echo.subdir = plugins/echo
file.subdir = plugins/file
website.subdir = plugins/website
calculator.subdir = plugins/calculator

peregrine.depends = echo file website calculator
