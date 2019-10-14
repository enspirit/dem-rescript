open Jest;

let () =
  describe(
    "app",
    ExpectJs.(
      () => {
        let text = "# {{executer}} executes {{executed}} using {{data_format}} data.";

        let more_structured_text = {|# Title

Some introduction ...

## Section 1 title

Some paragraph ...

### Subsection title

Some paragraph ...

## Section 2 title

Some paragraph ...

Another paragraph ...
|}

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

        let js_data = [%bs.raw {|{executer: "BuckleScript",
executed: "Mustache",
data_format: "js"
}|}];

        let complex_js_data = [%bs.raw {|function () {
  var fs = require('fs');
  var home = require('os').homedir();
  var dir = home + "/dem-bs/__tests__/resources/";
  var contents = fs.readFileSync(dir + "index.json", 'utf8');
  return JSON.parse(contents);
}|}];

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
            |> toBe({|<article id="executes-using-data"><h1>executes using data.</h1></article>
|})
          }
        );

        test("#compile_body works with json data", () => {
            expect(App.compile_body(Some(text), Some(json_data)))
            |> toBe({|<article id="bucklescript-executes-mustache-using-json-data">
  <h1>BuckleScript executes Mustache using json data.</h1>
</article>
|})
          }
        );

        test("#compile_body works with yaml data", () => {
            expect(App.compile_body(Some(text), Some(yaml_data)))
            |> toBe({|<article id="bucklescript-executes-mustache-using-yaml-data">
  <h1>BuckleScript executes Mustache using yaml data.</h1>
</article>
|})
          }
        );

        test("#compile_body correctly inject article and section tags with more structured text", () => {
            expect(App.compile_body(Some(more_structured_text), None))
            |> toBe({|<article id="title">
  <h1>Title</h1>
  <p>Some introduction ...</p>
  <section id="section-1-title">
    <h2>Section 1 title</h2>
    <p>Some paragraph ...</p>
    <section id="subsection-title">
      <h3>Subsection title</h3>
      <p>Some paragraph ...</p>
    </section>
  </section>
  <section id="section-2-title">
    <h2>Section 2 title</h2>
    <p>Some paragraph ...</p>
    <p>Another paragraph ...</p>
  </section>
</article>
|})
          }
        );

        test("#compile works with default when empty arguments given", () => {
            expect(App.compile(None, None, None, None))
            |> toBe({|<html>
  <head>
    <style></style>
  </head>
  <body></body>
</html>
|});
          }
        );

        test("#compile works with default when no body given", () => {
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
  <body></body>
</html>
|})
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
    <article id="executes-using-data"><h1>executes using data.</h1></article>
  </body>
</html>
|})
          }
        );

        test("#compile works with default when no text given", () => {
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
  <body></body>
</html>
|})
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
    <article id="bucklescript-executes-mustache-using-json-data">
      <h1>BuckleScript executes Mustache using json data.</h1>
    </article>
  </body>
</html>
|})
          }
        );

        test("#compile works", () => {
            expect(App.compile(Some(template), Some(style), Some(text), Some(js_data)))
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
    <article id="bucklescript-executes-mustache-using-js-data">
      <h1>BuckleScript executes Mustache using js data.</h1>
    </article>
  </body>
</html>
|})
          }
        );

        test("#compile works", () => {
          expect(App.compile(Some(template), Some(style), Some(text), Some(complex_js_data())))
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
    <article id="bucklescript-executes-mustache-using-complex-js-data">
      <h1>BuckleScript executes Mustache using complex js data.</h1>
    </article>
  </body>
</html>
|})
          }
        );
      }
    ),
  );
