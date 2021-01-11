open Jest

let () = describe(
  "formatter",
  {
    open ExpectJs
    () => {
      let accented = `C'est - tout à fait _ édifiant toùt ça!`
      let no_accent = "C'est - tout a fait _ edifiant tout ca!"
      let no_punctuation = "Cest - tout a fait  edifiant tout ca"
      let html_id_friendly = "cest-tout-a-fait-edifiant-tout-ca"

      test("#to_html_id works 1", () =>
        expect(Formatter.to_html_id(accented)) |> toBe(html_id_friendly)
      )

      test("#to_html_id works 2", () =>
        expect(Formatter.to_html_id(no_accent)) |> toBe(html_id_friendly)
      )

      test("#to_html_id works 3", () =>
        expect(Formatter.to_html_id(no_punctuation)) |> toBe(html_id_friendly)
      )

      test("#to_html_id works 4", () =>
        expect(Formatter.to_html_id(html_id_friendly))
        |> toBe(html_id_friendly)
      )

      let core = "Don't touch me. Because I'm already pretty. That's all folks.\n"

      let leading_spaces = " " ++ core
      let trailing_spaces = core ++ " "
      let empty_lines = "\nDon't touch me. Because I'm already pretty.\n\n\nThat's all folks.\n\n"
      let lonely_spaces = " \nDon't touch me.   Because I'm already pretty.\n   \n \n That's all    folks.\n \n   "

      test("#format does not touch pretty strings", () =>
        expect(Formatter.format(core)) |> toBe(core)
      )

      test("#format trims leading spaces", () =>
        expect(Formatter.format(leading_spaces)) |> toBe(core)
      )

      test("#format trims trailing spaces", () =>
        expect(Formatter.format(trailing_spaces)) |> toBe(core)
      )

      test("#format trims empty lines", () =>
        expect(Formatter.format(empty_lines)) |> toBe(core)
      )

      test("#format trims lonely spaces", () =>
        expect(Formatter.format(lonely_spaces)) |> toBe(core)
      )
    }
  }
)
