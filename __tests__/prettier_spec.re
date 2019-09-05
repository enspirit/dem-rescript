open Jest;

let () =
  describe(
    "prettier",
    ExpectJs.(
      () => {
        let core = {|Don't touch me.
  Because I'm already pretty.
That's all folks.|};

        let leading_spaces = " " ++ core;
        let trailing_spaces = core ++ " ";
        let empty_lines = "\n" ++ core ++ "\n\n\n" ++ core ++ "\n";
        let lonely_spaces = " \n" ++ core ++ "\n  \n" ++ core ++ "\n ";

        test("#format does not touch pretty strings", () => {
            expect(Prettier.format(core))
            |> toBe(core)
          }
        );

        test("#format trims leading spaces", () => {
            expect(Prettier.format(leading_spaces))
            |> toBe(core)
          }
        );

        test("#format trims trailing spaces", () => {
            expect(Prettier.format(trailing_spaces))
            |> toBe(core)
          }
        );

        test("#format trims empty lines", () => {
            expect(Prettier.format(empty_lines))
            |> toBe(core ++ "\n" ++ core)
          }
        );

        test("#format trims lonely spaces", () => {
            expect(Prettier.format(lonely_spaces))
            |> toBe(core ++ "\n" ++ core)
          }
        );
      }
    ),
  );
