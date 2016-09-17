#!/bin/sh
# basic command-line call, using the default cppdoc-standard module.
../cppdoc -title="Basic Example" -classdir=classdoc -autorun -overwrite Code cppdoc_test /tmp/cppdoc_example/docs.html
echo '(Output was sent to /tmp/cppdoc_example/docs.html.)'
