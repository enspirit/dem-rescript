open Jest;

let () =
  describe(
    "app",
    ExpectJs.(
      () => {
        let text = "# {{executer}} executes {{executed}} using {{data_format}} data.";

        let json_data = Js.Json.parseExn({|{
          "executer": "BuckleScript",
          "executed": "Mustache",
          "data_format": "json"
        }|});

        let yaml_data = {|---
          executer: BuckleScript
          executed: Mustache
          data_format: yaml
        |} -> Yaml.yamlParse();

        let template =
{|<html>
  <head>
    <style>
      {{{compiled_style}}}
    </style>
  </head>
  <body>
    {{{compiled_body}}}
  </body>
</html>|};

        let style =
{|h1 {
  color: green;
}|};

        test("#compile_body", () => {
            expect(App.compile_body(text, json_data))
            |> toBe("<h1>BuckleScript executes Mustache using json data.</h1>\n")
          }
        );

        test("#compile_body", () => {
            expect(App.compile_body(text, yaml_data))
            |> toBe("<h1>BuckleScript executes Mustache using yaml data.</h1>\n")
          }
        );

        test("#compile", () => {
            expect(App.compile(template, style, text, json_data))
            |> toBe(
{|<html>
  <head>
    <style>
      h1 {
  color: green;
}
    </style>
  </head>
  <body>
    <h1>BuckleScript executes Mustache using json data.</h1>

  </body>
</html>|})
          }
        );
      }
    ),
  );
