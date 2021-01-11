testDemVersion:
	dem --version > /tmp/version.actual
	diff /tmp/version.actual acceptance/version.expected

testDemHelp:
	dem --help > /tmp/help.actual
	diff /tmp/help.actual acceptance/help.expected

testDemHelpCompile:
	dem help compile > /tmp/help_compile.actual
	diff /tmp/help_compile.actual acceptance/help_compile.expected

testDem:
	dem > /tmp/no_argument.actual
	diff /tmp/no_argument.actual acceptance/no_argument.expected

testDemO:
	dem -o /tmp/o.actual.html
	diff /tmp/o.actual.html acceptance/o.expected.html

testDemOutput:
	dem --output /tmp/output.actual.html
	diff /tmp/output.actual.html acceptance/output.expected.html

testDemT:
	dem -t acceptance/resources/index.md > /tmp/t.actual
	diff /tmp/t.actual acceptance/t.expected

testDemText:
	dem --text acceptance/resources/index.md > /tmp/text.actual
	diff /tmp/text.actual acceptance/text.expected

testDemDjson:
	dem -t acceptance/resources/index.md -d acceptance/resources/index.json > /tmp/d_json.actual
	diff /tmp/d_json.actual acceptance/d_json.expected

testDemDataJson:
	dem -t acceptance/resources/index.md --data acceptance/resources/index.json > /tmp/data_json.actual
	diff /tmp/data_json.actual acceptance/data_json.expected

testDemDyaml:
	dem -t acceptance/resources/index.md -d acceptance/resources/index.json.yml > /tmp/d_yaml.actual
	diff /tmp/d_yaml.actual acceptance/d_yaml.expected

testDemDataYaml:
	dem -t acceptance/resources/index.md --data acceptance/resources/index.json.yml > /tmp/data_yaml.actual
	diff /tmp/data_yaml.actual acceptance/data_yaml.expected

testDemH:
	dem -h acceptance/resources/index.html.tpl > /tmp/h.actual
	diff /tmp/h.actual acceptance/h.expected

testDemHtmlTemplate:
	dem --html-template acceptance/resources/index.html.tpl > /tmp/html_template.actual
	diff /tmp/html_template.actual acceptance/html_template.expected

testDemScss:
	dem -s acceptance/resources/index.css > /tmp/s_css.actual
	diff /tmp/s_css.actual acceptance/s_css.expected

testDemStyleCss:
	dem --style acceptance/resources/index.css > /tmp/style_css.actual
	diff /tmp/style_css.actual acceptance/style_css.expected

testDemSsass:
	dem -s acceptance/resources/index.css.sass > /tmp/s_sass.actual
	diff /tmp/s_sass.actual acceptance/s_sass.expected

testDemStyleSass:
	dem --style acceptance/resources/index.css.sass > /tmp/style_sass.actual
	diff /tmp/style_sass.actual acceptance/style_sass.expected

testDemPublipost:
	dem -t acceptance/resources/index.md --publipost > /tmp/publipost.actual
	diff /tmp/publipost.actual acceptance/publipost.expected

testDemPublipostDO:
	dem -t acceptance/resources/index.md --publipost -d acceptance/resources/list.json -o "/tmp/publipost_do_{{executer}}.actual.html"
	diff /tmp/publipost_do_Buble.actual.html acceptance/publipost_do_Buble.expected.html
	diff /tmp/publipost_do_ReScript.actual.html acceptance/publipost_do_ReScript.expected.html

testDemAsyncD:
	dem --async -t acceptance/resources/index_foobar.md -d acceptance/resources/promise.json.js > /tmp/async_d.actual
	diff /tmp/async_d.actual acceptance/async_d.expected

testDemAsyncPublipostDO:
	dem --async -t acceptance/resources/index_foobar.md --publipost -d acceptance/resources/list_promise.json.js -o "/tmp/async_publipost_do_{{foo}}.actual.html"
	diff /tmp/async_publipost_do_bla.actual.html acceptance/async_publipost_do_bla.expected.html
	diff /tmp/async_publipost_do_dlo.actual.html acceptance/async_publipost_do_dlo.expected.html

testDemInstantiate:
	dem instantiate > /tmp/instantiate.actual
	diff /tmp/instantiate.actual acceptance/instantiate.expected

testDemInstantiateO:
	dem instantiate -t acceptance/resources/index.md -o /tmp/instantiate_o.actual.md
	diff /tmp/instantiate_o.actual.md acceptance/instantiate_o.expected.md

testDemInstantiateOutput:
	dem instantiate -t acceptance/resources/index.md --output /tmp/instantiate_output.actual.md
	diff /tmp/instantiate_output.actual.md acceptance/instantiate_output.expected.md

testDemInstantiateT:
	dem instantiate -t acceptance/resources/index.md > /tmp/instantiate_t.actual
	diff /tmp/instantiate_t.actual acceptance/instantiate_t.expected

testDemInstantiateText:
	dem instantiate --text acceptance/resources/index.md > /tmp/instantiate_text.actual
	diff /tmp/instantiate_text.actual acceptance/instantiate_text.expected

testDemInstantiateDjson:
	dem instantiate -t acceptance/resources/index.md -d acceptance/resources/index.json > /tmp/instantiate_d_json.actual
	diff /tmp/instantiate_d_json.actual acceptance/instantiate_d_json.expected

testDemInstantiateDataJson:
	dem instantiate -t acceptance/resources/index.md --data acceptance/resources/index.json > /tmp/instantiate_data_json.actual
	diff /tmp/instantiate_data_json.actual acceptance/instantiate_data_json.expected

testDemInstantiateDyaml:
	dem instantiate -t acceptance/resources/index.md -d acceptance/resources/index.json.yml > /tmp/instantiate_d_yaml.actual
	diff /tmp/instantiate_d_yaml.actual acceptance/instantiate_d_yaml.expected

testDemInstantiateDataYaml:
	dem instantiate -t acceptance/resources/index.md --data acceptance/resources/index.json.yml > /tmp/instantiate_data_yaml.actual
	diff /tmp/instantiate_data_yaml.actual acceptance/instantiate_data_yaml.expected

testDemInstantiateH:
	dem instantiate -t acceptance/resources/index.md -h acceptance/resources/index.html.tpl > /tmp/instantiate_h.actual
	diff /tmp/instantiate_h.actual acceptance/instantiate_h.expected

testDemInstantiateHtmlTemplate:
	dem instantiate -t acceptance/resources/index.md --html-template acceptance/resources/index.html.tpl > /tmp/instantiate_html_template.actual
	diff /tmp/instantiate_html_template.actual acceptance/instantiate_html_template.expected

testDemInstantiateScss:
	dem instantiate -t acceptance/resources/index.md -s acceptance/resources/index.css > /tmp/instantiate_s_css.actual
	diff /tmp/instantiate_s_css.actual acceptance/instantiate_s_css.expected

testDemInstantiateStyleCss:
	dem instantiate -t acceptance/resources/index.md --style acceptance/resources/index.css > /tmp/instantiate_style_css.actual
	diff /tmp/instantiate_style_css.actual acceptance/instantiate_style_css.expected

testDemInstantiateSsass:
	dem instantiate -t acceptance/resources/index.md -s acceptance/resources/index.css.sass > /tmp/instantiate_s_sass.actual
	diff /tmp/instantiate_s_sass.actual acceptance/instantiate_s_sass.expected

testDemInstantiateStyleSass:
	dem instantiate -t acceptance/resources/index.md --style acceptance/resources/index.css.sass > /tmp/instantiate_style_sass.actual
	diff /tmp/instantiate_style_sass.actual acceptance/instantiate_style_sass.expected

testDemInstantiatePublipostDO:
	dem instantiate -t acceptance/resources/index.md --publipost -d acceptance/resources/list.json -o "/tmp/instantiate_publipost_do_{{executer}}.actual.md"
	diff /tmp/instantiate_publipost_do_Buble.actual.md acceptance/instantiate_publipost_do_Buble.expected.md
	diff /tmp/instantiate_publipost_do_ReScript.actual.md acceptance/instantiate_publipost_do_ReScript.expected.md

testDemInstantiateAsyncD:
	dem instantiate --async -t acceptance/resources/index_foobar.md -d acceptance/resources/promise.json.js > /tmp/instantiate_async_d.actual
	diff /tmp/instantiate_async_d.actual acceptance/instantiate_async_d.expected

testDemInstantiateAsyncPublipostDO:
	dem instantiate --async -t acceptance/resources/index_foobar.md --publipost -d acceptance/resources/list_promise.json.js -o "/tmp/instantiate_async_publipost_do_{{foo}}.actual.md"
	diff /tmp/instantiate_async_publipost_do_bla.actual.md acceptance/instantiate_async_publipost_do_bla.expected.md
	diff /tmp/instantiate_async_publipost_do_dlo.actual.md acceptance/instantiate_async_publipost_do_dlo.expected.md

testDemCompile:
	dem compile > /tmp/compile.actual
	diff /tmp/compile.actual acceptance/compile.expected

testDemCompileO:
	dem compile -o /tmp/compile_o.actual.html
	diff /tmp/compile_o.actual.html acceptance/compile_o.expected.html

testDemCompileOutput:
	dem compile --output /tmp/compile_output.actual.html
	diff /tmp/compile_output.actual.html acceptance/compile_output.expected.html

testDemCompileT:
	dem compile -t acceptance/resources/index.md > /tmp/compile_t.actual
	diff /tmp/compile_t.actual acceptance/compile_t.expected

testDemCompileText:
	dem compile --text acceptance/resources/index.md > /tmp/compile_text.actual
	diff /tmp/compile_text.actual acceptance/compile_text.expected

testDemCompileDjson:
	dem compile -t acceptance/resources/index.md -d acceptance/resources/index.json > /tmp/compile_d_json.actual
	diff /tmp/compile_d_json.actual acceptance/compile_d_json.expected

testDemCompileDataJson:
	dem compile -t acceptance/resources/index.md --data acceptance/resources/index.json > /tmp/compile_data_json.actual
	diff /tmp/compile_data_json.actual acceptance/compile_data_json.expected

testDemCompileDyaml:
	dem compile -t acceptance/resources/index.md -d acceptance/resources/index.json.yml > /tmp/compile_d_yaml.actual
	diff /tmp/compile_d_yaml.actual acceptance/compile_d_yaml.expected

testDemCompileDataYaml:
	dem compile -t acceptance/resources/index.md --data acceptance/resources/index.json.yml > /tmp/compile_data_yaml.actual
	diff /tmp/compile_data_yaml.actual acceptance/compile_data_yaml.expected

testDemCompileH:
	dem compile -h acceptance/resources/index.html.tpl > /tmp/compile_h.actual
	diff /tmp/compile_h.actual acceptance/compile_h.expected

testDemCompileHtmlTemplate:
	dem compile --html-template acceptance/resources/index.html.tpl > /tmp/compile_html_template.actual
	diff /tmp/compile_html_template.actual acceptance/compile_html_template.expected

testDemCompileScss:
	dem compile -s acceptance/resources/index.css > /tmp/compile_s_css.actual
	diff /tmp/compile_s_css.actual acceptance/compile_s_css.expected

testDemCompileStyleCss:
	dem compile --style acceptance/resources/index.css > /tmp/compile_style_css.actual
	diff /tmp/compile_style_css.actual acceptance/compile_style_css.expected

testDemCompileSsass:
	dem compile -s acceptance/resources/index.css.sass > /tmp/compile_s_sass.actual
	diff /tmp/compile_s_sass.actual acceptance/compile_s_sass.expected

testDemCompileStyleSass:
	dem compile --style acceptance/resources/index.css.sass > /tmp/compile_style_sass.actual
	diff /tmp/compile_style_sass.actual acceptance/compile_style_sass.expected

testDemCompilePublipostDO:
	dem compile -t acceptance/resources/index.md --publipost -d acceptance/resources/list.json -o "/tmp/compile_publipost_do_{{executer}}.actual.html"
	diff /tmp/compile_publipost_do_Buble.actual.html acceptance/compile_publipost_do_Buble.expected.html
	diff /tmp/compile_publipost_do_ReScript.actual.html acceptance/compile_publipost_do_ReScript.expected.html

testDemCompileAsyncD:
	dem compile --async -t acceptance/resources/index_foobar.md -d acceptance/resources/promise.json.js > /tmp/compile_async_d.actual
	diff /tmp/compile_async_d.actual acceptance/compile_async_d.expected

testDemCompileAsyncPublipostDO:
	dem compile --async -t acceptance/resources/index_foobar.md --publipost -d acceptance/resources/list_promise.json.js -o "/tmp/compile_async_publipost_do_{{foo}}.actual.html"
	diff /tmp/compile_async_publipost_do_bla.actual.html acceptance/compile_async_publipost_do_bla.expected.html
	diff /tmp/compile_async_publipost_do_dlo.actual.html acceptance/compile_async_publipost_do_dlo.expected.html


test: testDem \
	    testDemO testDemOutput \
	    testDemT testDemText \
			testDemDjson testDemDataJson \
  	  testDemDyaml testDemDataYaml \
			testDemH testDemHtmlTemplate \
			testDemScss testDemStyleCss \
			testDemPublipost testDemPublipostDO \
			testDemAsyncD testDemAsyncPublipostDO \
			testDemInstantiate \
	    testDemInstantiateO testDemInstantiateOutput \
	    testDemInstantiateT testDemInstantiateText \
			testDemInstantiateDjson testDemInstantiateDataJson \
  	  testDemInstantiateDyaml testDemInstantiateDataYaml \
			testDemInstantiateH testDemInstantiateHtmlTemplate \
			testDemInstantiateScss testDemInstantiateStyleCss \
			testDemInstantiatePublipostDO \
			testDemInstantiateAsyncD \
			testDemInstantiateAsyncPublipostDO \
			testDemCompile \
	    testDemCompileO testDemCompileOutput \
	    testDemCompileT testDemCompileText \
			testDemCompileDjson testDemCompileDataJson \
  	  testDemCompileDyaml testDemCompileDataYaml \
			testDemCompileH testDemCompileHtmlTemplate \
			testDemCompileScss testDemCompileStyleCss \
			testDemCompilePublipostDO \
			testDemCompileAsyncD \
			testDemCompileAsyncPublipostDO \
			testDemHelp \
			testDemHelpCompile \
			testDemVersion

	# Not automatically tested:
	# COMMANDS
	#        print
	# OPTIONS (for all commands)
	#        -w, --watch
