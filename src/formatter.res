let remove_accents = s =>
  Js.String.normalizeByForm("NFD", s)
  |> Js.String.replaceByRe(%re("/[\u0300-\u036f]/g"), "")

let remove_punctuation = s =>
  Js.String.replaceByRe(%re("/[^a-zA-Z0-9\ \-]/g"), "", s)

let to_html_id = s =>
  s->remove_accents
  |> remove_punctuation
  |> String.lowercase
  |> Js.String.replaceByRe(%re("/[\ \-]+/g"), "-") // replace spaces by a hyphen

let format = s => Prettier.format(s, {"parser": "html"})
