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

        test("#read_data returns nothing if filename extension is not supported", () => {
            expect(File.read_data(Some("unsupported_extens.on")))
            |> toBe(None)
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
      }
    ),
  );
