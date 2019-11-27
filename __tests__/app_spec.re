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

{{> another_subsection}}

## Section 2 title

Some paragraph ...

Another paragraph ...

{{> another_section}}
|}

        let another_subsection = {|### Another subsection title

Some paragraph of another subsection ...

Another paragraph of another subsection ...
|}

        let another_section = {|## Another section title

Some paragraph of another section ...

Another paragraph of another section ...
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

        let partials = Js.Dict.fromList([("another_subsection", another_subsection), ("another_section", another_section)]);

        test("#partials_dependencies works", () => {
          expect(App.partials_dependencies(more_structured_text))
          |> toEqual(["another_subsection", "another_section"])
        })

        test("#partials_dependencies is empty when there is no partial call", () => {
          expect(App.partials_dependencies(text))
          |> toBe([])
          }
        );

        test("#instantiate_body works with default when empty arguments given", () => {
            expect(App.instantiate_body(None, None, None))
            |> toBe("")
          }
        );

        test("#instantiate_body works with default when no body given", () => {
            expect(App.instantiate_body(None, Some(json_data), None))
            |> toBe("")
          }
        );

        test("#instantiate_body works with default when no data given", () => {
            expect(App.instantiate_body(Some(text), None, None))
            |> toBe("#  executes  using  data.")
          }
        );

        test("#instantiate_body works with json data", () => {
            expect(App.instantiate_body(Some(text), Some(json_data), None))
            |> toBe("# BuckleScript executes Mustache using json data.")
          }
        );

        test("#instantiate_body works with yaml data", () => {
            expect(App.instantiate_body(Some(text), Some(yaml_data), None))
            |> toBe("# BuckleScript executes Mustache using yaml data.")
          }
        );

        test("#instantiate_body correctly inject article and section tags with more structured text", () => {
            expect(App.instantiate_body(Some(more_structured_text), None, Some(partials)))
            |> toBe({|# Title

Some introduction ...

## Section 1 title

Some paragraph ...

### Subsection title

Some paragraph ...

### Another subsection title

Some paragraph of another subsection ...

Another paragraph of another subsection ...

## Section 2 title

Some paragraph ...

Another paragraph ...

## Another section title

Some paragraph of another section ...

Another paragraph of another section ...
|})
          }
        );

        test("#compile works with default when empty arguments given", () => {
            expect(App.compile(None, None, None, None, None))
            |> toBe({|<html>
  <head>
    <style></style>
  </head>
  <body></body>
</html>
|});
          }
        );

        test("#compile works with default when only data given", () => {
            expect(App.compile(None, None, None, Some(json_data), None))
            |> toBe({|<html>
  <head>
    <style></style>
  </head>
  <body></body>
</html>
|})
          }
        );

        test("#compile works with default when only text given", () => {
            expect(App.compile(None, None, Some(text), None, None))
            |> toBe({|<html>
  <head>
    <style></style>
  </head>
  <body>
    <article id="executes-using-data"><h1>executes using data.</h1></article>
  </body>
</html>
|})
          }
        );

        test("#compile works with json data", () => {
            expect(App.compile(None, None, Some(text), Some(json_data), None))
            |> toBe({|<html>
  <head>
    <style></style>
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

        test("#compile works with yaml data", () => {
            expect(App.compile(None, None, Some(text), Some(yaml_data), None))
            |> toBe({|<html>
  <head>
    <style></style>
  </head>
  <body>
    <article id="bucklescript-executes-mustache-using-yaml-data">
      <h1>BuckleScript executes Mustache using yaml data.</h1>
    </article>
  </body>
</html>
|})
          }
        );

        test("#compile correctly inject article and section tags with more structured text", () => {
            expect(App.compile(None, None, Some(more_structured_text), None, Some(partials)))
            |> toBe({|<html>
  <head>
    <style></style>
  </head>
  <body>
    <article id="title">
      <h1>Title</h1>
      <p>Some introduction ...</p>
      <section id="section-1-title">
        <h2>Section 1 title</h2>
        <p>Some paragraph ...</p>
        <section id="subsection-title">
          <h3>Subsection title</h3>
          <p>Some paragraph ...</p>
        </section>
        <section id="another-subsection-title">
          <h3>Another subsection title</h3>
          <p>Some paragraph of another subsection ...</p>
          <p>Another paragraph of another subsection ...</p>
        </section>
      </section>
      <section id="section-2-title">
        <h2>Section 2 title</h2>
        <p>Some paragraph ...</p>
        <p>Another paragraph ...</p>
      </section>
      <section id="another-section-title">
        <h2>Another section title</h2>
        <p>Some paragraph of another section ...</p>
        <p>Another paragraph of another section ...</p>
      </section>
    </article>
  </body>
</html>
|})
          }
        );

        test("#compile works with default when no body given", () => {
            expect(App.compile(Some(template), Some(style), None, Some(json_data), None))
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
            expect(App.compile(Some(template), Some(style), Some(text), None, None))
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
            expect(App.compile(Some(template), Some(style), None, Some(json_data), None))
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

        test("#compile works with json data", () => {
            expect(App.compile(Some(template), Some(style), Some(text), Some(json_data), None))
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

        test("#compile works with js data", () => {
            expect(App.compile(Some(template), Some(style), Some(text), Some(js_data), None))
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

        test("#compile works with complex js data", () => {
          expect(App.compile(Some(template), Some(style), Some(text), Some(complex_js_data()), None))
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
