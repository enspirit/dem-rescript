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

        test("#compile_body works with default when empty arguments given", () => {
            expect(App.compile_body(None, None))
            |> toBe("")
          }
        );

        test("#compile_body works with default when no body given", () => {
            expect(App.compile_body(None, Some(json_data)))
            |> toBe("")
          }
        );

        test("#compile_body works with default when no data given", () => {
            expect(App.compile_body(Some(text), None))
            |> toBe("<h1>executes  using  data.</h1>")
          }
        );

        test("#compile_body works with json data", () => {
            expect(App.compile_body(Some(text), Some(json_data)))
            |> toBe("<h1>BuckleScript executes Mustache using json data.</h1>")
          }
        );

        test("#compile_body works with yaml data", () => {
            expect(App.compile_body(Some(text), Some(yaml_data)))
            |> toBe("<h1>BuckleScript executes Mustache using yaml data.</h1>")
          }
        );

        test("#compile works with default when empty arguments given", () => {
            expect(App.compile(None, None, None, None))
            |> toBe({|<html>
  <head>
    <style>
    </style>
  </head>
  <body>
  </body>
</html>|});
          }
        );

        test("#compile works with default when no body given", () => {
            expect(
              Js.String.trim(
                App.compile(Some(template), Some(style), None, Some(json_data))
              )
            )
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
  </body>
</html>|})
          }
        );

        test("#compile works with default when no data given", () => {
            expect(App.compile(Some(template), Some(style), Some(text), None))
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
    <h1>executes  using  data.</h1>
  </body>
</html>|})
          }
        );

        test("#compile works with default when no data given", () => {
            expect(App.compile(Some(template), Some(style), None, Some(json_data)))
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
  </body>
</html>|})
          }
        );

        test("#compile works", () => {
            expect(App.compile(Some(template), Some(style), Some(text), Some(json_data)))
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
