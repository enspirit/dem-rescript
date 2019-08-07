let markdownIt = MarkdownIt.createMarkdownIt();

Js.log(MarkdownIt.render(markdownIt, "# Bucklescript executes markdown-it."));

Js.log(Mustache.render(
          "{{executer}} executes {{executed}}.",
          { "executer" : "Bucklescript", "executed" : "Mustache" }
        )
      );
