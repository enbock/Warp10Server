#!/bin/bash

./cppdoc \
	-title="ulibs" \
	-company="Endre Bock" \
	-include-const  \
	-source-code-tab-size="2" \
	-namespace-as-project \
	-include-anonymous \
	-document-preprocessor \
	-additional-header="<style>\r\nbody, td, tr, th, a, em\r\n{\r\n  font-family: Verdana;\r\n  font-size:  10pt;\r\n}\r\ncode, pre\r\n{\r\n  font-family: Lucida Console, New Courier, courier;\r\n  font-size: 9pt;\r\n}\r\na \r\n{ \r\n  text-decoration: none; \r\n  color: #0000a0;\r\n}\r\npre\r\n{\r\n  border: 1px dotted black;\r\n  background-color: #f0f0f0;\r\n  padding: 2 px;\r\n  font-size: 9pt;\r\n}\r\n</style>" \
	-classdir="projects" \
	-module="cppdoc-standard" \
	-extensions="cpp,*" \
	-languages="cpp=cpp" \
	-enable-author="true" \
	-enable-deprecations="true" \
	-enable-since="true" \
	-enable-version="true" \
	-file-links-for-globals="false" \
	-generate-deprecations-list="true" \
	-generate-hierarchy="true" \
	-header-background-dark="#ccccff" \
	-header-background-light="#eeeeff" \
	-include-private="false" \
	-include-protected="true" \
	-index-file-base="index" \
	-overview-html="overview.html" \
	-reduce-summary-font="true" \
	-selected-text-background="navy" \
	-selected-text-foreground="white" \
	-separate-index-pages="false" \
	-show-cppdoc-version="false" \
	-show-timestamp="false" \
	-summary-html="project.html" \
	-suppress-details="false" \
	-suppress-frames-links="false" \
	-table-background="white" \
	-wrap-long-lines="false" \
	"../../ulib/include" "../../warp10server/include" "../../../local/www/c++-dev/index.html" \
	-verbose

