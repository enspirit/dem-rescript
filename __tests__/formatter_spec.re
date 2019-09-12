open Jest;

let () =
  describe(
    "prettier",
    ExpectJs.(
      () => {
        let core = "Don't touch me. Because I'm already pretty. That's all folks.\n";

        let leading_spaces = " " ++ core;
        let trailing_spaces = core ++ " ";
        let empty_lines = "\nDon't touch me. Because I'm already pretty.\n\n\nThat's all folks.\n\n";
        let lonely_spaces = " \nDon't touch me.   Because I'm already pretty.\n   \n \n That's all    folks.\n \n   ";

        test("#format does not touch pretty strings", () => {
            expect(Formatter.format(core))
            |> toBe(core)
          }
        );

        test("#format trims leading spaces", () => {
            expect(Formatter.format(leading_spaces))
            |> toBe(core)
          }
        );

        test("#format trims trailing spaces", () => {
            expect(Formatter.format(trailing_spaces))
            |> toBe(core)
          }
        );

        test("#format trims empty lines", () => {
            expect(Formatter.format(empty_lines))
            |> toBe(core)
          }
        );

        test("#format trims lonely spaces", () => {
            expect(Formatter.format(lonely_spaces))
            |> toBe(core)
          }
        );
      }
    ),
  );
