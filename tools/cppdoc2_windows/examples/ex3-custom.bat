REM demonstrates the use of custom header and footer, with separate index pages for each letter and with deprecations disabled.

..\CppDoc -classdir=classdoc -autorun -overwrite -header="<html><head><title>Custom Documentation</title><h1>This is a custom header.</h1></head><body>" -footer="</body></html>" -enable-deprecations=false -separate-index-pages Code cppdoc_test c:\temp\cppdoc_example\docs.html
