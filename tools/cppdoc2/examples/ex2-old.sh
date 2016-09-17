#!/bin/sh
# generates output from the old (cppdoc-old) module, with a hierarchical class index.
../cppdoc -module=cppdoc-old -hier -title="Basic Example" -company="Your Company" -classdir=classdoc -autorun -overwrite Code /tmp/cppdoc_example/docs.html
echo '(Output was sent to /tmp/cppdoc_example/docs.html.)'
