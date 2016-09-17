REM creates a custom index with all documentation, using files for the header, footer, and per-item, and linking to a website for the source-file links.  Uses the cppdoc-old module.

..\CppDoc -module=cppdoc-old -autorun -overwrite -classdir=classdoc -header="ex4-header.txt" -item="ex4-peritem.txt" -footer="ex4-footer.txt" -sourcedir="http://www.cppdoc.com/example" Code c:\temp\cppdoc_example\docs.html
