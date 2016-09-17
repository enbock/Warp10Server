#!/bin/sh
# demonstrates the use of custom header and footer, with separate index pages for each letter and with deprecations disabled.

../cppdoc -classdir=classdoc -autorun -overwrite -header="<html><head><title>Custom Documentation</title><h1>This is a custom header.</h1></head><body>" -footer="</body></html>" -enable-deprecations=false -separate-index-pages Code cppdoc_test /tmp/cppdoc_example/docs.html
echo '(Output was sent to /tmp/cppdoc_example/docs.html.)'
