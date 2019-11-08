open Jest;

let () =
  describe(
    "file",
    ExpectJs.(
      () => {
        test("#extension works", () => {
            expect(File.extension("some_filename.json"))
            |> toBe(Some("json"))
          }
        );

        test("#extension returns nothing if no delimiter", () => {
            expect(File.extension("no_extension"))
            |> toBe(None)
          }
        );

        test("#read_text works", () => {
            expect(File.read_text("__tests__/resources/index.md"))
            |> toBe(Some("# {{executer}} executes {{executed}} using {{data_format}} data.
"))
          }
        );

        test("#read_text returns nothing if file does not exist", () => {
            expect(File.read_text("does_not_exist"))
            |> toBe(None)
          }
        );

        test("#read_json_data returns nothing if json file is malformed", () => {
            expect(File.read_json_data("__tests__/resources/malformed_index.json"))
            |> toBe(None)
          }
        );

        test("#read_yaml_data returns nothing if yaml file is malformed", () => {
            expect(File.read_yaml_data("__tests__/resources/malformed_index.json.yml"))
            |> toBe(None)
          }
        );

        test("#read_js_data returns nothing if javascript file is malformed", () => {
            expect(File.read_js_data("__tests__/resources/malformed_index.json.javascript"))
            |> toBe(None)
          }
        );

        let js_data = [%bs.raw {|{executer: "BuckleScript",
executed: "Mustache",
data_format: "computed  with javascript"
}|}];

        test("#read_js_data works", () => {
            expect(File.read_js_data("__tests__/resources/index.json.javascript"))
            |> toEqual(Some(js_data))
          }
        );

        let complex_js_data = [%bs.raw {|function () {
  var fs = require('fs');
  var home = require('os').homedir();
  var dir = home + "/dem-bs/__tests__/resources/";
  var contents = fs.readFileSync(dir + "index.json", 'utf8');
  return JSON.parse(contents);
}|}];

        test("#read_js_data works with more complex js data", () => {
            expect(File.read_js_data("__tests__/resources/complex.json.javascript"))
            |> toEqual(Some(complex_js_data()))
          }
        );

        test("#read_data returns nothing if filename extension is not supported", () => {
            expect(File.read_data(Some("unsupported_extens.on")))
            |> toBe(None)
          }
        );

        test("#find does not return false positive on unexisting path", () => {
            expect(File.find("unexisting"))
            |> toBe(None)
          }
        );

        test("#find does not return false positive from unexisting directory", () => {
            expect(File.find(~root="unexisting_dir", "unexisting"))
            |> toBe(None)
          }
        );

        test("#find does not return false positive on unexisting relative path", () => {
            expect(File.find(~root="__tests__/resources", "unexisting"))
            |> toBe(None)
          }
        );

        test("#find works", () => {
            expect(File.find("__tests__/resources/index2"))
            |> toBe(Some("./__tests__/resources/index2.md"))
          }
        );

        test("#find works with specified root", () => {
            expect(File.find(~root="__tests__/resources/inside", "blackhole"))
            |> toBe(Some("__tests__/resources/inside/blackhole.md"))
          }
        );

        test("#find works with path with levels", () => {
            expect(File.find(~root="__tests__/resources", "inside/blackhole"))
            |> toBe(Some("__tests__/resources/inside/blackhole.md"))
          }
        );

        test("#build_partials works with the empty list", () => {
            expect(File.build_partials([]))
            |> toEqual(Js.Dict.empty())
          }
        );

        test("#build_partials works with unexisting partials", () => {
            expect(File.build_partials(["unexisting"]))
            |> toEqual(Js.Dict.fromList([("unexisting", "")]))
          }
        );

        test("#build_partials works with unexisting partials and root", () => {
            expect(File.build_partials(~root="./unexisting_dir/index.md", ["unexisting"]))
            |> toEqual(Js.Dict.fromList([("unexisting", "")]))
          }
        );

        test("#build_partials works", () => {
            expect(File.build_partials(~root="__tests__/resources/index2.md", ["follow", "inside/blackhole"]))
            |> toEqual(Js.Dict.fromList([("follow", "# just {{verb}} it\n"), ("inside/blackhole", "# my name is {{name}}\n")]))
          }
        );

        test("#build_partials works with multiple levels of dependency", () => {
            expect(File.build_partials(~root="__tests__/resources/index3.md", ["follow2", "inside/blackhole2"]))
            |> toEqual(Js.Dict.fromList([
              ("follow2", "{{> follow}}\n"),
              ("inside/blackhole2", "{{> inside/blackhole}}\n"),
              ("follow", "# just {{verb}} it\n"),
              ("inside/blackhole", "# my name is {{name}}\n")
            ]))
          }
        );
      }
    ),
  );
