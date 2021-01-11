open Jest

let () = describe(
  "file",
  {
    open ExpectJs
    () => {
      test("#extension works", () =>
        expect(File.extension("some_filename.json")) |> toBe(Some("json"))
      )

      test("#extension returns nothing if no delimiter", () =>
        expect(File.extension("no_extension")) |> toBe(None)
      )

      test("#make_absolute_filepath works", () =>
        expect(
          File.make_absolute_filepath(~ext_before="before", ~ext_after="after", "filename.before")
        ) |> toBe("filename.after")
      )

      test("#make_absolute_filepath works without replacement", () =>
        expect(
          File.make_absolute_filepath(~ext_before=".before", ~ext_after="", "filename.before")
        ) |> toBe("filename")
      )

      test("#read_text works", () =>
        expect(File.read_text("__tests__/resources/index.md"))
        |> toBe(Some("# {{executer}} executes {{executed}} using {{data_format}} data."))
      )

      test("#read_text returns nothing if file does not exist", () =>
        expect(File.read_text("does_not_exist")) |> toBe(None)
      )

      test("#read_json_data returns nothing if json file is malformed", () => {
        let file = "__tests__/resources/malformed_index.json"
        expect(File.read_json_data(file)) |> toBe(list{})
      })

      let json = Js.Json.parseExn(`{
  "executer": "ReScript",
  "executed" : "Mustache",
  "data_format": "complex js"
}`
      )

      test("#read_json_data works", () => {
        let file = "__tests__/resources/index.json"
        expect(File.read_json_data(file)) |> toEqual(list{json})
      })

      let json2 = Js.Json.parseExn(`{
  "executer": "Buble",
  "executed" : "Moustache",
  "data_format": "json list"
}`
      )

      test("#read_json_data works in batch mode", () => {
        let file = "__tests__/resources/list.json"
        expect(File.read_json_data(~batch=true, file)) |> toEqual(list{json, json2})
      })

      test("#read_yaml_data returns nothing if yaml file is malformed", () => {
        let file = "__tests__/resources/malformed_index.json.yml"
        expect(File.read_yaml_data(file)) |> toBe(list{})
      })

      test("#read_yaml_data works", () => {
        let file = "__tests__/resources/index.json.yml"
        expect(File.read_yaml_data(file)) |> toEqual(list{json})
      })

      test("#read_yaml_data works in batch mode", () => {
        let file = "__tests__/resources/list.json.yml"
        expect(File.read_yaml_data(~batch=true, file)) |> toEqual(list{json, json2})
      })

      testPromise("#read_js_data returns nothing if javascript file is malformed", () => {
        let file = "__tests__/resources/malformed_index.json.javascript"
        File.read_js_data(file) |> Sugar.then_resolve(actual_data =>
          expect(actual_data) |> toBe(list{})
        )
      })

      let js_data = %bs.raw(`{executer: "ReScript",
executed: "Mustache",
data_format: "computed  with javascript"
}`
      )

      testPromise("#read_js_data works", () => {
        let file = "__tests__/resources/index.json.javascript"
        File.read_js_data(file) |> Sugar.then_resolve(actual_data =>
          expect(actual_data) |> toEqual(list{js_data})
        )
      })

      let complex_js_data = %bs.raw(`function () {
  var fs = require('fs');
  var home = require('os').homedir();
  var dir = home + "/dem-rescript/__tests__/resources/";
  var contents = fs.readFileSync(dir + "index.json", 'utf8');
  return JSON.parse(contents);
}`
      )

      testPromise("#read_js_data works with more complex js data", () => {
        let file = "__tests__/resources/complex.json.javascript"
        File.read_js_data(file) |> Sugar.then_resolve(actual_data =>
          expect(actual_data) |> toEqual(list{complex_js_data()})
        )
      })

      let js_data_list = list{%bs.raw(`
{
  foo: 'bla',
  bar: 'bli'
}`), %bs.raw(`{
  foo: 'dlo',
  bar: 'dlu'
}`)
      }

      testPromise("#read_js_data works in batch mode", () => {
        let file = "__tests__/resources/list.json.javascript"
        File.read_js_data(~batch=true, file) |> Sugar.then_resolve(actual_data =>
          expect(actual_data) |> toEqual(js_data_list)
        )
      })

      let js_data = %bs.raw(`{
  foo: 'bla',
  bar: 'bli'
}`)

      testPromise("#read_js_data works with a js data promise", () => {
        let file = "__tests__/resources/promise.json.javascript"
        File.read_js_data(~batch=false, ~async=true, file) |> Sugar.then_resolve(actual_data =>
          expect(actual_data) |> toEqual(list{js_data})
        )
      })

      testPromise("#read_js_data works with a js data list promise", () => {
        let file = "__tests__/resources/list_promise.json.javascript"
        File.read_js_data(~batch=true, ~async=true, file) |> Sugar.then_resolve(actual_data =>
          expect(actual_data) |> toEqual(js_data_list)
        )
      })

      testPromise("#read_js_data in batch mode returns empty if the promise is not a list", () => {
        let file = "__tests__/resources/promise.json.javascript"
        File.read_js_data(~batch=true, ~async=true, file) |> Sugar.then_resolve(actual_data =>
          expect(actual_data) |> toEqual(list{})
        )
      })

      // TODO move in sugar_spec.re
      test("#objToJson works with a list", () => {
        let js_list = %bs.raw(`[{x: 1, y: 2}, {x: 1, y: 2}]`)
        let json_list = Js.Json.parseExn(`[{"x": 1, "y": 2}, {"x": 1 , "y": 2}]`)
        expect(Sugar.objToJson(js_list)) |> toEqual(json_list)
      })

      testPromise("#read_data returns nothing if filename extension is not supported", () =>
        File.read_data(Some("unsupported_extens.on")) |> Sugar.then_resolve(actual_data =>
          expect(actual_data) |> toEqual(list{})
        )
      )

      test("#read_sass_style returns nothing if sass file is malformed", () => {
        let file = "__tests__/resources/malformed_index.css.sass"
        expect(File.read_sass_style(file)) |> toBe(None)
      })

      test("#read_sass_style works", () => {
        let expected_css = `h1 {
  color: green; }
`
        expect(File.read_sass_style("__tests__/resources/index.css.sass")) |> toEqual(
          Some(expected_css)
        )
      })

      test("#find does not return false positive on unexisting path", () =>
        expect(File.find("unexisting")) |> toBe(None)
      )

      test("#find does not return false positive from unexisting directory", () =>
        expect(File.find(~root="unexisting_dir", "unexisting")) |> toBe(None)
      )

      test("#find does not return false positive on unexisting relative path", () =>
        expect(File.find(~root="__tests__/resources", "unexisting")) |> toBe(None)
      )

      test("#find works", () =>
        expect(File.find("__tests__/resources/index2"))
        |> toBe(Some("./__tests__/resources/index2.md"))
      )

      test("#find works with specified root", () =>
        expect(File.find(~root="__tests__/resources/inside", "blackhole"))
        |> toBe(Some("__tests__/resources/inside/blackhole.md"))
      )

      test("#find works with path with levels", () =>
        expect(File.find(~root="__tests__/resources", "inside/blackhole"))
        |> toBe(Some("__tests__/resources/inside/blackhole.md"))
      )

      test("#build_partials works with the empty list", () =>
        expect(File.build_partials(list{})) |> toEqual(Js.Dict.empty())
      )

      test("#build_partials works with unexisting partials", () =>
        expect(File.build_partials(list{"unexisting"}))
        |> toEqual(Js.Dict.fromList(list{("unexisting", "")}))
      )

      test("#build_partials works with unexisting partials and root", () => {
        let file = "./unexisting_dir/index.md"
        expect(File.build_partials(~root=file, list{"unexisting"}))
        |> toEqual(Js.Dict.fromList(list{("unexisting", "")}))
      })

      test("#build_partials works", () => {
        let file = "__tests__/resources/index2.md"
        let names = list{"follow", "inside/blackhole"}
        expect(File.build_partials(~root=file, names))
        |> toEqual(Js.Dict.fromList(list{
          ("follow", "# just {{verb}} it\n"),
          ("inside/blackhole", "# my name is {{name}}\n")
        }))
      })

      test("#build_partials works with multiple levels of dependency", () => {
        let file = "__tests__/resources/index3.md"
        let names = list{"follow2", "inside/blackhole2"}
        expect(File.build_partials(~root=file, names))
        |> toEqual(Js.Dict.fromList(list{
          ("follow2", "{{> follow}}\n"),
          ("inside/blackhole2", "{{> inside/blackhole}}\n"),
          ("follow", "# just {{verb}} it\n"),
          ("inside/blackhole", "# my name is {{name}}\n")
        }))
      })
    }
  }
)
